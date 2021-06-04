#include "TimeKeeper.h"

int TimeKeeper::begin()
{
    if (!rtc.begin()) {
        has_hw_clock = false;
        return E_RTC_NOT_FOUND;
    }
    has_hw_clock = true;
    has_time_source = false; // Set to true when validated.

    return E_SUCCESS;
}

DateTime TimeKeeper::getHWTime()
{
    return rtc.now();
}

DateTime TimeKeeper::getSysTime()
{
    return sys_time;
}

void TimeKeeper::adjustHWTime(const DateTime &dt)
{
    rtc.adjust(dt);
}