#include "date_time.h"

#define NULL ((void *)0)

static s_DateTime s_RealTimeClock_context = {1, 1, 0, 0, 0, 0};

static inline uint8_t isLeapYear(uint16_t fullYear)
{
    return ((fullYear % 4 == 0) && ((fullYear % 100 != 0) || (fullYear % 400 == 0))) ? 1 : 0;
}

static inline uint8_t getMaxDays(uint8_t month, uint16_t fullYear)
{
    static const uint8_t daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2) {
        return 28 + isLeapYear(fullYear);
    } else {
        return daysInMonth[month - 1];
    }
}

void date_time_init(void)
{
    s_RealTimeClock_context.year = 0;  // 2000
    s_RealTimeClock_context.month = 1; // January
    s_RealTimeClock_context.day = 1;
    s_RealTimeClock_context.hour = 0;
    s_RealTimeClock_context.minute = 0;
    s_RealTimeClock_context.second = 0;
}

void date_time_set(uint8_t day, uint8_t hour, uint8_t minute, uint8_t second)
{
    s_RealTimeClock_context.year = 0;  // 2000
    s_RealTimeClock_context.month = 1; // January
    s_RealTimeClock_context.day = day;
    s_RealTimeClock_context.hour = hour;
    s_RealTimeClock_context.minute = minute;
    s_RealTimeClock_context.second = second;
}

void date_time_update(void)
{
    static uint16_t ms_counter = 0;

    if (++ms_counter >= 1000) // 1000 ms = 1 second
    {
        ms_counter = 0;
        if (++s_RealTimeClock_context.second >= 60)
        {
            s_RealTimeClock_context.second = 0;
            if (++s_RealTimeClock_context.minute >= 60)
            {
                s_RealTimeClock_context.minute = 0;
                if (++s_RealTimeClock_context.hour >= 24)
                {
                    s_RealTimeClock_context.hour = 0;
                    if (++s_RealTimeClock_context.day > getMaxDays(s_RealTimeClock_context.month, s_RealTimeClock_context.year))
                    {
                        s_RealTimeClock_context.day = 1;
                        if (++s_RealTimeClock_context.month > 12)
                        {
                            s_RealTimeClock_context.month = 1;
                            if (++s_RealTimeClock_context.year > 99)
                            {
                                s_RealTimeClock_context.year = 0;
                            }
                        }
                    }
                }
            }
        }
    }
}


void date_time_get(uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
    if (days) *days = s_RealTimeClock_context.day;
    if (hours) *hours = s_RealTimeClock_context.hour;
    if (minutes) *minutes = s_RealTimeClock_context.minute;
    if (seconds) *seconds = s_RealTimeClock_context.second;
}
