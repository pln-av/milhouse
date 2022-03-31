/*
 * Demonstrate using time/calendar types all based on Hinnant's calendar 
 * library here:
 * https://howardhinnant.github.io/date/date.html
 * Note that this is all coming to c++20 but is not all implemented in my
 * gcc11, as far as I can tell.  So I will switch away from Hinnant's 
 * personal lib when possible but for now this is a mix of 
 * date:: and std::chrono::
 */
 

#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>
#include <assert.h>

#include "date.h"


// these will be the foundation date/times types for this project
// the print function is because we don't have ostream ready for date::
using Time = std::chrono::minutes;
using Date = date::year_month_day;
using DateTime = date::sys_time<Time>;

// little utility for clarity
DateTime combine(Date d, Time m)
{ 
    return date::sys_days(d) + std::chrono::duration_cast<Time>(m); 
}

bool is_weekend(Date d)
{ 
    const auto wd { date::weekday{d} };
    return (wd==date::Saturday)||(wd==date::Sunday); }

bool is_weekday(Date d) 
{ 
    return !is_weekend(d); 
}

uint64_t days_between(DateTime t0, DateTime t1, bool weekday_only=true)
{
    // this function is not generally correct, but will work in our special case for now.
    assert(t1 >= t0);
    
    Date d0 = Date{date::floor<date::days>(t0)};
    const Date d1 = Date{date::floor<date::days>(t1)};
    if (d0==d1) { return 0; }
    uint64_t count {0};
    while (d0 < d1) 
    { 
        if (weekday_only && is_weekday(d0)) { ++count; }
        d0 = date::sys_days{d0} + date::days{1};
    }
    return count;
}
    
// these two to_string methods are needed for now to avoid opending namespace date::
// completely for streaming
std::string to_string(Time t)
{ 
    std::ostringstream os;
    date::operator<<(os, t);
    return os.str();    
}

std::string to_string( const std::chrono::system_clock::time_point& time)
{
    // this is essentially serializeTimePoint from Adrian Maire off stackoverflow
    const std::string format {"%Y%m%d %H:%M:%S UTC"};
    const std::time_t tt = std::chrono::system_clock::to_time_t(time);
    const std::tm tm = *std::gmtime(&tt); //GMT (UTC)
    std::stringstream ss;
    ss << std::put_time( &tm, format.c_str() );
    return ss.str();
}

Date last_business_day(date::year y, date::month m)
{
    // start with monday
    Date mi_d {y/m/date::Monday[date::last]};
    mi_d = std::max( Date{y/m/date::Tuesday[date::last]},   mi_d);
    mi_d = std::max( Date{y/m/date::Wednesday[date::last]}, mi_d);
    mi_d = std::max( Date{y/m/date::Thursday[date::last]},  mi_d);
    mi_d = std::max( Date{y/m/date::Friday[date::last]},    mi_d);
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
    Date mi_d {y/m/date::Monday[1]};
    mi_d = std::min( Date{y/m/date::Tuesday[1]},   mi_d );
    mi_d = std::min( Date{y/m/date::Wednesday[1]}, mi_d);
    mi_d = std::min( Date{y/m/date::Thursday[1]},  mi_d);
    mi_d = std::min( Date{y/m/date::Friday[1]},    mi_d);
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
    return next_year_month(ym.year(), ym.month() ); 
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
    expiries.push_back( combine(e1,t) );
    uint32_t idx {0};
    while (idx < e)
    {
        const Date e2 = last_business_day(next_year_month(e1));
        expiries.push_back( combine(e2,t) );
        e1 = e2;
        ++idx;
    }
    return expiries;
}

Time get_time(DateTime tf) 
{
    const Date df = Date{ std::chrono::floor<std::chrono::days>(tf) };
    return tf - combine(df, std::chrono::minutes{0});
}
void roll_expiries(std::vector<DateTime>& expiries)
{
    const size_t n = expiries.size();

    // compute next expiry
    const DateTime tf = expiries.back();
    const Date df = Date{ std::chrono::floor<std::chrono::days>(tf) };
    //const Time exp_time = tf - combine(df, std::chrono::minutes{0}) ;   
    const Time exp_time = get_time(tf);
    const Date ndf = last_business_day(next_year_month(df.year(), df.month()));
    
    // roll forward
    for (size_t i=0; i<n-1; ++i) { expiries[i] = expiries[i+1]; }
 
    // place at the end
    expiries.back() = combine(ndf, exp_time);
    return;
}

date::year_month_day next_day(date::year_month_day ymd, int64_t d, bool only_wkday=false)
{
    // go to next day.  note d can be negative
    // maybe only included weekdays
    date::sys_days sd{ ymd };
    int64_t count {0};
    int64_t incr = d>0 ? 1 : -1;
    while (count != d)
    {
        sd += date::days{incr};
        if (only_wkday && is_weekend(Date(sd))) { continue; }
        count += incr;
    }
    return sd;
}

DateTime next_day(DateTime t, int64_t d, bool only_wkday=false)
{
    // use the year_month_day function to calculate this
    const date::year_month_day d0 = Date{std::chrono::floor<std::chrono::days>(t)};
    const date::year_month_day dt = next_day(d0, d, only_wkday);
    const Time tf = get_time(t);  // this is the time component only
    return combine(dt, tf);
}

std::string timePointAsString(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::string ts = std::ctime(&t);
    ts.resize(ts.size()-1);
    return ts;
}

int main()
{

    std::cout << "Test Date/Time/Calendars." << std::endl;

    // simulation start/end dates
    const uint32_t sim_length {50};
    const Date d0 { date::year{2000}/date::month{2}/1 };
    const Date df { next_day(d0, sim_length, true) };
    std::cout << "start date: " << d0 << " | end date: " << df << std::endl;

    // set expiry time, and thus start/end DateTimes
    const Time expiry_time = std::chrono::minutes{0}+ std::chrono::hours{15};
    const DateTime t0 { combine(d0, expiry_time) };
    const DateTime tf { combine(df, expiry_time) };
    std::cout << "Initial Snap: " << to_string(t0) << " | Final Snap: " << to_string(tf) << std::endl;
    std::cout << "Alternative: " << timePointAsString(t0) << std::endl;
    // observations at t0 are our initial conditions
    // build expiries, starting at the montly corresponding to our starting month.
    // T == number of expiries
    // min_exp is shortest expiry considered
    const uint32_t T { 12 };
    const uint32_t min_exp {6};  
    auto&& expiries = compute_expiries(d0.year(), d0.month(), expiry_time, T);
    std::cout << "Initial Expiries: " << std::endl;
    for (auto&& e : expiries) { std::cout << " -- " << to_string(e) << std::endl; }

    // begin simulation
    DateTime curr {t0 };
    DateTime fm { expiries.front() };
    while (curr<tf)
    {
        // detect if we need to roll to next expiry
        if (days_between(curr, expiries.front(), true)<min_exp)
        {
            // need to roll today
            roll_expiries(expiries);
            fm = expiries.front();
        }
        std::cout << "Calculation Dates: " << to_string(curr) << ", front: " << to_string(fm) << std::endl;
        curr = next_day(curr, 1, true);
    }
    
}