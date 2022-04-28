#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <cassert>

#include "sim.H"




int main(int argc, char** argv)
{
    std::cout << "Gatheral Simulation." << std::endl;
    auto&& args = read_cmd(argc, argv);
    bool badArgs { false };
    for (auto&& req : reqArgs) 
    { 
        if (args.find(req)==args.cend()) 
        { 
            std::cout << "Argument " << req << " is required but not found." << std::endl; 
            badArgs = true;
        }
    }
    if (badArgs ) { std::exit(1); }

    // read in times, and construct calendards
    const util::time::Time expiryTime = std::chrono::minutes{0} + std::chrono::hours{15};
    const auto [t0, tf] = parse_times(args, expiryTime);
    std::cout << " -- startDate: " << util::time::to_string(t0) << std::endl;
    std::cout << " -- endDate: " << util::time::to_string(tf) << std::endl;

    // basic calendars.  note the option calendar precomputes all expiry
    // info for all simulation dates.  this is read onto the option grid, but 
    // doesn't need to be calculated again.
    const util::time::Date d0 { util::time::get_date(t0) };
    const uint64_t nExpiries { std::stoul(args.at("expiries")) };
    const std::vector<util::time::DateTime> expiries = option::infra::compute_expiries(d0.year(), d0.month(), expiryTime, nExpiries);
    option::infra::UnderlyingCalendar underlyingCalendar{t0, tf}; 
    option::infra::OptionCalendar optionCalendar{ underlyingCalendar, expiries, 6};
    assert( underlyingCalendar.size() == optionCalendar.size() );

    // utility for writing the csv files we need
    util::sim::SimulationWriter writer { args.at("optionOutput"), args.at("underlyingOutput") };

    // construct the {KxT} option grid 
    option::infra::OptionGrid grid{ expiries.size(), std::stoul(args.at("strikes")), std::stod(args.at("rate")) };
   
    // construct the SimulationEngine, which takes the calendars above
    // creates option grids, and begins the simulation
    util::sim::SimulationEngine engine{ underlyingCalendar, optionCalendar, grid, writer, args };

    // putting this here for now, I'm not sure what to do with it.
    const double optionNoiseMult { std::stod(args.at("optionNoiseMult")) };

    // two vol functions.
    // first is ATMVol(tte), second is vol curve(K)
    constexpr double v0 {0.10 };
    constexpr double v1 {0.30 };
    option::vol::ATMVol atmVol {v0, v1};
 
    const double eta { std::stod(args.at("eta")) };
    const double rho { std::stod(args.at("rho")) };
    option::vol::Gatheral gatheral(atmVol, eta, rho);

    engine.initialise( atmVol, gatheral, std::stod(args.at("s0")) );
    engine.simulate(atmVol, gatheral, optionNoiseMult);
  
}