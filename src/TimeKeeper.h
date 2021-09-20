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
    TimeKeeper():
        has_hw_clock(false),
        has_time_source(false)
    {}

    void begin();
    bool hasHWClock();
    DateTime getHWTime();
    DateTime getLastTimeSync();
private:
    bool has_hw_clock;
    bool has_time_source;
    DateTime last_sync;
    RTC_DS3231 rtc;
    void adjustHWTime(const DateTime &dt);
};

#endif //TIMEKEEPER_H