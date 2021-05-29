#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#include <TimeLib.h>
#include <RTClib.h>
#include <TinyGPS++.h>

#define time_offset 3600  // define a clock offset of 3600 seconds (1 hour) ==> UTC + 1

// DS3231 i2c addr = 0x68 ?

// variable definitions
char Time[]  = "TIME: 00:00:00";
char Date[]  = "DATE: 00-00-2000";
byte last_second, Second, Minute, Hour, Day, Month;
int Year;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

class TimeKeeper {
public:
    int begin();
private:
    RTC_DS3231 rtc;
    TinyGPSPlus gps_device;
};

#endif //TIMEKEEPER_H