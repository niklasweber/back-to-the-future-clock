#ifndef COMMANDINTERFACE_H
#define COMMANDINTERFACE_H

#include "RF24.h"

#define RADIO_CE_PIN 9
#define RADIO_CSN_PIN 10

enum ERADIO_CMD
{
    CMD_SET_SEGMENTS,
    CMD_PLAY_SOUND,
    CMD_SHOW_TIME
};

struct cmd_set_segments
{
    uint8_t preamble[4];
    uint8_t cmd;
    uint8_t length; // Max 25 bytes (due to max payload size).
    uint8_t startPos;
    uint8_t segments[25];
};

struct cmd_show_time
{
    uint8_t preamble[4];
    uint8_t cmd;
    uint8_t on;
    uint8_t row;
    uint8_t unused[24];
};

struct message_t
{
    float data;
};

class CommandInterface
{
public:
    CommandInterface() : radio(RADIO_CE_PIN, RADIO_CSN_PIN) {};
    uint8_t begin(void (*onSetSegmentsPtr)(cmd_set_segments&),
                  void (*onShowTimePtr)(cmd_show_time&));
    void handleInput();
private:
    RF24 radio;
    void (*onSetSegmentsPtr)(cmd_set_segments&);
    void (*onShowTimePtr)(cmd_show_time&);
};

#endif //COMMANDINTERFACE_H