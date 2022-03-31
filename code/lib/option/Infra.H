#pragma once
#include <vector>
#include "lib/util/Time.H"

namespace option 
{
    namespace infra
    {
        void roll_expiries(std::vector<util::time::DateTime> &expiries);
        std::vector<util::time::DateTime> compute_expiries(date::year y, date::month m, util::time::Time t, uint32_t e);
    }
}


