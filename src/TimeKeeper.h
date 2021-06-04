#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#include <TimeLib.h>
#include <RTClib.h>

enum HWTimeError
{
    E_SUCCESS = 0,
    E_RTC_NOT_FOUND = -1
};

class TimeKeeper {
public:
    int begin();
    DateTime getHWTime();
    DateTime getSysTime();
    void adjustHWTime(const DateTime &dt);
private:
    DateTime sys_time;
    RTC_DS3231 rtc;
    bool has_hw_clock;
    bool has_time_source;
};

#endif //TIMEKEEPER_H