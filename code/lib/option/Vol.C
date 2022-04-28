#include "lib/option/Vol.H"
#include "lib/option/BlackScholes.H"

namespace option 
{
    namespace vol 
    {
        ATMVol::ATMVol(double v0, double v1) : v0(v0), v1(v1), L( std::log(2*v1 / (v1 + v0)) ) {};
        double ATMVol::atm(double tau) const 
        {
            const double df { std::exp(-L*tau) };
            return v0 + v1 * (1.0 - df);
        }


        // if eps, rho are constructor arguments each particle will have their own Gatheral
        // im not sure if this is the best way, or to make them arguments?  
        Gatheral::Gatheral(const ATMVol& vol, double eps, double rho) 
            : atmVol(vol), n(2.016048* std::exp(eps)) , rho(rho) {}

        double Gatheral::phi(double theta) const
        {
            // again using gamma = 1/4
            const double arg0 { std::sqrt(std::sqrt(theta)) };         // ie g1 = 1/4
            const double arg1 { std::sqrt(std::sqrt(1 + b1*theta)) };  // ie g2 = 1/4
            const double arg2 { std::sqrt(1 + b2*theta) };
            return n / ( arg0 * arg1 * arg2 );
        }

        // vols in terms of atmf x
        double Gatheral::w_x(double x, double tau) const 
        {
            const double vt { atmVol.atm(tau) };
            const double theta { vt*vt*tau };
            const double phi_theta { phi(theta) };
            
            const double tmp0 { phi_theta*x + rho };
            const double arg_sqrt { tmp0*tmp0 + (1.0-rho)*(1.0-rho) };
            return 0.5*theta*(1.0 + rho*phi_theta*x + std::sqrt(arg_sqrt));
        }

        double Gatheral::v_x(double x, double tau) const 
        {   return std::sqrt(w_x(x,tau)/tau); }

        double Gatheral::w(double s, double K, double r, double tau) const
        {
            const double f { s*std::exp(r*tau) };
            const double x { std::log(K/f) };
            return w_x(x, tau);
        }

        double Gatheral::v(double s, double K, double r, double tau) const
        {   return std::sqrt( w(s,K,r,tau)/tau ); }

        
        double Gatheral::lvol(double s, double K, double r, double tau) const
        {
            auto vT = [&](double iTau) { return v(s,K,r,iTau); };
            auto vK = [&](double iK) { return v(s,iK,r,tau); } ;
            
            const double epsT { 0.0001 };
            const double epsK { 0.0001 };
            const double v_TK = vT(tau);

            // compute 1st and 2nd derivatives
            const double dVdT { 0.5*(vT(tau+epsT) - vT(tau-epsT))/epsT };
            const double dVdK { 0.5*(vK(K+epsK) - vK(K-epsK))/epsK };
            const double ddVdKK { (vK(K+epsK) - 2.0*v_TK + vK(K-epsK))/(epsK*epsK) };

            // evaluate 
            const double num {v_TK*v_TK + 2.0*tau*v_TK*dVdT + 2.0*r*K*tau*v_TK*dVdK };
            const double d1 { option::bs::d1(s,K,r,tau,v_TK) };
            const double den_arg1 { 1.0 + K*d1*std::sqrt(tau)*dVdK };
            const double den_arg2 { ddVdKK - d1*std::sqrt(tau)*dVdK*dVdK };
            const double den { den_arg1*den_arg1 + K*K*tau*v_TK*den_arg2 };
            return std::sqrt(num/den);
        }

    }
}