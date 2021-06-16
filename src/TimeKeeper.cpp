#include "TimeKeeper.h"

void TimeKeeper::begin()
{
    if (rtc.begin()) {
        has_hw_clock = true;
        setTime(rtc.now().unixtime());
    }

    dcf77.Start();
}

bool TimeKeeper::hasHWClock()
{
    return has_hw_clock;
}

DateTime TimeKeeper::getHWTime()
{
    if(has_hw_clock)
        return rtc.now();

    return DateTime();
}

void TimeKeeper::adjustHWTime(const DateTime &dt)
{
    if(has_hw_clock)
        rtc.adjust(dt);
}

DateTime TimeKeeper::getLastTimeSync()
{
    return last_sync;
}

bool TimeKeeper::pollTimeUpdate()
{
    DateTime dcf77time = DateTime(dcf77.getTime());
    if (dcf77time.unixtime() != 0)
    {
        adjustHWTime(dcf77time);
        setTime(dcf77time.unixtime());
        last_sync = dcf77time;
        has_time_source = true;
        return true;
    }
    return false;
}