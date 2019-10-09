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

static const u8_t __days_of_month[2][13] = {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                                    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

bool datetime_is_lerp(u32_t year)
{
    return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}

bool datetime_is_valid(datetime_t *datetime)
{
    if( datetime->seconds > 59 ){
        return false;
    }
    if( datetime->minutes > 59 ){
        return false;
    }
    if( datetime->hours > 23 ){
        return false;
    }
    if( datetime->months > 12 || !datetime->months ){
        return false;
    }
    if( __days_of_month[ datetime->months ] > datetime->days || !datetime->days ){
        return false;
    }
    return true;
}

int datetime_add_datetime(datetime_t *start, datetime_t *period, datetime_t *result)
{
    // Check datetime start
    if( datetime_is_valid(start) ) {
        memset(result, 0 , sizeof(datetime_t));
        return -1;
    }

    memcpy( result, start, sizeof(datetime_t) );

    u32_t seconds = start->seconds + period->seconds;
    result->seconds = seconds % 60;
    u32_t minutes = seconds/60 + start->minutes + period->minutes;
    result->minutes = minutes % 60;
    u32_t hours = minutes/60 + start->hours + period->hours;
    result->hours = hours % 24;

    // Count number of days in period
    u32_t days = hours/24 + period->days + 365 * period->years + 30 * period->months;

    datetime_t last_day_of_year = {
        .seconds = 0,
        .minutes = 0,
        .hours = 0,
        .days = 31,
        .months = 12,
        .years = start->years,
    };

    // Get number of days to end the year
    u32_t days_to_end_the_year = 0;
    if( datetime_days_between_dates(start, &last_day_of_year, &days_to_end_the_year) ){
        memset(result, 0, sizeof(datetime_t) );
        return -1;
    }

    // There are days to end the year
    u8_t is_lerp = datetime_is_lerp(result->years);
    if( days_to_end_the_year <= days ) {    
        days -= days_to_end_the_year;
        result->days = 1;
        result->months = 1;
        result->years++;

        // Jump year by year
        while( days >= datetime_is_lerp(result->years) + 365 ){ 
            days -= datetime_is_lerp(result->years) + 365;
            result->years++;
        }
        // There aren't days to end the year, but there are to end to month
    } else if( days + result->days > __days_of_month[is_lerp][result->months] ){
        days -= __days_of_month[is_lerp][result->months] - result->days;
        result->days = 1;
        result->months++;
    }

    // Jump month by month
    while ( days >= __days_of_month[is_lerp][result->months] ){ 
        days -= __days_of_month[is_lerp][result->months];
        result->months++;
    }

    // Sum the last days
    result->days += days; 

    return 0;
}

int datetime_days_between_dates(datetime_t *start, datetime_t *end, u32_t *result)
{
    *result = 0;

    // Check datetime start and end
    if( !datetime_is_valid(start) || !datetime_is_valid(end) ){
        return -EINVAL;
    }

    u32_t number_of_days_before, number_of_days_after;
    u32_t number_of_days_between_years = 0;
    u8_t is_lerp;

    // Get number of days to begin the year 
    is_lerp = datetime_is_lerp(start->years);
    number_of_days_before = start->days;
    for (int i = start->months - 1; i > 0; --i)
		number_of_days_before += __days_of_month[is_lerp][i];

    // Get number of days to end the year 
    is_lerp = datetime_is_lerp(end->years);
    number_of_days_after = end->days;
    for (int i = end->months - 1; i > 0; --i)
		number_of_days_after += __days_of_month[is_lerp][i];

    // Get number of days between the years
    int year = start->years;
    while ( year < end->years ) {
		number_of_days_between_years += 365 + datetime_is_lerp(year++);
    }
    
    *result = number_of_days_between_years - number_of_days_before + number_of_days_after;
    return 0;
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

// TODO
int timestamp2datetime(timestamp_t *timestamp, datetime_t *result)
{
    result->seconds = *timestamp % 60;
    result->minutes = (*timestamp / 60) % 60;
    result->hours = (*timestamp / 3600) % 24;
    
    result->years = *timestamp / 31536000;
}

int datetime_get_weekday(datetime_t *datetime, weekday_t *result)
{
    static datetime_t start = {
        .seconds = 0,
        .minutes = 0,
        .hours   = 0,
        .days    = 1,
        .months  = 1,
        .years   = 0,
    };
    u32_t days = {0};
    int err = 0;

    err = datetime_days_between_dates(&start, datetime, &days);
    if (err) {
        return err;
    }

    *result = (days - 1 + 6) % 7;

    return 0;
}
