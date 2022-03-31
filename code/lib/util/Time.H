#pragma once
#include "lib/util/date.h"

#include <chrono>
#include <string>

namespace util {
    namespace time {
        using Time = std::chrono::minutes;
        using Date = date::year_month_day;
        using DateTime = date::sys_time<Time>;

        // probably put this all in some inbuilt DateTime object
        // for now these are just small conversion functions
        DateTime combine(Date d, Time m);
        Time get_time(DateTime tf);

        // some usefull tests
        bool is_weekend(Date d);
        bool is_weekday(Date d);

        // streams dont seem to work nicely on gcc11 yet. use this instead for now
        std::string to_string(const std::chrono::system_clock::time_point &time);
        std::string to_string(Time t);

        // various usefull functions for finding correct expiries
        Date last_business_day(date::year y, date::month m);
        Date last_business_day(date::year_month ym);
        Date first_business_day(date::year y, date::month m);
        Date first_business_day(date::year_month ym);
        date::year_month next_year_month(date::year y, date::month m);
        date::year_month next_year_month(date::year_month ym);
        date::year_month next_year_month(Date ymd);
        DateTime next_day(DateTime t, int64_t d, bool only_wkday = false);
        Date next_day(date::year_month_day ymd, int64_t d, bool only_wkday = false);

        // some simple date algorithms
        uint64_t days_between(DateTime t0, DateTime t1, bool weekday_only = true);
    }
}

