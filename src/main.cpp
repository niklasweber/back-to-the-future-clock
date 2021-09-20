#include <Arduino.h>
#include <RTClib.h>
#include <TimeLib.h>
#include "DisplayPanel.h"
#include "CommandInterface.h"

#include <BH1750.h>
#include <Wire.h>


#define BOOT_MIN_TIME 2000

BH1750 lightMeter;
RTC_DS3231 rtc;
DisplayPanel displayPanel;
CommandInterface commandInterface;

bool showTime = true;
uint8_t timeRow = 1;
uint8_t messageRow = 1;

void onSetSegments(cmd_set_segments& cmd)
{
    if(cmd.startPos + cmd.length > displayPanel.getSegmentsMax())
        return;

    displayPanel.setSegments(cmd.segments, cmd.length, cmd.startPos);
}

void onShowTime(cmd_show_time& cmd)
{
    // Serial.println("onShowTime");

    if(!cmd.on)
        showTime = false;
    else
    {
        showTime = true;
        if(cmd.row >= displayPanel.getRows())
            cmd.row = displayPanel.getRows()-1;
    }
}

void setup()
{
    displayPanel.begin();

    Serial.begin(9600);
    Serial.println("Back to the future clock - Start");

    Wire.begin();

    if(rtc.begin()) 
    {
        setTime(rtc.now().unixtime());
    }
    else
    {
        displayPanel.setRow(messageRow);
        displayPanel.showRTCError();
        delay(2000);
    }

    lightMeter.begin();

    if(!commandInterface.begin(&onSetSegments, &onShowTime))
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

void loop() 
{
    commandInterface.handleInput();

    // Set middle row to current system time
    if(showTime)
    {
        displayPanel.setRow(timeRow);
        displayPanel.setDay(day());
        displayPanel.setMonth(month());
        displayPanel.setYear(year());
        displayPanel.setHourAndMinute(hour(), minute());
    }

    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx");

    if(lux > 5)
    {
        displayPanel.setBrightnessAll(7);
    }
    else
    {
        displayPanel.setBrightnessAll(1);
    }

    delay(1000);
}