#include <Arduino.h>
#include "TimeKeeper.h"

TimeKeeper timeKeeper;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup()
{
    Serial.begin(9600);
    
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
    default:
        break;
    }
}

void loop() 
{
}