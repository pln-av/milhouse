
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
    const uint32_t sim_length {50};
    const util::time::Date d0 { date::year{2000}/date::month{2}/1 };
    const util::time::Date df { util::time::next_day(d0, sim_length, true) };
    std::cout << "start date: " << d0 << " | end date: " << df << std::endl;

    // set expiry time, and thus start/end DateTimes
    const util::time::Time expiry_time = std::chrono::minutes{0}+ std::chrono::hours{15};
    const util::time::DateTime t0 { util::time::combine(d0, expiry_time) };
    const util::time::DateTime tf { util::time::combine(df, expiry_time) };
    std::cout << "Initial Snap: " << util::time::to_string(t0) << " | Final Snap: " << util::time::to_string(tf) << std::endl;
    
    // observations at t0 are our initial conditions
    // build expiries, starting at the montly corresponding to our starting month.
    // T == number of expiries
    // min_exp is shortest expiry considered
    /*
    const uint32_t T { 12 };
    const uint32_t min_exp {6};  
    auto&& expiries = util::time::compute_expiries(d0.year(), d0.month(), expiry_time, T);

    // begin simulation
    util::time::DateTime curr {t0 };
    util::time::DateTime fm { expiries.front() };
    while (curr<tf)
    {
        // detect if we need to roll to next expiry
        if (util::time::days_between(curr, expiries.front(), true)<min_exp)
        {
            // need to roll today
            util::time::roll_expiries(expiries);
            fm = expiries.front();
        }
        std::cout << "Calculation Dates: " << util::time::to_string(curr) << " vs Front Month: " << util::time::to_string(fm) << std::endl;
        curr = util::time::next_day(curr, 1, true);
    }

    */
}