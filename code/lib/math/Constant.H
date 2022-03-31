#pragma once
#include <numbers>

namespace math 
{ 
    namespace constant 
    { 
        template <typename T>
        constexpr T inv_sqrt_2pi = std::numbers::inv_sqrtpi_v<T>/std::numbers::sqrt2_v<T>;
    }
}