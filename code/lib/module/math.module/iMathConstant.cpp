// module implementation unit for the Math module
// note that there are NO EXPORTS in here.  they are only allowed
// in the module interface file
module;

// again, the global module fragment is here.  you need to 
// compile these system headers 
import <cmath>;
import <numbers>;

module Math;


namespace math {

float normcdf(float x)
{
  
    // constants
    
    constexpr float inv_sqrt2  = 1.0f/std::numbers::sqrt2_v<float>;
    constexpr float a1 =  0.254829592;
    constexpr float a2 = -0.284496736;
    constexpr float a3 =  1.421413741;
    constexpr float a4 = -1.453152027;
    constexpr float a5 =  1.061405429;
    constexpr float p  =  0.3275911;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = std::abs(x)*inv_sqrt2;

    // A&S formula 7.1.26
    const float t = 1.0/(1.0 + p*x);
    const float y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*std::exp(-x*x);

    return 0.5*(1.0 + sign*y);
    
}

double normcdf(double x)
{

  constexpr double RT2PI = std::sqrt(std::numbers::pi_v<double>);
  constexpr double SPLIT = 7.07106781186547;
  constexpr double N0 = 220.206867912376;
  constexpr double N1 = 221.213596169931;
  constexpr double N2 = 112.079291497871;
  constexpr double N3 = 33.912866078383;
  constexpr double N4 = 6.37396220353165;
  constexpr double N5 = 0.700383064443688;
  constexpr double N6 = 3.52624965998911e-02;
  constexpr double M0 = 440.413735824752;
  constexpr double M1 = 793.826512519948;
  constexpr double M2 = 637.333633378831;
  constexpr double M3 = 296.564248779674;
  constexpr double M4 = 86.7807322029461;
  constexpr double M5 = 16.064177579207;
  constexpr double M6 = 1.75566716318264;
  constexpr double M7 = 8.83883476483184e-02;

  const double z = std::abs(x);
  double c = 0.0;

  if(z<=37.0)
  {
    const double e = std::exp(-0.50*z*z);
    if(z<SPLIT)
    {
      const double n = (((((N6*z + N5)*z + N4)*z + N3)*z + N2)*z + N1)*z + N0;
      const double d = ((((((M7*z + M6)*z + M5)*z + M4)*z + M3)*z + M2)*z + M1)*z + M0;
      c = e*n/d;
    }
    else
    {
      const double f = z + 1.0/(z + 2.0/(z + 3.0/(z + 4.0/(z + 13.0/20.0))));
      c = e/(RT2PI*f);
    }
  }
  return x<=0.0 ? c : 1-c;
  
}

}