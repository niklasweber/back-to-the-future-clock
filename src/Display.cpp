#include "Display.h"
#include <Arduino.h>

void Display::setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
    if(pos > segmentsMax-1)
        pos = segmentsMax-1;

    if((pos + length) > segmentsMax)
        length = segmentsMax - pos;

    memcpy(this->segments+pos, segments, length);

    // Due to limitation of display library, pos cannot be larger than 4. Bypass by starting at 4.
    if(pos > 3)
    {
        length += pos - 3;
        pos = 3;
    }
    hw.setSegments(segments, length, pos);
}

void Display::setDigits(uint8_t digits[], uint8_t length, uint8_t pos)
{
    for(int i=0; i<length; i++)
    {
        digits[i] = hw.encodeDigit(digits[i]);
    }
    setSegments(digits, length, pos);
}

uint8_t Display::encodeDigit(uint8_t digit)
{
    return hw.encodeDigit(digit);
}

void Display::setBrightness(uint8_t brightness)
{
    if(brightness > 100) brightness = 100;
    this->brightness = brightness;

    if(brightness > 0)
    {
        brightness = map(brightness, 1, 100, 0, 7);
        hw.setBrightness(brightness, true);
    }
    else
    {
        hw.setBrightness(0, false);
    }
}

uint8_t Display::getSegmentsMax()
{
    return segmentsMax;
}