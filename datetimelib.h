#ifndef __DATETIMELIB_H__
#define __DATETIMELIB_H__

#include <stdbool.h>
#include "datetimelib_types.h"

typedef enum __weekday {
    WEEKDAY_SUNDAY = 0,
    WEEKDAY_MONDAY,
    WEEKDAY_TUESDAY,
    WEEKDAY_WEDNESDAY,
    WEEKDAY_THURSDAY,
    WEEKDAY_FRIDAY,
    WEEKDAY_SATURDAY
} weekday_t;

typedef struct __datetime {
    u32_t seconds;
    u32_t minutes;
    u32_t hours;
    u32_t days;
    u32_t months;
    u32_t years;
} datetime_t;

typedef struct __datetime_ext {
    datetime_t datetime;
    weekday_t weekday;
} datetime_ext_t;

typedef u32_t timestamp_t;

bool datetime_is_lerp(u32_t year);
bool datetime_is_valid(datetime_t *datetime);
int datetime_add_datetime(datetime_t *start, datetime_t *period, datetime_t *result);
int datetime_days_between_dates(datetime_t *start, datetime_t *end, u32_t *result);
int datetime2timestamp(datetime_t *datetime, timestamp_t *result);
int timestamp2datetime(timestamp_t *timestamp, datetime_t *result);
int datetime_get_weekday(datetime_t *datetime, weekday_t *result);

#endif //__DATETIMELIB_H__
