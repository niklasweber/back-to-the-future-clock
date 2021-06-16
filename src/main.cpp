#include <Arduino.h>
#include "TimeKeeper.h"

TimeKeeper timeKeeper;

void setup()
{
    Serial.begin(9600);
    Serial.println("Start");

    // debugging LED
    pinMode(17, OUTPUT);
    
    timeKeeper.begin();
}

char timeStr1[40];
char timeStr2[40];

void loop() 
{
    if(timeKeeper.pollTimeUpdate())
        Serial.println("Time updated!");

    DateTime hwTime = timeKeeper.getHWTime();

    if(timeKeeper.hasHWClock())
        sprintf(timeStr1, "HW: %02d.%02d.%04d %02d:%02d:%02d ",
            hwTime.day(),
            hwTime.month(),
            hwTime.year(),
            hwTime.hour(),
            hwTime.minute(),
            hwTime.second());
    else
        sprintf(timeStr1, "No HW Clock!");

    sprintf(timeStr2, "SW: %02d.%02d.%04d %02d:%02d:%02d ", 
        day(),
        month(),
        year(),
        hour(),
        minute(),
        second());

    Serial.print(timeStr1);
    Serial.println(timeStr2);

    delay(1000);
}