#include <iostream>
#include <cassert>
#include <random>
#include <fstream>

#include "lib/option/Option.H"
#include "lib/option/Infra.H"
#include "lib/math/Constant.H"
#include "lib/util/date.h"
#include "lib/util/Time.H"


int main()
{
   
    std::cout << '\n' ;
    const uint32_t sim_length {500};
    const uint32_t T { 12 };
    const uint32_t min_exp {6}; 
    const util::time::Date d0 { date::year{2022}/date::month{2}/1 };
    const util::time::Date df { util::time::next_day(d0, sim_length, true) };

    // set expiry time, and thus start/end DateTimes
    const util::time::Time expiry_time = std::chrono::minutes{0} + std::chrono::hours{15};
    const util::time::DateTime t0 { util::time::combine(d0, expiry_time) };
    const util::time::DateTime tf { util::time::combine(df, expiry_time) };

    // basic calendar accounting
    auto&& expiries = option::infra::compute_expiries(d0.year(), d0.month(), expiry_time, T);
    option::infra::UnderlyingCalendar underlyingCalendar{t0, tf}; 
    option::infra::OptionCalendar optionCalendar{underlyingCalendar, expiries, min_exp};
    [[likely]] assert( underlyingCalendar.size() == optionCalendar.size() );

    // storage for underlying prices.  should i do anything richer here?
    // initialise 
    std::vector<double> underlyingPrice(sim_length);
    constexpr double s0 { 4000.0 };
    underlyingPrice[0] = s0;

    SimulationWriter writer("ofn.csv", "ufn.csv");

    std::cout << " Simulation Setup" << std::endl;
    std::cout << " -- Initial Snap: " << util::time::to_string(t0) << std::endl;
    std::cout << " -- Final Snap: " << util::time::to_string(tf) << std::endl;
    std::cout << " -- Number of days: " << underlyingCalendar.size() << std::endl;
    std::cout << " -- Number of expiries: " << expiries.size() << std::endl;
    
    // Initialize our options grid.   
    // The OptionGrid will preallocate space for us to use during the simulation
    uint32_t nStrikes {50};    
    constexpr double optionNoiseMult { 0.0 };
    constexpr double rate { 0.02 };
    option::infra::OptionGrid grid{ expiries.size(), nStrikes, rate };

    // Construct volatility info - atm and the gatheral function
    constexpr double v0 {0.10 };
    constexpr double v1 {0.30 };
    option::vol::ATMVol atmVol {v0, v1};

    constexpr double eta { 0.25 };
    constexpr double rho { -0.50 };
    constexpr double volFloor { 0.05 };
    option::vol::Gatheral gatheral(atmVol, eta, rho);

    // generator for simulations
    std::random_device rd {};
    std::mt19937_64 gen { rd() };
    std::normal_distribution<double> normdist {0.0, 1.0};

    // Begin simulation

    for (size_t i=0; i<underlyingCalendar.size()-1; ++i)
    {
        // in this simulation we are at point i=t, ie
        // s_{t+1}|s_t is associated with s[i+1]|s[i]
        // so first assign everything at s[i], then attempt to 
        // simulation s[i+1]
        const double s0 { underlyingPrice[i] };
  
        
        // load current simulation times, if they are needed
        auto [currTime, dt0] = underlyingCalendar(i);
        
        writer.write(currTime, s0);
        // here we update strip level information for the option::infra::Strip
        // objects in the OptionGrid.  this includes tte and atmVol.
        
        // the OptionCalendar has precomputed all ttes for the whole simulation
        // ie ttes for every expiry on every day for the whole sim.  the
        // OptionGrid::updateCalendar method just reads this information on to
        // the grid at day i in the simulation, assigning the right tte for each
        // strip
        grid.updateCalendar(optionCalendar, i);

        // OptionGrid::updateATMVol call assigns the correct ATM vol for each strip,
        // given its now update tte information
        grid.updateATMVol(atmVol);  // should be a function of i too?

        // OptionGrid::updateStrike call evaluates both strikes K and LATFM x 
        // across the grid.  this isnt needed, unless you want to do options too.
        grid.updateStrike(s0, rate, i);

        // generate a new spot price
        // use x=0 point on the curve at front month
        const double x0 { 0.0 };
        const double t0 { grid(0).tte };
        const double k0 { option::vol::atfm_k(s0, rate, t0, x0) };

        // use vol function to get lvol(s,t). then interp it backwards
        // do something here if lvol < volFloor I guess
        const double lvol0 { gatheral.lvol(s0, k0, rate, t0) };
        const double lvol { std::sqrt( volFloor*volFloor + t0*lvol0*lvol0 ) };
        
        const double mean0 { s0 + rate*s0*dt0 };
        const double vol0 { lvol * s0 * std::sqrt(dt0) };
        const double s1 { mean0 + vol0*normdist(gen) };
        underlyingPrice[i+1] = s1;

        // and price the options
        option::bs::priceBSGrid(grid, gatheral);
        if (optionNoiseMult>0.0)
        {
            for (size_t i=0; i<grid.size().first; ++i)
            {
                option::infra::Strip& strip = grid(i);
                for (auto& c : strip.c)
                {
                    c += optionNoiseMult*std::sqrt(c)*normdist(gen);
                }
            }
        }

        writer.write(currTime, grid);
        

        // at this point we need to update vols across each strip.  this step
        // is a particle level calculation, since it dependant on the parameter
        // choice in the implied curve.    
        // if we were only interested in underlying simulations (and not options)
        // we would only need the front month vol curve.
        // grid.updateVol(gatheral, rate, i);  this is needed

        // in this model, we extrapolate the vol curve at T_0 BACK
        // to current time t<T_0.  Which vol point to extrapolate back
        // from is open to discussion, but for now I take the ATF zero
        // moneyness vol x=0.  this corresponds to 
        //
        // here i would LOAD options, and then determine ATM vol
        // in a historical simulation, i first need to assign the 
        // ATM vol the generate option prices
        

    }

    
}

