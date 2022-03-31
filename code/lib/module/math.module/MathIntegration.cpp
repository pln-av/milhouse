// module implementation unit for the Math module
// note that there are NO EXPORTS in here.  they are only allowed
// in the module interface file
module;

// again, the global module fragment is here.  you need to
// compile these system headers 
import <cmath>;
import <numbers>;

module MathIntegration;

namespace math { namespace integration { 
double func1(double x) { return x; }
double func2(double x) { return 2*x; }
}
}
