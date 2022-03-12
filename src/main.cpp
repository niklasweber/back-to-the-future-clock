#include <Arduino.h>
#include <RTClib.h>
#include <TimeLib.h>
#include "DisplayPanel.h"
// #include "CommandInterface.h"

#include <BH1750.h>
// #include <Wire.h>

// set this in AudioConfig.h or here after installing https://github.com/pschatzmann/arduino-libhelix.git
#define USE_HELIX
#include "SPIFFS.h"
#include "AudioTools.h"
#include "AudioCodecs/CodecMP3Helix.h"

using namespace audio_tools;

#define BOOT_MIN_TIME 2000

BH1750 lightMeter;
RTC_DS3231 rtc;
DisplayPanel displayPanel;
// CommandInterface commandInterface;

typedef int16_t sound_t;                        // sound will be represented as int16_t (with 2 bytes)
uint16_t sample_rate=44100;
uint8_t channels = 2;                           // The stream will have 2 channels

I2SStream i2s;                                  // final output of decoded stream
EncodedAudioStream decoder(&i2s, new MP3DecoderHelix()); // Decoding stream
StreamCopy copier;                  // copies sound into i2s
File audioFile;

bool showTime = true;
uint8_t timeRow = 1;
uint8_t messageRow = 1;

// void onSetSegments(cmd_set_segments& cmd)
// {
//     if(cmd.startPos + cmd.length > displayPanel.getSegmentsMax())
//         return;

//     displayPanel.setSegments(cmd.segments, cmd.length, cmd.startPos);
// }

// void onShowTime(cmd_show_time& cmd)
// {
//     // Serial.println("onShowTime");

//     if(!cmd.on)
//         showTime = false;
//     else
//     {
//         showTime = true;
//         if(cmd.row >= displayPanel.getRows())
//             cmd.row = displayPanel.getRows()-1;
//     }
// }

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

void setup()
{
    WiFi.mode(WIFI_OFF);
    //btStop();
    Serial.begin(115200);

    displayPanel.begin();

    // Wire.begin();

    if(rtc.begin()) 
    {
        // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // rtc.adjust(DateTime(2022, 1, 6, 23, 59, 50));
        setTime(rtc.now().unixtime());
    }
    else
    {
        displayPanel.setRow(messageRow);
        displayPanel.showRTCError();
        delay(2000);
    }

    lightMeter.begin();

//     if(!commandInterface.begin(&onSetSegments, &onShowTime))
//     {
//         displayPanel.setRow(1);
//         displayPanel.showCommandInterfaceError();
//         delay(2000);

//         while (1) {} // hold in infinite loop
//         // TODO: Keep system alive, but handle with care. Check if radio has been initialized everywhere.
//     }

//     Serial.begin(115200);
//     int rc = initSoundModule();
//     if(rc != 0)
//     {
//         displayPanel.setRow(messageRow);
//         displayPanel.showSoundError(rc);
        // delay(3000);
//     }

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
        displayPanel.setRow(messageRow);
        displayPanel.showSoundError(10);
        delay(3000);
    }

    displayPanel.clear();

    // Set top row to "26.10. 1985 AM 01:21"
    displayPanel.setRow(2);
    displayPanel.setDay(26);
    displayPanel.setMonth(10);
    displayPanel.setYear(1985);
    displayPanel.setHourAndMinute(1, 21);

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
}

void loop() 
{
//     commandInterface.handleInput();

    // Set middle row to current system time
    if(showTime)
    {
        displayPanel.setRow(timeRow);
        displayPanel.setDay(day());
        displayPanel.setMonth(month());
        displayPanel.setYear(year());
        displayPanel.setHourAndMinute(hour(), minute());
    }

    float lux = lightMeter.readLightLevel();
    // Serial.print("Light: ");
    // Serial.print(lux);
    // Serial.println(" lx");

    if(lux > 5)
    {
        displayPanel.setBrightnessAll(7);
    }
    else
    {
        displayPanel.setBrightnessAll(1);
    }

    delay(1000);
}