#ifndef COMMANDINTERFACE_H
#define COMMANDINTERFACE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define DISPLAY_SERVICE_UUID            "6a58cb2a-f8a2-4eea-b8d4-1c50c0f60d25"
#define SEGMENT_CHARACTERISTIC_UUID     "f1c1b1ba-e230-4472-b6da-809f65e3233e"
#define BRIGHTNESS_CHARACTERISTIC_UUID  "237def27-170e-4383-8600-eddc4fc26725"

#define TIME_SERVICE_UUID               "12a1c6e7-23f0-4117-911f-6b42872a1497"
#define TIME_CHARACTERISTIC_UUID        "d2920f73-f49f-42f8-85b2-8111a0daff28"
#define TRAVEL_CHARACTERISTIC_UUID      "131666ca-4cc7-42c9-ac4f-833a3466e24e"

#define SOUND_SERVICE_UUID              "ee7119dc-e8e1-40c6-a8bc-197a4db3b211"
#define VOLUME_CHARACTERISTIC_UUID      "b51427c7-e803-43e2-b393-e1e808c519bb"
#define PLAYBACK_CHARACTERISTIC_UUID    "9c802e03-14c7-481a-9105-a5dbe3c774a0"

// #include "RF24.h"

// #define RADIO_CE_PIN 9
// #define RADIO_CSN_PIN 10

// enum ERADIO_CMD
// {
//     CMD_SET_SEGMENTS,
//     CMD_CONTROL_SOUND,
//     CMD_SHOW_TIME
// };

// struct cmd_set_segments
// {
//     uint8_t preamble[4];
//     uint8_t cmd;
//     uint8_t length; // Max 25 bytes (due to max payload size).
//     uint8_t startPos;
//     uint8_t segments[25];
// };

// struct cmd_show_time
// {
//     uint8_t preamble[4];
//     uint8_t cmd;
//     uint8_t on;
//     uint8_t row;
//     uint8_t unused[24];
// };

// cmd_row_settings
// row
// mode: show timestamp / manually set segments
// timestamp || day month year hour minute
// brightness
// time to turn off

// struct cmd_control_sound
// {
//     uint8_t preamble[4];
//     uint8_t cmd;
//     uint8_t pause;  // if > 0: pause.
//     uint8_t resume; // if > 0: resume
//     uint8_t play;   // if > 0: play.
//     uint8_t track;
//     uint8_t position;
//     uint8_t changeVolume; // if > 0: change volume. Otherwise keep set volume.
//     uint8_t volume;
//     uint8_t unused[20];
// };

// struct message_t
// {
//     float data;
// };

class CommandInterface: public BLECharacteristicCallbacks, BLEServerCallbacks
{
public:
//     CommandInterface() : radio(RADIO_CE_PIN, RADIO_CSN_PIN) {};
//     uint8_t begin(void (*onSetSegmentsPtr)(cmd_set_segments&);
    uint8_t begin();
//     void handleInput();
    void setCallbackOnSetSegment(void (*onSetSegmentPtr)(std::string&))
    {
        this->onSetSegmentPtr = onSetSegmentPtr;
    }
    void setCallbackOnSetBrightness(void (*onSetBrightnessPtr)(std::string&))
    {
        this->onSetBrightnessPtr = onSetBrightnessPtr;
    }
    void setCallbackOnSetTime(void (*onSetTimePtr)(std::string&))
    {
        this->onSetTimePtr = onSetTimePtr;
    }
    void setCallbackOnTravel(void (*onTravelPtr)(std::string&))
    {
        this->onTravelPtr = onTravelPtr;
    }
    void setCallbackOnSetVolume(void (*onSetVolumePtr)(std::string&))
    {
        this->onSetVolumePtr = onSetVolumePtr;
    }
    void setCallbackOnSetPlayback(void (*onSetPlaybackPtr)(std::string&))
    {
        this->onSetPlaybackPtr = onSetPlaybackPtr;
    }
private:
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
    void onWrite(BLECharacteristic* pCharacteristic);
    //virtual void onRead(BLECharacteristic* pCharacteristic);
	//virtual void onNotify(BLECharacteristic* pCharacteristic);
	//virtual void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code);

    void (*onSetSegmentPtr)(std::string&);
    void (*onSetBrightnessPtr)(std::string&);
    void (*onSetTimePtr)(std::string&);
    void (*onTravelPtr)(std::string&);
    void (*onSetVolumePtr)(std::string&);
    void (*onSetPlaybackPtr)(std::string&);
};

#endif //COMMANDINTERFACE_H