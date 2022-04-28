#pragma once
#include <cmath>
#include "lib/math/Constant.H"
namespace math {
    namespace sf {
        double normcdf(double x);
        float normcdf(float x);

        template <typename T>
        constexpr T normpdf(T x, T mu=T(0.0), T sigsq = T(1.0))
        {
            constexpr T sig { std::sqrt(sigsq)};
            constexpr T inv_sig { T(1.0)/sig };
            constexpr T z { (x - mu)*inv_sig };
            constexpr T arg { -0.50*z*z };
            return inv_sig * math::constant::inv_sqrt_2pi<T> * std::exp(arg);
        }
    }
}