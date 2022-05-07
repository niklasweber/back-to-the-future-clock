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

#define BOOT_MIN_TIME 2000

RTC_DS3231 rtc;
PresentTime presentTime;
DisplayPanel displayPanel;
CommandInterface commandInterface;

TaskHandle_t updateTimeTaskHandle;

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
        displayPanel.overwriteSegments(&segmentData, 1, segmentPos);
    }
    else // clear segment
    {
        if(data.length() != 2) return;
        uint8_t segmentPos = data[1];
        displayPanel.resetSegments(1, segmentPos);
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
        slot -= 1;
        displayPanel.setRow(slot);
        displayPanel.setDay(newDay);
        displayPanel.setMonth(newMonth);
        displayPanel.setYear(newYear);
        displayPanel.setHourAndMinute(newHour, newMinute);
    }
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

void soundTask( void * parameter )
{
    I2SStream i2s; // final output of decoded stream
    VolumeStream out(i2s); // stream to control volume
    EncodedAudioStream decoder(&out, new MP3DecoderHelix()); // Decoding stream
    StreamCopy copier; // copies sound into i2s

    File audioFile = SPIFFS.open("/time_circuits_on.mp3");
    if(!audioFile || audioFile.isDirectory()){
        Serial.println("Failed to open file for reading");
        vTaskDelete( NULL );
    }
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
    out.setVolume(0.4);

    // setup I2S based on sampling rate provided by decoder
    decoder.setNotifyAudioChange(i2s);
    decoder.begin();

    // begin copy
    copier.begin(decoder, audioFile);

    while (true)
    {
        if (!copier.copy()) {
            audioFile.close();
            break;
        }
    }
    delay(3000);
    vTaskDelete( NULL );
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

    displayPanel->write();
    delay(50);
  }
}

void setup()
{
    WiFi.mode(WIFI_OFF);
    Serial.begin(115200);

    displayPanel.begin();
    displayPanel.write();

    if(rtc.begin()) 
    {
        DateTime now = rtc.now();
        presentTime.setTime(now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
        presentTime.markSynced();
    }
    else
    {
        displayPanel.setRow(1);
        displayPanel.showRTCError();
        displayPanel.write();
        delay(3000);
    }

    commandInterface.setCallbackOnSetSegment(&onSetSegment);
    commandInterface.setCallbackOnSetBrightness(&onSetBrightness);
    commandInterface.setCallbackOnSetTime(&onSetTime);
    commandInterface.setCallbackOnSetVolume(&onSetVolume);
    commandInterface.setCallbackOnSetPlayback(&onSetPlayback);
    commandInterface.begin();

    if(unsigned long m = millis() < BOOT_MIN_TIME) delay(BOOT_MIN_TIME - m);

    if(SPIFFS.begin(false)) // false = don't format SPIFFS on fail
    {
        xTaskCreate(
            soundTask,      /* Task function. */
            "SoundTask",    /* String with name of task. */
            10000,          /* Stack size in bytes. */
            NULL,           /* Parameter passed as input of the task */
            1,              /* Priority of the task. */
            NULL            /* Task handle. */
        );
    }
    else
    {
        displayPanel.setRow(1);
        displayPanel.showSoundError(10);
        displayPanel.write();
        delay(3000);
    }

    displayPanel.clear();
    displayPanel.write();
    displayPanel.setAutoBrightness(true);

    // Set top row to "26.10. 1985 AM 01:21"
    displayPanel.setRow(2);
    displayPanel.setDay(26);
    displayPanel.setMonth(10);
    displayPanel.setYear(1985);
    displayPanel.setHourAndMinute(1, 21);

    // Set bottom row to "12.11. 1955 AM 06:38"
    displayPanel.setRow(0);
    displayPanel.setDay(12);
    displayPanel.setMonth(11);
    displayPanel.setYear(1955);
    displayPanel.setHourAndMinute(6, 38);
    displayPanel.write();

    xTaskCreate(
        updateTimeTask,     /* Task function. */
        "updateTimeTask",   /* String with name of task. */
        10000,              /* Stack size in bytes. */
        &displayPanel,      /* Parameter passed as input of the task */
        1,                  /* Priority of the task. */
        &updateTimeTaskHandle     /* Task handle. */
    );
}

void loop() 
{
    // const uint8_t minus = 0x00 | SEG_G;
    // for(int i=18; i<=35; i++)
    // {
    //     displayPanel.overwriteSegments(&minus, 1, i);
    //     displayPanel.write();
    //     delay(500);
    //     displayPanel.resetSegments(1, i);
    //     displayPanel.write();
    // }
}