#ifndef DISPLAYPANEL_H
#define DISPLAYPANEL_H

#include "Display.h"

#define DIO 14 // A0
#define CLK_DISPLAY1_ROW_BOTTOM 3
#define CLK_DISPLAY2_ROW_BOTTOM 4
#define CLK_DISPLAY3_ROW_BOTTOM 5
#define CLK_DISPLAY1_ROW_MIDDLE 6
#define CLK_DISPLAY2_ROW_MIDDLE 7
#define CLK_DISPLAY3_ROW_MIDDLE 8
#define CLK_DISPLAY1_ROW_TOP 15 // A1
#define CLK_DISPLAY2_ROW_TOP 16 // A2
#define CLK_DISPLAY3_ROW_TOP 17 // A3

class DisplayPanel
{
public:
    void begin();
    void clear();
    void setBrightness(unsigned char row, unsigned char column, unsigned char brightness, bool on = true);
    void showRTCError();
    void showCommandInterfaceError();
    void setRow(unsigned int row);
    void setMonth(unsigned char month);
    void setDay(unsigned char day);
    void setYear(unsigned int year);
    void setHourAndMinute(unsigned char hour, unsigned char minute);

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
};

#endif //DISPLAYPANEL_H