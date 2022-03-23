#include "Display.h"
#include <Arduino.h>

void Display::write()
{
    if (hasChange)
    {
        uint8_t changeLength = changeMaxIndex - changeMinIndex + 1;

        // Due to limitation of display library, pos cannot be larger than 4. Bypass by starting at 4.
        if(changeMinIndex > 3)
        {
            changeLength += changeMinIndex - 3;
            changeMinIndex = 3;
        }
        hw.setSegments(segments+changeMinIndex, changeLength, changeMinIndex);

        changeMinIndex = segmentsMax - 1;
        changeMaxIndex = 0;
        hasChange = false;
    }
}

// Only writes to buffer. Use write() to send segments to hardware.
void Display::setSegments(const uint8_t segments[], uint8_t length, uint8_t pos)
{
    // Boundary checks
    if(pos > segmentsMax-1)
        pos = segmentsMax-1;
    if((pos + length) > segmentsMax)
        length = segmentsMax - pos;

    // Go through segments that should be changed
    for(uint8_t i=0; i<length; i++)
    {
        // Only pay attention if segments are different from previously set segments
        if(this->segments[pos+i] != segments[i])
        {
            // Find min and max of changed fields
            if(!hasChange || pos+i < changeMinIndex)
                changeMinIndex = pos+i;
            if(!hasChange || pos+i > changeMaxIndex)
                changeMaxIndex = pos+i;

            hasChange = true;
            this->segments[pos+i] = segments[i];
        }
    }
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