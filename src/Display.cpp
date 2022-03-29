#include "Display.h"
#include <Arduino.h>

void Display::write()
{
    // Get segments from active layers and find changes
    bool hasChange = false;
    uint8_t changeMinIndex;
    uint8_t changeMaxIndex;
    for(int s = 0; s<segmentsMax; s++)
    {
        uint8_t activeSegment = segments[activeLayers[s]][s];
        if(activeSegments[s] != activeSegment)
        {
            // Find min and max of changed fields
            if(!hasChange || s < changeMinIndex)
                changeMinIndex = s;
            if(!hasChange || s > changeMaxIndex)
                changeMaxIndex = s;

            activeSegments[s] = activeSegment;
            hasChange = true;
        }
    }

    if (hasChange)
    {
        uint8_t changeLength = changeMaxIndex - changeMinIndex + 1;

        // Due to limitation of display library, pos cannot be larger than 4. Bypass by starting at 4.
        if(changeMinIndex > 3)
        {
            changeLength += changeMinIndex - 3;
            changeMinIndex = 3;
        }

        hw.setSegments(activeSegments+changeMinIndex, changeLength, changeMinIndex);
    }
}

void Display::setActiveLayer(uint8_t segment, uint8_t layer)
{
    // Boundary checks
    if(segment > segmentsMax-1)
        segment = segmentsMax-1;
    if(layer > layersMax-1)
        layer = layersMax-1;

    activeLayers[segment] = layer;
}

// Only writes to buffer. Use write() to send segments to hardware.
void Display::setSegments(const uint8_t segments[], uint8_t length, uint8_t pos, uint8_t layer)
{
    // Boundary checks
    if(pos > segmentsMax-1)
        pos = segmentsMax-1;
    if((pos + length) > segmentsMax)
        length = segmentsMax - pos;
    if(layer > layersMax-1)
        layer = layersMax-1;

    memcpy(this->segments[layer]+pos, segments, length);
}

void Display::setDigits(uint8_t digits[], uint8_t length, uint8_t pos, uint8_t layer)
{
    for(int i=0; i<length; i++)
    {
        digits[i] = hw.encodeDigit(digits[i]);
    }
    setSegments(digits, length, pos, layer);
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