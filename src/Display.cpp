#include "Display.h"
#include <Arduino.h>

void Display::flush()
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

    // If segments didn't change, but brightness did, at least one segment needs to be written
    // in order for brightness to change on hardware.
    if(!hasChange && (brightness != targetBrightness))
    {
        hasChange = true; // Segments didn't change, but brightness did
        changeMinIndex = 0;
        changeMaxIndex = 0;
    }

    if (hasChange)
    {
        // Set brightness first
        if(this->targetBrightness > 0)
        {
            uint8_t mappedBrightness = map(this->targetBrightness, 1, 100, 0, 7);
            hw.setBrightness(mappedBrightness, true);
        }
        else
        {
            hw.setBrightness(0, false);
        }
        this->brightness = this->targetBrightness;

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

void Display::read(uint8_t * segments, uint8_t length, uint8_t pos, uint8_t layer)
{
    // Boundary checks
    if(pos > segmentsMax-1)
        pos = segmentsMax-1;
    if((pos + length) > segmentsMax)
        length = segmentsMax - pos;
    if(layer > layersMax-1)
        layer = layersMax-1;

    memcpy(segments, this->segments[layer]+pos, length);
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
void Display::setSegments(const uint8_t segments[], uint8_t length, uint8_t pos, uint8_t layer, bool updateActiveLayer)
{
    // Boundary checks
    if(pos > segmentsMax-1)
        pos = segmentsMax-1;
    if((pos + length) > segmentsMax)
        length = segmentsMax - pos;
    if(layer > layersMax-1)
        layer = layersMax-1;

    memcpy(this->segments[layer]+pos, segments, length);

    if(updateActiveLayer)
    {
        for(int s=pos; s<length; s++)
        {
            activeLayers[s] = layer;
        }
    }
}

uint8_t Display::getSegment(uint8_t pos, uint8_t layer)
{
    // Boundary checks
    if(pos > segmentsMax-1)
        pos = segmentsMax-1;
    if(layer > layersMax-1)
        layer = layersMax-1;

    return segments[layer][pos];
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
    if(brightness > 100)
        brightness = 100;

    this->targetBrightness = brightness;
}

uint8_t Display::getSegmentsMax()
{
    return segmentsMax;
}

uint8_t Display::getLayersMax()
{
    return layersMax;
}