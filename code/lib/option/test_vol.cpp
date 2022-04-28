#include <iostream>
#include <vector>

#include "lib/option/Vol.H"
#include "lib/option/BlackScholes.H"

int main()
{
    std::cout << "Test Vol Functions." << std::endl;
    std::cout << "\n\n" << std::endl;

    std::cout << " -- testing atm vol function. " << std::endl;
    const double v0 { 0.15 };
    const double v1 { 0.30 };
    option::vol::ATMVol atmVol(v0, v1);

    const double t0 { 0.0 };
    const double tf { 2.0 };
    const uint64_t nT { 7 };
    const double dt { (tf-t0)/ (nT-1.0) };
    for (size_t i=0; i<nT; ++i)
    { 
        const double t = t0 + i*dt;
        std::cout << "atm(" << t << ")=" << atmVol.atm(t) << std::endl; 
    }
    std::cout << "\n" << std::endl;

    // choose an expiry, and evalue the implied vol function.
    // test in both dollar and x space
    std::cout << " -- testing Gatheral vol function " << std::endl;
    const double eta { 0.25 };
    const double rho { -0.50 };
    option::vol::Gatheral gatheral(atmVol, eta, rho);

    const double tau { 0.75 };
    const double spot { 100.0 };
    const double rate { 0.02 };
    const std::vector<double> x {-2.0, -1.5, -0.5, 0, 0.5, 1.5, 2.0};
    for (auto&& xi : x)
    {
        const double ki { option::bs::atfm_k(spot,rate,tau,xi) };
        std::cout << "xi: " << xi << " | w(x)=" << std::sqrt(gatheral.w_x(xi, tau)/tau) 
                                  << " | w(k)=" << std::sqrt(gatheral.w(spot,ki,rate,tau)/tau)
                                  << " | v(k)=" << gatheral.v(spot,ki,rate,tau) 
                                  << " | lv(k)=" << gatheral.lvol(spot,ki,rate,tau) 
                                  << std::endl;
    }
    
}