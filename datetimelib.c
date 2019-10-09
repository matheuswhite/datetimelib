#include "datetimelib.h"

static const u32_t _days_to_month[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 
};
static const u32_t _days_to_month_leap[] = {
    0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366
};

#define __MINUTE_SECS 60
#define __HOUR_SECS   3600
#define __DAY_SECS    86400
#define __YEAR_SECS   31536000

bool datetime_is_leap(u32_t year)
{

}

bool datetime_is_valid(datetime_t *datetime)
{

}

int datetime_add_datetime(datetime_t *start, datetime_t *period, datetime_t *result)
{
    result->seconds = start->seconds + period->seconds;
    if (result->seconds > 59) {
        result->seconds = result->seconds % 60;
        result->minutes = result->seconds / 60;
    }
}

int datetime_days_between_dates(datetime_t *start, datetime_t *end, u32_t *result)
{

}

int datetime2timestamp(datetime_t *datetime, timestamp_t *result)
{
    if (!datetime_is_valid(datetime)) {
        return -EINVAL;
    }

    u32_t years = datetime->years + 2000;
    u32_t months = datetime->months - 1;
    u32_t days = datetime->days - 1;

    *result = datetime->seconds;
    *result += datetime->minutes * __MINUTE_SECS;
    *result += datetime->hours * __HOUR_SECS;

    *result += days * __DAY_SECS;
    *result += (datetime_is_leap(years) ? 
                _days_to_month_leap[months] : 
                _days_to_month[months]) * __DAY_SECS;
    *result += (((years - 2000) / 4) + 1) * __DAY_SECS;
    *result += (years - 2000) * __YEAR_SECS;
}

int timestamp2datetime(timestamp_t *timestamp, datetime_t *result)
{
    result->seconds = *timestamp % 60;
    result->minutes = (*timestamp / 60) % 60;
    result->hours = (*timestamp / 3600) % 24;
    
    result->years = *timestamp / 31536000;
}

int datetime_get_weekday(datetime_t *datetime, weekday_t *result)
{

}
