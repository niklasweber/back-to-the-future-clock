#include "PresentTime.h"

TimeElements_t PresentTime::now() {
    TimeElements_t now;
    // calculate number of halfseconds passed since last call to now()
    while (millis() - prevMillis >= 500) {
        // millis overflow dealt with!
        // millis() and prevMillis are both unsigned ints thus the subtraction will always be the absolute value of the difference
        sysTime++;
        prevMillis += 500;
    }
    breakTime(sysTime, now);
    return now;
}

void PresentTime::setTime(int64_t t) { 
  sysTime = t;
  prevMillis = millis(); // restart counting from now
}

void PresentTime::setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t halfsecond) {
    TimeElements_t now;
    now.Year = year;
    now.Month = month;
    now.Day = day;
    now.Hour = hour;
    now.Minute = minute;
    now.Second = second;
    now.Halfsecond = halfsecond;
    setTime(makeTime(now));
}

// leap year calulator expects year argument as years offset from 1970
// #define LEAP_YEAR(Y)     ( ((1970+(Y))>0) && !((1970+(Y))%4) && ( ((1970+(Y))%100) || !((1970+(Y))%400) ) )
#define LEAP_YEAR(Y)     ( (((Y))>0) && !(((Y))%4) && ( (((Y))%100) || !(((Y))%400) ) )
static  const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31}; // API starts months from 1, this array starts from 0

void PresentTime::breakTime(int64_t timeInput, TimeElements_t &tm){
    // break the given timeInput into time components (month, day, ...)

    // Convert timestamp to use offset from 0 instead of from 1970 / add seconds of 1970 years
    //timeInput += 62125920000LL;
    // timeInput += 62167219200LL;

    int64_t year;
    int64_t month, monthLength;
    int64_t days;

    tm.Halfsecond = timeInput % 2;
    timeInput /= 2; // now it is seconds
    tm.Second = timeInput % 60;
    timeInput /= 60; // now it is minutes
    tm.Minute = timeInput % 60;
    timeInput /= 60; // now it is hours
    tm.Hour = timeInput % 24;
    timeInput /= 24; // now it is days

    year = 0; // Offset from year 0
    days = 0;

    while((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= timeInput) {
        year++;
    }
    tm.Year = year;

    days -= LEAP_YEAR(year) ? 366 : 365;
    timeInput -= days; // now it is days in this year, starting at 0

    days=0;
    month=0;
    monthLength=0;
    for (month=0; month<12; month++) {
        if (month==1) { // february
            if (LEAP_YEAR(year)) {
                monthLength=29;
            } else {
                monthLength=28;
            }
        } else {
            monthLength = monthDays[month];
        }
        
        if (timeInput >= monthLength) {
            timeInput -= monthLength;
        } else {
            break;
        }
    }
    tm.Month = month + 1;  // jan is month 1  
    tm.Day = timeInput + 1; // day of month
}

int64_t PresentTime::makeTime(const TimeElements_t &tm){
    // assemble time elements into int64_t
    int64_t halfseconds;

    halfseconds = tm.Year * 31536000LL;
    for (int i = 0; i < tm.Year; i++) {
        if (LEAP_YEAR(i)) {
            halfseconds +=  86400; // add extra days for leap years
        }
    }
  
    // add days for this year, months start from 1
    for (int i = 1; i < tm.Month; i++) {
        if ( (i == 2) && LEAP_YEAR(tm.Year)) {
            halfseconds += 86400 * 29;
        } else {
            halfseconds += 86400 * monthDays[i-1];  //monthDay array starts from 0
        }
    }
    halfseconds += (tm.Day-1) * 86400;
    halfseconds += tm.Hour * 3600;
    halfseconds += tm.Minute * 60;
    halfseconds += tm.Second;
    halfseconds *= 2;
    halfseconds += tm.Halfsecond;
    return halfseconds;
}

bool PresentTime::isSyncTimeDue()
{
    if (nextSyncTime <= sysTime)
        return true;
    else
        return false;
}

void PresentTime::markSynced()
{
    nextSyncTime = sysTime + (60 * 2); //  1 minute in halfseconds
    Serial.println("Synced time with rtc");
}