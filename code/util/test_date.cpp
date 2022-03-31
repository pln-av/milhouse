#include <iostream>
#include <chrono>
#include <vector>
#include "date.h"
// date.h is a lib from howard hinnant that makes up date time upgrades to std::chrono
// these are basically not implemented even in gcc11.  but apparently date.h is VERY close.
// documentation of date.h is available at 
// https://howardhinnant.github.io/date/date.html

// usings from docs

using namespace date;
using namespace std::chrono;

// these will be the foundation date/times types for this project
using Time = std::chrono::minutes;
using Date = date::year_month_day;
using DateTime = date::sys_time<Time>;

// little utility for clarity
DateTime combine(Date d, Time m)
{ return sys_days(d) + std::chrono::duration_cast<Time>(m); }

// these are needed for now, if you want to avoid opening 
Date last_business_day(date::year y, date::month m)
{
    // start with monday
    Date mi_d {y/m/Monday[last]};
    mi_d = std::max( Date{y/m/Tuesday[last]},   mi_d);
    mi_d = std::max( Date{y/m/Wednesday[last]}, mi_d);
    mi_d = std::max( Date{y/m/Thursday[last]},  mi_d);
    mi_d = std::max( Date{y/m/Friday[last]},    mi_d);
    return mi_d;
}

Date last_business_day(year_month ym) 
{ 
    return last_business_day(ym.year(), ym.month()); 
}

Date first_business_day(date::year y, date::month m)
{
    // start with monday
    Date mi_d {y/m/Monday[1]};
    mi_d = std::min( Date{y/m/Tuesday[1]},   mi_d );
    mi_d = std::min( Date{y/m/Wednesday[1]}, mi_d);
    mi_d = std::min( Date{y/m/Thursday[1]},  mi_d);
    mi_d = std::min( Date{y/m/Friday[1]},    mi_d);
    return mi_d;
}

Date first_business_day(year_month ym) 
{ 
    return first_business_day(ym.year(), ym.month()); 
}

year_month next_year_month(date::year y, date::month m)
{
    const auto yi = m == date::December ? y + years{1} : y;
    const auto mi = m == date::December ? date::January : m + months{1};
    return year_month(yi, mi);
}

year_month next_year_month(year_month ym) 
{ 
    return next_year_month(ym.year(), ym.month() ); 
}

year_month next_year_month(Date ymd)
{
    return next_year_month(ymd.year(), ymd.month());
}

std::vector<Date> compute_expiries(date::year y, date::month m, uint32_t e)
{
    // construct expiries from (y, m)
    std::vector<Date> expiries;
    auto e1 = last_business_day(y, m);
    expiries.push_back(e1);
    uint32_t idx {0};
    while (idx < e)
    {
        const auto e2 = last_business_day(next_year_month(e1));
        expiries.push_back(e2);
        e1 = e2;
        ++idx;
    }
    return expiries;
}

;
int main()
{
    // Construct Month Expiries

    // need to construct data structure of sys_minute objects
    //std::vector<sys_minute> expiries;

    int y { 2022 };
    uint32_t m { 1 };
    
    
    auto&& expiries = compute_expiries( date::year{y}, date::month{m}, 15);
    for (auto&& e : expiries)
    {
        std::cout << "Expiry: " << e << std::endl;
    }
    std::chrono::minutes mk {60};
    std::chrono::hours hk {16};
    std::cout << mk << std::endl;
    std::cout << hk << std::endl;
    auto ok = mk + hk;
    std::cout << ok << std::endl;

    DateTime tp { combine(expiries.back(), ok)  };
    std::cout << tp << std::endl;
    
    
    /*
    ymd e1 = last_business_day(date::year{y}, date::month{m});
    uint32_t T {15};
    uint32_t idx {0};
    std::cout << "e1: " << e1  << std::endl;
    while (idx < T) 
    {
        ymd e2 = last_business_day( next_year_month( e1 ) );
        std::cout << "e2: " << e2 << std::endl;
        e1 = e2;
        ++idx;
    }
    */
    //auto nm = next_year_month(e1);
    //std::cout << "nm: " << nm << std::endl;
    //ymd e2 = last_business_day(nm);
    //expiries.emplace_back( sys_minute)
    
    //std::cout << days(2022_y) << std::endl;
    std::cout << "Test date.h" << std::endl;
    /*
    constexpr auto d1 { 2022_y/March/Friday[3] };
    constexpr auto d2 = Date { d1 };  

    // loop over every month 
    for (auto d=d2; d.year()==d1.year(); d+=months{1}){
        
        // increment over all remaining months in the year.
        //if ((weekday{d}==Sunday)||(weekday{d}==Saturday))
        //   std::cout << "Weekend: " << d << std::endl;

        // construct time_point object, which has required granuality
        // its type is complicated though so just use auto i guess
        // system time is UTC by default.  lets leave it there, OK?
        const auto tp1 { sys_days{d} + 13h + 15min };  // sys_days = time_point<system_clock, days>
        const auto tp2 { sys_days{d} + 13h + 30min };
        const auto tp3 { sys_days{d} + 13h + 10min + 0s};

        date::sys_time<std::chrono::minutes> tp5 { sys_days{d} + 10h + 10min };
       
        sys_minute tp6 { sys_days{d} + 16h + 30min};
        tp6 += days{1};
        std::cout << "d: " << d << "| tp6: " << tp6 << std::endl; 
        //const auto dtp = tp2 - tp1;
        //std::cout << tp1 << std::endl;
        
    }

    // given year, month, return me year_month_date of last business day
    const auto y = year{2022};
    const auto m = 3;


    const auto day1 = Date{year{2022}/month{3}/Friday[last]};
    std::cout << day1 << std::endl;
   

    
    //std::chrono::minutes m;
    auto o = last_business_day(year{2022}, month{3});
    for (uint16_t u=0; u<15; ++u) { std::cout << date::month{u} << std::endl; }
    */  
}
