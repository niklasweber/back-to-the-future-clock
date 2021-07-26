#include <Arduino.h>
#include "TimeKeeper.h"
#include "Display.h"

#define BOOT_MIN_TIME 2000

TimeKeeper timeKeeper;
Display display;

// Make LED blink on DCF77 signal
void dcf77IntHandler() {
    digitalWrite(LED_BUILTIN, digitalRead(DCF77_PIN));
}

void setup()
{
    display.begin();

    Serial.begin(9600);
    Serial.println("Back to the future clock - Start");
    
    timeKeeper.begin();

    // DCF77 signal LED (for debugging / positioning)
    pinMode(LED_BUILTIN, OUTPUT);
    attachInterrupt(DCF77_INTERRUPT, dcf77IntHandler, CHANGE);

    if(!timeKeeper.hasHWClock())
    {
        display.setRow(2);
        display.showRTCError();
    }

    if(unsigned long m = millis() < BOOT_MIN_TIME) delay(BOOT_MIN_TIME - m);
    display.clear();
    delay(500);

    // Set top row to "26.10. 1985 AM 01:21"
    display.setRow(2);
    display.setDay(26);
    display.setMonth(10);
    display.setYear(1985);
    display.setHourAndMinute(1, 21);

    // Set middle row to current system time
    display.setRow(1);
    display.setDay(day());
    display.setMonth(month());
    display.setYear(year());
    display.setHourAndMinute(hour(), minute());

    // Set bottom row to "12.11. 1955 AM 06:38"
    display.setRow(0);
    display.setDay(12);
    display.setMonth(11);
    display.setYear(1955);
    display.setHourAndMinute(6, 38);
}

char timeStr1[40];
char timeStr2[40];

void loop() 
{
    if(timeKeeper.pollTimeUpdate())
    {
        Serial.println("Time updated!");

        // On bottom row, show when time was last updated
        display.setRow(0);
        display.setDay(day());
        display.setMonth(month());
        display.setYear(year());
        display.setHourAndMinute(hour(), minute());
    }

    if(timeKeeper.hasHWClock())
    {
        DateTime hwTime = timeKeeper.getHWTime();
        sprintf(timeStr1, "HW: %02d.%02d.%04d %02d:%02d:%02d ",
            hwTime.day(),
            hwTime.month(),
            hwTime.year(),
            hwTime.hour(),
            hwTime.minute(),
            hwTime.second());
    }
    else
        sprintf(timeStr1, "No HW Clock! ");

    sprintf(timeStr2, "SW: %02d.%02d.%04d %02d:%02d:%02d ", 
        day(),
        month(),
        year(),
        hour(),
        minute(),
        second());

    // Set middle row to current system time
    display.setRow(1);
    display.setDay(day());
    display.setMonth(month());
    display.setYear(year());
    display.setHourAndMinute(hour(), minute());

    Serial.print(timeStr1);
    Serial.println(timeStr2);

    delay(500);
}