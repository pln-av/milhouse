module;

#include "lib/util/iUtil.H"
import <vector>;
export module OptionUtil;


export
{
    namespace option
    {
        namespace util
        {
            double func1(double x);
            double func2(double x);

            std::vector<::util::time::DateTime> compute_expiries(date::year y, date::month m, ::util::time::Time t, uint32_t e);
            //void roll_expiries(std::vector<DateTime> &expiries);
        }
    }
}