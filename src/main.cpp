#include <Arduino.h>
#include <RTClib.h>
#include <TimeLib.h>
#include "PresentTime.h"
#include "DisplayPanel.h"
#include "CommandInterface.h"

// set this in AudioConfig.h or here after installing https://github.com/pschatzmann/arduino-libhelix.git
#define USE_HELIX
#include "SPIFFS.h"
#include "AudioTools.h"
#include "AudioCodecs/CodecMP3Helix.h"

RTC_DS3231 rtc;
PresentTime presentTime;
TimeElements_t destinationTime;
DisplayPanel displayPanel;
CommandInterface commandInterface;

TaskHandle_t updateTimeTaskHandle = NULL;
TaskHandle_t playSoundTaskHandle = NULL;

I2SStream i2s(0); // final output of decoded stream
VolumeStream out(i2s); // stream to control volume

uint8_t playSound(const char * file)
{
    File soundFile = SPIFFS.open(file);
    if(!soundFile || soundFile.isDirectory()){
        Serial.println("Failed to open sound file for reading");
        return 1;
    }

    MP3DecoderHelix mp3Decoder;
    EncodedAudioStream decoder(out, mp3Decoder);

    StreamCopy copier; // copies sound into i2s

    // setup I2S based on sampling rate provided by decoder
    decoder.setNotifyAudioChange(i2s);
    decoder.begin();

    // begin copy
    copier.begin(decoder, soundFile);

    while(copier.copy()){}

    copier.end();
    decoder.end();
    soundFile.close();

    return 0;
}

void playSoundTask( void * parameter )
{
    playSound((const char *)parameter);
    vTaskDelete( NULL );
}

void playSoundAsync(const char * file)
{
    xTaskCreate(
        playSoundTask,          /* Task function. */
        "playSoundTask",        /* String with name of task. */
        10000,                  /* Stack size in bytes. */
        ( void * ) file,        /* Parameter passed as input of the task */
        3,                      /* Priority of the task. */
        &playSoundTaskHandle    /* Task handle. */
    );
}

void beepTask( void * parameter )
{
    File beep = SPIFFS.open("/beep.mp3");
    if(!beep || beep.isDirectory()){
        Serial.println("Failed to open beep.mp3 for reading");
        vTaskDelete( NULL );
    }
    beep.close();

    MP3DecoderHelix mp3Decoder;
    EncodedAudioStream decoder(out, mp3Decoder); // Decoding stream

    StreamCopy copier; // copies sound into i2s

    //----------------------------------

    decoder.setNotifyAudioChange(i2s);
    decoder.begin();

    copier.begin(decoder, beep);

    while(true)
    {
        beep = SPIFFS.open("/beep.mp3");
        while(!presentTime.now().Halfsecond % 2){}
        while(copier.copy()){}
        beep.close();
    }

    copier.end();
    decoder.end();
    vTaskDelete( NULL );
}

void onSetSegment(std::string& data)
{
    if(data.length() < 1) return;
    // Indicates whether segment should be set or cleared
    bool isSetSegment = data[0];

    if(isSetSegment)
    {
        if(data.length() != 3) return;
        uint8_t segmentPos = data[1];
        uint8_t segmentData = data[2];
        displayPanel.writeSegments(&segmentData, 1, segmentPos, LAYER_OVERWRITE, true);
    }
    else // clear segment
    {
        if(data.length() != 2) return;
        uint8_t segmentPos = data[1];
        uint8_t layer = LAYER_TIME;
        displayPanel.setActiveLayers(&layer, 1, segmentPos);
    }
}

void onSetBrightness(std::string& data)
{
    if(data.length() != 2) return;
    bool autoBrightness = data[0];

    if(autoBrightness)
    {
        uint8_t timeoutMinutes = data[1];
        displayPanel.setAutoBrightness(true);
    }
    else
    {
        uint8_t brightness = data[1];
        displayPanel.setAutoBrightness(false);
        displayPanel.setBrightnessAll(brightness);
    }
}

