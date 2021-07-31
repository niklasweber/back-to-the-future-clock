#ifndef COMMANDINTERFACE_H
#define COMMANDINTERFACE_H

#include "RF24.h"

#define RADIO_CE_PIN 9
#define RADIO_CSN_PIN 10

class CommandInterface
{
public:
    CommandInterface() : radio(RADIO_CE_PIN, RADIO_CSN_PIN) {};
    uint8_t begin();
    void handleInput();
private:
    RF24 radio;
};

#endif //COMMANDINTERFACE_H