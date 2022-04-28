
#include <iostream>
#include <chrono>
#include <string>
#include <ctime>

#include "date.h"

#include "lib/util/Util.H"

int main()
{
    std::cout << "Test Util module. \n" << std::endl;
    // simulation start/end dates
    //const uint32_t sim_length {10000};
    const util::time::Date d0 { date::year{2038}/date::month{4}/30 };
    //const util::time::Date df { util::time::next_day(d0, sim_length, true) };
    const util::time::Date df { date::year{2039}/date::month(4)/29 };
    std::cout << "start date: " << d0 << " | end date: " << df << std::endl;

    // set expiry time, and thus start/end DateTimes
    const util::time::Time expiry_time = std::chrono::minutes{0}+ std::chrono::hours{15};
    const util::time::DateTime t0 { util::time::combine(d0, expiry_time) };
    const util::time::DateTime tf { util::time::combine(df, expiry_time) };
    std::cout << "Initial Snap: " << util::time::to_string(t0) << " | Final Snap: " << util::time::to_string(tf) << std::endl;
    
    std::cout << util::time::days_between(t0, tf, true) << std::endl;;
}