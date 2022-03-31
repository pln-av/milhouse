

module;

// again, the global module fragment is here.  you need to
// compile these system headers

#include "date.h"

#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <assert.h>
#include <ctime> 

module UtilTime;

namespace util
{
    namespace time
    {
        // little utility for clarity
        DateTime combine(Date d, Time m)
        {
            return date::sys_days(d) + std::chrono::duration_cast<Time>(m);
        }

        bool is_weekend(Date d)
        {
            const auto wd{date::weekday{d}};
            return (wd == date::Saturday) || (wd == date::Sunday);
        }

        bool is_weekday(Date d)
        {
            return !is_weekend(d);
        }
        
        std::string to_string(Time t)
        {
            std::ostringstream os;
            date::operator<<(os, t);
            return os.str();
        }

        
        std::string to_string(const std::chrono::system_clock::time_point &time)
        {
            // this is essentially serializeTimePoint from Adrian Maire off stackoverflow
            const std::string format{"%Y%m%d %H:%M:%S UTC"};
            const std::time_t tt = std::chrono::system_clock::to_time_t(time);
            const std::tm tm = *std::gmtime(&tt); // GMT (UTC)
            std::stringstream ss;
            ss << std::put_time(&tm, format.c_str());
            return ss.str();
        } 

        Date last_business_day(date::year y, date::month m)
        {
            // start with monday
            Date mi_d{y / m / date::Monday[date::last]};
            mi_d = std::max(Date{y / m / date::Tuesday[date::last]}, mi_d);
            mi_d = std::max(Date{y / m / date::Wednesday[date::last]}, mi_d);
            mi_d = std::max(Date{y / m / date::Thursday[date::last]}, mi_d);
            mi_d = std::max(Date{y / m / date::Friday[date::last]}, mi_d);
            return mi_d;
        }
    }
}
