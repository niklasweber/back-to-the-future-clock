#include <Arduino.h>
#include <RTClib.h>
#include <TimeLib.h>
#include "DisplayPanel.h"
#include "CommandInterface.h"

// set this in AudioConfig.h or here after installing https://github.com/pschatzmann/arduino-libhelix.git
#define USE_HELIX
#include "SPIFFS.h"
#include "AudioTools.h"
#include "AudioCodecs/CodecMP3Helix.h"

using namespace audio_tools;

#define BOOT_MIN_TIME 2000

RTC_DS3231 rtc;
DisplayPanel displayPanel;
CommandInterface commandInterface;

typedef int16_t sound_t;                        // sound will be represented as int16_t (with 2 bytes)
uint16_t sample_rate=44100;
uint8_t channels = 2;                           // The stream will have 2 channels

I2SStream i2s;                                  // final output of decoded stream
EncodedAudioStream decoder(&i2s, new MP3DecoderHelix()); // Decoding stream
StreamCopy copier;                  // copies sound into i2s
File audioFile;

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
    Serial.println("onSetTime");
    Serial.print(data.length());
    Serial.print(" ");
    Serial.println(data.c_str());
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
    audioFile = SPIFFS.open("/time_circuits_on.mp3");
    if(!audioFile || audioFile.isDirectory()){
        Serial.println("Failed to open file for reading");
        vTaskDelete( NULL );
    }
    // setup i2s
    auto config = i2s.defaultConfig(TX_MODE);
    config.sample_rate = sample_rate; 
    config.channels = channels;
    config.bits_per_sample = 16;
    config.pin_bck = 17;
    config.pin_data = 16;
    config.pin_ws = 4;
    i2s.begin(config);

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
    displayPanel->setRow(1);
    displayPanel->setDay(day());
    displayPanel->setMonth(month());
    displayPanel->setYear(year());
    displayPanel->setHourAndMinute(hour(), minute());
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
        // DateTime time = DateTime(F(__DATE__), F(__TIME__)) + 40;
        // DateTime time = DateTime(2022, 1, 6, 23, 59, 50)
        // rtc.adjust(time);
        setTime(rtc.now().unixtime());
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
    // TODO: Load times from SPIFFS

    // Set middle row to current system time
    displayPanel.setRow(1);
    displayPanel.setDay(day());
    displayPanel.setMonth(month());
    displayPanel.setYear(year());
    displayPanel.setHourAndMinute(hour(), minute());

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