#pragma once
#include <vector>
#include <iostream>
#include <limits>
#include <cmath>

namespace option 
{
    namespace vol
    {
        template <typename T>
        constexpr T atfm_k(T s, T r, T tau, T x)
        {   return s*std::exp( x + r*tau); }

        template <typename T>
        constexpr T atfm_x(T s, T r, T tau, T k)
        {   return std::log(k/s) - r*tau; } 

        template <typename T>
        constexpr T interp(T t, std::pair<T,T> t0, std::pair<T,T> t1)
        {
            // interp between (t0, s0), (t1, s1)
            assert(t0.first < t1.first);
            assert(t0.first < t);
            
            const T s02 { t0.second*t0.second };
            const T s12 { t1.second*t1.second };
            const T frac { (t - t0.first)/(t1.first - t0.first) };
            return std::sqrt( s02 + frac*s12 );
        }

        class ATMVol
        {
        public:
            ATMVol(double v0, double v1);
            double atm(double tau) const;
        private:
            const double v0 {0.0};
            const double v1 {0.0};
            const double L {0.0};
        };

        class Gatheral
        {
        public:

            // if eps, rho are constructor arguments each particle will have their own Gatheral
            // im not sure if this is the best way, or to make them arguments?  
            Gatheral(const ATMVol& vol, double eps, double rho);
            double phi(double theta) const ;

            // vols in terms of atmf x
            double w_x(double x, double tau) const;
            double v_x(double x, double tau) const;

            // vols in terms of dollars
            double w(double s, double K, double r, double tau) const;
            double v(double s, double K, double r, double tau) const;

            // lvol in terms of dollars
            double lvol(double s, double K, double r, double tau) const;
            

        private:
            // constexpr g1 { 0.250 }
            // constexpr g2 { 0.250 }  // implied in my w formulas

            // constants
            const double b1 { std::exp(double(5.18)) };
            const double b2 { std::exp(double(-3.0)) };
            const double one_on_six = 1.0/double(6.0);

            // parameters
            // atm vol function
            const ATMVol& atmVol;
            double n;
            double rho;          
          
        };
    }
}