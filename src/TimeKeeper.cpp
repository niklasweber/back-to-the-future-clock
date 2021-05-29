#include "TimeKeeper.h"

int TimeKeeper::begin()
{
    if (!rtc.begin())
        return E_RTC_NOT_FOUND;

    return E_SUCCESS;
}

DateTime TimeKeeper::getHWTime()
{
    return rtc.now();
}

void TimeKeeper::adjustHWTime(const DateTime &dt)
{
    rtc.adjust(dt);

    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}