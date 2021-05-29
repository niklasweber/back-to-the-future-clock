#include <Arduino.h>
#include "TimeKeeper.h"

TimeKeeper timeKeeper;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup()
{
    Serial.begin(115200);
    
    int rc_begin = timeKeeper.begin();
    switch (rc_begin)
    {
    case E_SUCCESS:
        Serial.println("Initialized time keeper - OK");
        break;
    case E_RTC_NOT_FOUND:
        Serial.println("ERROR: Could not find RTC!");
        // TODO: put in sleep mode
        // "end" program.
        while(true) {}
        break;
    case E_GPS_HW_NOT_FOUND:
        Serial.println("ERROR: Could not find GPS module!");
        // TODO: put in sleep mode
        // "end" program.
        while(true) {}
        break;
    default:
        break;
    }
}

void loop() 
{
    DateTime now = timeKeeper.getHWTime();
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();

    delay(1000);
}