#include "lib/util/Time.H"
#include <iomanip>
#include <vector>
#include <string>
#include <iostream>

// definitions here
namespace util
{
    namespace time
    {
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

        std::string to_string(Time t)
        {
            std::ostringstream os;
            date::operator<<(os, t);
            return os.str();
        }

        // NEW STUFF AFTER HERE!
        uint64_t days_between(DateTime t0, DateTime t1, bool weekday_only)
        {
            // this function is not generally correct, but will work in our special case for now.
            assert(t1 >= t0);

            Date d0 = Date{date::floor<date::days>(t0)};
            const Date d1 = Date{date::floor<date::days>(t1)};
            if (d0 == d1)
            {
                return 0;
            }
            uint64_t count{0};
            while (d0 < d1)
            {
                if (weekday_only)
                {
                    // in this case, only increment count on weekdays
                    count += is_weekday(d0) ? 1 : 0;
                }
                else 
                {
                    // in this case, always increment
                    count += 1;
                }
             
                d0 = date::sys_days{d0} + date::days{1};
            }
            return count;
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

        // Date next_day(date_)
        Date last_business_day(date::year_month ym)
        {
            return last_business_day(ym.year(), ym.month());
        }

        Date first_business_day(date::year y, date::month m)
        {
            // start with monday
            Date mi_d{y / m / date::Monday[1]};
            mi_d = std::min(Date{y / m / date::Tuesday[1]}, mi_d);
            mi_d = std::min(Date{y / m / date::Wednesday[1]}, mi_d);
            mi_d = std::min(Date{y / m / date::Thursday[1]}, mi_d);
            mi_d = std::min(Date{y / m / date::Friday[1]}, mi_d);
            return mi_d;
        }

        Date first_business_day(date::year_month ym)
        {
            return first_business_day(ym.year(), ym.month());
        }

        date::year_month next_year_month(date::year y, date::month m)
        {
            const bool is_dec = m == date::December;
            const auto yi = is_dec ? y + date::years{1} : y;
            const auto mi = is_dec ? date::January : m + date::months{1};
            return date::year_month(yi, mi);
        }

        date::year_month next_year_month(date::year_month ym)
        {
            return next_year_month(ym.year(), ym.month());
        }

        date::year_month next_year_month(Date ymd)
        {
            return next_year_month(ymd.year(), ymd.month());
        }

        std::vector<DateTime> compute_expiries(date::year y, date::month m, Time t, uint32_t e)
        {
            // construct e expiries from (y, m)
            std::vector<DateTime> expiries;
            Date e1 = last_business_day(y, m);
            expiries.push_back(combine(e1, t));
            uint32_t idx{0};
            while (idx < e)
            {
                const Date e2 = last_business_day(next_year_month(e1));
                expiries.push_back(combine(e2, t));
                e1 = e2;
                ++idx;
            }
            return expiries;
        }

        Time get_time(DateTime tf)
        {
            const Date df = Date{std::chrono::floor<std::chrono::days>(tf)};
            return tf - combine(df, std::chrono::minutes{0});
        }

        Date get_date(DateTime tf)
        { 
            return Date{date::floor<date::days>(tf)}; 
        }
        Date parse_date(std::string yyyymmdd)
        {
            if (yyyymmdd.size()!=8) 
            {
                std::cout << "Invalid YYYYMMDD string." << std::endl;
                std::exit(1);
            }
            int32_t yyyy = std::stol( std::string(yyyymmdd.begin(), yyyymmdd.begin()+4) );
            uint32_t mm = std::stoul (std::string( yyyymmdd.begin()+4, yyyymmdd.begin()+6));
            uint32_t dd = std::stoul( std::string( yyyymmdd.begin()+6, yyyymmdd.end()));
            return Date { date::year{yyyy}/date::month{mm}/date::day{dd} };
        }

        Date next_day(date::year_month_day ymd, int64_t d, bool only_wkday)
        {
            // go to next day.  note d can be negative
            // maybe only included weekdays
            date::sys_days sd{ymd};
            int64_t count{0};
            int64_t incr = d > 0 ? 1 : -1;
            while (count != d)
            {
                sd += date::days{incr};
                if (only_wkday && is_weekend(Date(sd)))
                {
                    continue;
                }
                count += incr;
            }
            return sd;
        }

        DateTime next_day(DateTime t, int64_t d, bool only_wkday)
        {
            // use the year_month_day function to calculate this
            const date::year_month_day d0 = Date{std::chrono::floor<std::chrono::days>(t)};
            const date::year_month_day dt = next_day(d0, d, only_wkday);
            const Time tf = get_time(t); // this is the time component only
            return combine(dt, tf);
        }
    }
}