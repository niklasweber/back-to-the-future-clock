#include <Arduino.h>
#include "TimeKeeper.h"
#include "DisplayPanel.h"
#include "CommandInterface.h"


#define BOOT_MIN_TIME 2000

TimeKeeper timeKeeper;
DisplayPanel displayPanel;
CommandInterface commandInterface;

// Make LED blink on DCF77 signal
// void dcf77IntHandler() {
//     digitalWrite(LED_BUILTIN, digitalRead(DCF77_PIN));
// }

void setup()
{
    displayPanel.begin();

    Serial.begin(9600);
    Serial.println("Back to the future clock - Start");
    
    timeKeeper.begin();

    // TODO: Make DCF77 signal visible on Display (in debugging mode)
    // // DCF77 signal LED (for debugging / positioning)
    // pinMode(LED_BUILTIN, OUTPUT);
    // attachInterrupt(DCF77_INTERRUPT, dcf77IntHandler, CHANGE);

    if(!timeKeeper.hasHWClock())
    {
        displayPanel.setRow(1);
        displayPanel.showRTCError();
        delay(2000);
    }

    if(!commandInterface.begin())
    {
        displayPanel.setRow(1);
        displayPanel.showCommandInterfaceError();
        delay(2000);

        while (1) {} // hold in infinite loop
        // TODO: Keep system alive, but handle with care. Check if radio has been initialized everywhere.
    }

    if(unsigned long m = millis() < BOOT_MIN_TIME) delay(BOOT_MIN_TIME - m);
    displayPanel.clear();
    delay(500);

    // Set top row to "26.10. 1985 AM 01:21"
    displayPanel.setRow(2);
    displayPanel.setDay(26);
    displayPanel.setMonth(10);
    displayPanel.setYear(1985);
    displayPanel.setHourAndMinute(1, 21);

    // Set middle row to current system time
    displayPanel.setRow(1);
    displayPanel.setDay(day());
    displayPanel.setMonth(month());
    displayPanel.setYear(year());
    displayPanel.setHourAndMinute(hour(), minute());

    // Set bottom row to "12.11. 1955 AM 06:38"
    displayPanel.setRow(0);
    displayPanel.setDay(12);
    displayPanel.setMonth(11);
    displayPanel.setYear(1955);
    displayPanel.setHourAndMinute(6, 38);
}

// char timeStr1[40];
// char timeStr2[40];

void loop() 
{
    commandInterface.handleInput();

    if(timeKeeper.pollTimeUpdate())
    {
        Serial.println("Time updated!");

        // On bottom row, show when time was last updated
        displayPanel.setRow(0);
        displayPanel.setDay(day());
        displayPanel.setMonth(month());
        displayPanel.setYear(year());
        displayPanel.setHourAndMinute(hour(), minute());
    }

    // if(timeKeeper.hasHWClock())
    // {
    //     DateTime hwTime = timeKeeper.getHWTime();
    //     sprintf(timeStr1, "HW: %02d.%02d.%04d %02d:%02d:%02d ",
    //         hwTime.day(),
    //         hwTime.month(),
    //         hwTime.year(),
    //         hwTime.hour(),
    //         hwTime.minute(),
    //         hwTime.second());
    // }
    // else
    //     sprintf(timeStr1, "No HW Clock! ");

    // sprintf(timeStr2, "SW: %02d.%02d.%04d %02d:%02d:%02d ", 
    //     day(),
    //     month(),
    //     year(),
    //     hour(),
    //     minute(),
    //     second());

    // // Set middle row to current system time
    displayPanel.setRow(1);
    displayPanel.setDay(day());
    displayPanel.setMonth(month());
    displayPanel.setYear(year());
    displayPanel.setHourAndMinute(hour(), minute());

    // Serial.print(timeStr1);
    // Serial.println(timeStr2);

    // delay(1000);
}


// TODO: Save displayPanel data internally, check if content changed, only update displayPanel if content changed.