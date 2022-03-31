
module;
import <vector>;
#include "lib/util/iUtil.H"
module OptionUtil;

namespace option 
{ 
    namespace util { 
double func1(double x) { return x; }
double func2(double x) { return 2*x; }
std::vector<::util::time::DateTime> compute_expiries(date::year y, date::month m, ::util::time::Time t, uint32_t e);
}
}