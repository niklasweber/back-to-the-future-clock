#ifndef DISPLAY_H
#define DISPLAY_H

#include <TM1637Display.h>

enum SegmentLayer
{
    LAYER_TIME = 0,
    LAYER_OVERWRITE = 1
};

class Display
{
public:
    Display(uint8_t pinClk, uint8_t pinDIO, unsigned int bitDelay = 100U):
        hw(pinClk, pinDIO, bitDelay), segments{}, brightness(0) {};
    void write();
    void setActiveLayer(uint8_t segment, uint8_t layer);
    void setSegments(const uint8_t segments[], uint8_t length = 4, uint8_t pos = 0, uint8_t layer = 0);
    // uint8_t getSegments(uint8_t column);
    void setDigits(uint8_t digits[], uint8_t length = 4, uint8_t pos = 0, uint8_t layer = 0);
    uint8_t encodeDigit(uint8_t digit);
    void setBrightness(uint8_t brightness);
    uint8_t getSegmentsMax();
private:
    TM1637Display hw;

    // Select visible layer for each segment
    uint8_t activeLayers[6] = { 0 };
    // First dimension: layer. second dimension: segments.
    uint8_t segments[2][6];
    uint8_t segmentsMax = 6;
    // Save active segments to be able to compare and check if segments changed.
    uint8_t activeSegments[6] = { 0 };

    uint8_t layersMax = 2;
    uint8_t brightness;
};

#endif // DISPLAY_H