void onSetTime(std::string& data)
{
    if(data.length() != 8) return;

    uint8_t slot = data[0];
    if(slot > (displayPanel.getRows())) return;

    uint8_t newDay = data[1];
    uint8_t newMonth = data[2];
    uint8_t yearBytes[2];
            yearBytes[0] = data[3];
            yearBytes[1] = data[4];
    uint8_t newHour = data[5];
    uint8_t newMinute = data[6];
    uint8_t newSecond = data[7];

    uint16_t newYear;
    memcpy(&newYear, &yearBytes, sizeof newYear);
    newYear = ntohs(newYear);
    if(newYear > 9999)
        newYear = 9999;

    if(slot == 0)
    {
        // set RTC
        // RTC can only save year 2000-2099
        if(newYear < 2000)
            newYear = 2000;
        else if(newYear > 2099)
            newYear = 2099;
        DateTime time = DateTime(newYear, newMonth, newDay, newHour, newMinute, newSecond);
        rtc.adjust(time);
        presentTime.setTime(newYear, newMonth, newDay, newHour, newMinute, newSecond);
    }
    else if(slot == 2)
    {
        //System time
        presentTime.setTime(newYear, newMonth, newDay, newHour, newMinute, newSecond);
    }
    else if(slot == 1 || slot == 3)
    {
        //slot 0 is for rtc. Thus row 0 = 1, row 2 = 3
        if(slot == 3)
        {
            destinationTime.Halfsecond = 0;
            destinationTime.Second = 0;
            destinationTime.Minute = newMinute;
            destinationTime.Hour = newHour;
            destinationTime.Day = newDay;
            destinationTime.Month = newMonth;
            destinationTime.Year = newYear;
        }
        slot -= 1;
        displayPanel.setRow(slot);
        displayPanel.setDay(newDay);
        displayPanel.setMonth(newMonth);
        displayPanel.setYear(newYear);
        displayPanel.setHourAndMinute(newHour, newMinute);
    }
}

void onTravel(std::string& data)
{
    Serial.println("TRAVEL!!!!!!!!");
}

void onSetVolume(std::string& data)
{
    Serial.println("onSetVolume");
    Serial.print(data.length());
    Serial.print(" ");
    Serial.println(data.c_str());
}

void onSetPlayback(std::string& data)
{
    Serial.println("onSetPlayback");
    Serial.print(data.length());
    Serial.print(" ");
    Serial.println(data.c_str());
}

void updateTimeTask( void * parameter )
{
  DisplayPanel *displayPanel = (DisplayPanel *) parameter;

  while (true)
  {
    TimeElements_t now = presentTime.now();

    if(presentTime.isSyncTimeDue())
    {
        if(now.Year >= 2000 && now.Year <= 2099 && rtc.begin())
        {
            DateTime rtcNow = rtc.now();
            //If system time is not +/- 1 minute of RTC, do not sync (time could be intentionally set differently).
            if( rtcNow.year() == now.Year &&
                rtcNow.month() == now.Month &&
                rtcNow.day() == now.Day &&
                rtcNow.hour() == now.Hour &&
                (rtcNow.minute() - now.Minute <= 1 && rtcNow.minute() - now.Minute >= -1)
            )
            {
                presentTime.setTime(rtcNow.year(), rtcNow.month(), rtcNow.day(), rtcNow.hour(), rtcNow.minute(), rtcNow.second());
                presentTime.markSynced();
            }
        }
    }

    displayPanel->setRow(0);
    displayPanel->setColon(now.Halfsecond % 2);

    displayPanel->setRow(1);
    displayPanel->setDay(now.Day);
    displayPanel->setMonth(now.Month);
    displayPanel->setYear(now.Year);
    displayPanel->setHourAndMinute(now.Hour, now.Minute);
    displayPanel->setColon(now.Halfsecond % 2);

    displayPanel->setRow(2);
    displayPanel->setColon(now.Halfsecond % 2);

    displayPanel->flush();
    delay(50);
  }
}

