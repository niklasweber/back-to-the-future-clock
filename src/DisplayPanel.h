#ifndef DISPLAYPANEL_H
#define DISPLAYPANEL_H

#include "Display.h"
#include <Wire.h>
#include <BH1750.h>

#define DIO 15 // TD0
#define CLK_DISPLAY1_ROW_BOTTOM 33
#define CLK_DISPLAY2_ROW_BOTTOM 23
#define CLK_DISPLAY3_ROW_BOTTOM 13 // TCK
#define CLK_DISPLAY1_ROW_MIDDLE 27
#define CLK_DISPLAY2_ROW_MIDDLE 25
#define CLK_DISPLAY3_ROW_MIDDLE 32
#define CLK_DISPLAY1_ROW_TOP 26
#define CLK_DISPLAY2_ROW_TOP 18
#define CLK_DISPLAY3_ROW_TOP 19

class DisplayPanel
{
public:
    void begin();
    void clear();
    void setSegments(const uint8_t segments[], uint8_t length, uint8_t pos = 0);
    void setBrightness(unsigned char row, unsigned char column, unsigned char brightness);
    void setBrightnessAll(unsigned char brightness);
    void setAutoBrightness(bool autoBrightness);
    float getLightMeterValue();
    void showRTCError();
    void showCommandInterfaceError();
    void showSoundError(int error_code);
    void setRow(unsigned int row);
    void setMonth(unsigned char month);
    void setDay(unsigned char day);
    void setYear(unsigned int year);
    void setHourAndMinute(unsigned char hour, unsigned char minute);
    uint8_t getSegmentsMax();
    uint8_t getRows();
    uint8_t getColumns();
private:
    unsigned char row = 0;
    Display display1rowBottom = Display(CLK_DISPLAY1_ROW_BOTTOM, DIO);
    Display display2rowBottom = Display(CLK_DISPLAY2_ROW_BOTTOM, DIO);
    Display display3rowBottom = Display(CLK_DISPLAY3_ROW_BOTTOM, DIO);
    Display display1rowMiddle = Display(CLK_DISPLAY1_ROW_MIDDLE, DIO);
    Display display2rowMiddle = Display(CLK_DISPLAY2_ROW_MIDDLE, DIO);
    Display display3rowMiddle = Display(CLK_DISPLAY3_ROW_MIDDLE, DIO);
    Display display1rowTop    = Display(CLK_DISPLAY1_ROW_TOP, DIO);
    Display display2rowTop    = Display(CLK_DISPLAY2_ROW_TOP, DIO);
    Display display3rowTop    = Display(CLK_DISPLAY3_ROW_TOP, DIO);
    Display *displays[3][3] = {   {&display1rowBottom, &display2rowBottom, &display3rowBottom},
                                        {&display1rowMiddle, &display2rowMiddle, &display3rowMiddle},
                                        {&display1rowTop,    &display2rowTop,    &display3rowTop}};
    const unsigned int displayRows = 3;
    const unsigned int displayColumns = 3;
    uint8_t segmentsMax = 54;

    BH1750 lightMeter;
    TaskHandle_t autoBrightnessTask;
};

#endif //DISPLAYPANEL_H