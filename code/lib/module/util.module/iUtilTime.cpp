
module;

#include "date.h"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <assert.h>

export module UtilTime;

export
{
    namespace util
    {
        namespace time
        {
            // these will be the foundation date/times types for this project
            // the print function is because we don't have ostream ready for date::
            using Time = std::chrono::minutes;
            using Date = date::year_month_day;
            using DateTime = date::sys_time<Time>;

            // probably put this all in some inbuilt DateTime object
            DateTime combine(Date d, Time m);
            bool is_weekend(Date d); 
            bool is_weekday(Date d);
            //std::string to_string(Time t);
            //std::string to_string( const std::chrono::system_clock::time_point& time);
            std::string timePointAsString(const std::chrono::system_clock::time_point& tp);
            
            Date last_business_day(date::year y, date::month m);
        }
    }
}