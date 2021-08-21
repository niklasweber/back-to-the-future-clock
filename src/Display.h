#ifndef DISPLAY_H
#define DISPLAY_H

#include <TM1637Display.h>

class Display
{
public:
    Display(uint8_t pinClk, uint8_t pinDIO, unsigned int bitDelay = 100U):
        hw(pinClk, pinDIO, bitDelay), segments{}, brightness(0), powerOn(false) {};
    void setSegments(const uint8_t segments[], uint8_t length = 4, uint8_t pos = 0);
    // uint8_t getSegments(uint8_t column);
    void setDigits(uint8_t digits[], uint8_t length = 4, uint8_t pos = 0);
    uint8_t encodeDigit(uint8_t digit);
    void setBrightness(uint8_t brightness, bool on = true);
    uint8_t getSegmentsMax();
private:
    TM1637Display hw;
    uint8_t segments[6];
    uint8_t segmentsMax = 6;
    uint8_t brightness;
    bool powerOn;
};

#endif // DISPLAY_H