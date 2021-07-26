#ifndef TIMEKEEPER_H
#define TIMEKEEPER_H

#include <TimeLib.h>
#include <RTClib.h>
#include <DCF77.h>

#define DCF77_PIN 2 // Connection pin to DCF 77 device
#define DCF77_INTERRUPT digitalPinToInterrupt(DCF77_PIN)

enum HWTimeError
{
    E_SUCCESS = 0,
    E_RTC_NOT_FOUND = -1
};

class TimeKeeper {
public:
    TimeKeeper():
        has_hw_clock(false),
        has_time_source(false),
        dcf77(DCF77_PIN,DCF77_INTERRUPT)
    {}

    void begin();
    bool hasHWClock();
    DateTime getHWTime();
    bool pollTimeUpdate();
    DateTime getLastTimeSync();
private:
    bool has_hw_clock;
    bool has_time_source;
    DateTime last_sync;
    RTC_DS3231 rtc;
    DCF77 dcf77;
    void adjustHWTime(const DateTime &dt);
};

#endif //TIMEKEEPER_H