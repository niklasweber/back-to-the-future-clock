#ifndef DISPLAY_H
#define DISPLAY_H

#include <TM1637Display.h>

#define CLK 2
#define DIO_DISPLAY1_ROW_BOTTOM 39
#define DIO_DISPLAY2_ROW_BOTTOM 40
#define DIO_DISPLAY3_ROW_BOTTOM 41
#define DIO_DISPLAY1_ROW_MIDDLE 42
#define DIO_DISPLAY2_ROW_MIDDLE 43
#define DIO_DISPLAY3_ROW_MIDDLE 3
#define DIO_DISPLAY1_ROW_TOP 45
#define DIO_DISPLAY2_ROW_TOP 46
#define DIO_DISPLAY3_ROW_TOP 47

class Display 
{
public:
    void begin();
    void setRow(unsigned char row);
    void setDay(unsigned int day);
    void setYear(unsigned int year);
    void setHour(const unsigned int hour);
    void setMinute(const unsigned int minute);
    void setAM(const bool on);
    void setPM(const bool on);
private:
    unsigned char row = 0;
    TM1637Display display1rowBottom = TM1637Display(CLK, DIO_DISPLAY1_ROW_BOTTOM);
    TM1637Display display2rowBottom = TM1637Display(CLK, DIO_DISPLAY2_ROW_BOTTOM);
    TM1637Display display3rowBottom = TM1637Display(CLK, DIO_DISPLAY3_ROW_BOTTOM);
    TM1637Display display1rowMiddle = TM1637Display(CLK, DIO_DISPLAY1_ROW_MIDDLE);
    TM1637Display display2rowMiddle = TM1637Display(CLK, DIO_DISPLAY2_ROW_MIDDLE);
    TM1637Display display3rowMiddle = TM1637Display(CLK, DIO_DISPLAY3_ROW_MIDDLE);
    TM1637Display display1rowTop    = TM1637Display(CLK, DIO_DISPLAY1_ROW_TOP);
    TM1637Display display2rowTop    = TM1637Display(CLK, DIO_DISPLAY2_ROW_TOP);
    TM1637Display display3rowTop    = TM1637Display(CLK, DIO_DISPLAY3_ROW_TOP);
    TM1637Display *displays[3][3] = {   {&display1rowBottom, &display2rowBottom, &display3rowBottom},
                                        {&display1rowMiddle, &display2rowMiddle, &display3rowMiddle},
                                        {&display1rowTop,    &display2rowTop,    &display3rowTop}};
};

#endif //DISPLAY_H