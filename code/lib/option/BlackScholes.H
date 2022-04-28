#pragma once

#include <cmath>
#include "lib/math/SpecialFunction.H"
#include "lib/option/Infra.H"
#include "lib/option/Vol.H"

namespace option
{
    namespace bs
    {

        template <typename T>
        T d1(T s, T K, T r, T tau, T sig)
        {
            const T arg1 = std::log(s/K);
            const T arg2 = (r + 0.5*sig*sig)*tau;
            return (arg1 + arg2)/(sig*std::sqrt(tau)); 
        };

        template <typename T>
        T d2(T s, T K, T r, T tau, T sig)
        {
            const T arg1 = std::log(s/K);
            const T arg2 = (r - 0.5*sig*sig)*tau;
            return (arg1 + arg2)/(sig*std::sqrt(tau));
        }

        template <typename T>
        T bs_call(T s, T K, T r, T tau, T sig)
        {
            const T _d1 = d1(s,K,r,tau,sig);
            const T _d2 = d2(s,K,r,tau,sig);
            const T df = std::exp(-r*tau);
            return s*math::sf::normcdf(_d1) - K*df*math::sf::normcdf(_d2);
        }

        template <typename T>
        constexpr T atfm_k(T s, T r, T tau, T x)
        {   return s*std::exp(x + r*tau); }

        template <typename T>
        constexpr T atfm_x(T s, T r, T tau, T K)
        {   return s*std::exp(s/K) - r*tau; }


        void priceBSGrid(option::infra::OptionGrid& grid, const option::vol::Gatheral& gatheral);
    }
}