void setup()
{
    WiFi.mode(WIFI_OFF);
    Serial.begin(115200);

    displayPanel.begin();
    displayPanel.flush();

    // setup i2s
    auto config = i2s.defaultConfig(TX_MODE);
    config.sample_rate = 44100;
    config.channels = 2;
    config.bits_per_sample = 16;
    config.pin_bck = 17;
    config.pin_data = 16;
    config.pin_ws = 4;

    i2s.begin(config);

    // set initial volume
    out.begin(config); // we need to provide the bits_per_sample and channels
    out.setVolume(0.2);

    if(!SPIFFS.begin(true)) // true = format SPIFFS on failure
    {
        displayPanel.setRow(1);
        displayPanel.showError(B2TF_ERR_SPIFFS_INIT);
        displayPanel.flush();
        delay(3000);
    }

    playSound("/time_circuits_on.mp3");

    if(rtc.begin()) 
    {
        DateTime now = rtc.now();
        presentTime.setTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
        presentTime.markSynced();
    }
    else
    {
        displayPanel.setRow(1);
        displayPanel.showError(B2TF_ERR_RTC_CONNECTION);
        displayPanel.flush();
        delay(3000);
    }

    commandInterface.setCallbackOnSetSegment(&onSetSegment);
    commandInterface.setCallbackOnSetBrightness(&onSetBrightness);
    commandInterface.setCallbackOnSetTime(&onSetTime);
    commandInterface.setCallbackOnTravel(&onTravel);
    commandInterface.setCallbackOnSetVolume(&onSetVolume);
    commandInterface.setCallbackOnSetPlayback(&onSetPlayback);
    commandInterface.begin();

    displayPanel.clear();
    displayPanel.flush();
    displayPanel.setAutoBrightness(true);

    destinationTime.Halfsecond = 0;
    destinationTime.Second = 0;
    destinationTime.Minute = 21;
    destinationTime.Hour = 1;
    destinationTime.Day = 26;
    destinationTime.Month = 10;
    destinationTime.Year = 1985;

    // Set top row to "26.10. 1985 AM 01:21"
    displayPanel.setRow(2);
    displayPanel.setDay(destinationTime.Day);
    displayPanel.setMonth(destinationTime.Month);
    displayPanel.setYear(destinationTime.Year);
    displayPanel.setHourAndMinute(destinationTime.Hour, destinationTime.Minute);

    // Set bottom row to "12.11. 1955 AM 06:38"
    displayPanel.setRow(0);
    displayPanel.setDay(12);
    displayPanel.setMonth(11);
    displayPanel.setYear(1955);
    displayPanel.setHourAndMinute(6, 38);
    displayPanel.flush();

    xTaskCreate(
        updateTimeTask,     /* Task function. */
        "updateTimeTask",   /* String with name of task. */
        2000,              /* Stack size in bytes. */
        &displayPanel,      /* Parameter passed as input of the task */
        1,                  /* Priority of the task. */
        &updateTimeTaskHandle     /* Task handle. */
    );

    playSound("/time_circuits_off.mp3");
    delay(1000);

    xTaskCreate(
        beepTask,       /* Task function. */
        "beepTask",     /* String with name of task. */
        4000,           /* Stack size in bytes. */
        NULL,           /* Parameter passed as input of the task */
        1000,           /* Priority of the task. */
        NULL            /* Task handle. */
    );
}

void loop() 
{
    // const uint8_t minus = 0x00 | SEG_G;
    // for(int i=18; i<=35; i++)
    // {
    //     displayPanel.overwriteSegments(&minus, 1, i);
    //     displayPanel.flush();
    //     delay(500);
    //     displayPanel.resetSegments(1, i);
    //     displayPanel.flush();
    // }
}