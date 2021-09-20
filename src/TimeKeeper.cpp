#include "TimeKeeper.h"

void TimeKeeper::begin()
{
    if (rtc.begin()) {
        has_hw_clock = true;
        setTime(rtc.now().unixtime());
    }
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