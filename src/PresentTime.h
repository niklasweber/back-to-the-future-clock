#ifndef PRESENTTIME_H
#define PRESENTTIME_H

#include <Arduino.h>

typedef struct  {
  uint8_t Halfsecond;
  uint8_t Second;
  uint8_t Minute;
  uint8_t Hour;
  uint8_t Day;
  uint8_t Month;
  uint16_t Year;
} 	TimeElements_t;

class PresentTime
{
public:
    void setTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint8_t halfsecond = 0);
    // void setTime(int hr,int min,int sec,int day, int month, int yr);
    TimeElements_t now();
    void breakTime(int64_t timeInput, TimeElements_t &tm); // Get day, month, ... from unix ts
    int64_t makeTime(const TimeElements_t &tm);
private:
    void setTime(int64_t t);
    int64_t sysTime = 0; // call now() to update sysTime
    uint32_t prevMillis = 0;
};

#endif // PRESENTTIME_H