#include <iostream>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <cassert>

#include "lib/util/Sim.H"

using ArgMap = std::unordered_map<std::string, std::string>;
const std::vector<std::string> reqArgs
{   
    "startDate", "expiries", "dates", "rate", 
    "rho", "eta", "strikes", "optionOutput", 
    "underlyingOutput", "s0", "optionNoiseMult"
};

std::pair<util::time::DateTime, util::time::DateTime> parse_times(const ArgMap &args, util::time::Time expiryTime)
{
    // first construct the required calendards.
    const util::time::Date d0 { util::time::parse_date(args.at("startDate")) };
    const util::time::Date df { util::time::next_day(d0, std::stol(args.at("dates")), true) };
    const util::time::DateTime t0 {util::time::combine(d0, expiryTime) };
    const util::time::DateTime tf {util::time::combine(df, expiryTime) };
    return std::make_pair(t0, tf);
}

bool valid_arg_name(const std::string &s)
{
    if (s.size() < 3)
    {
        std::cout << "Arg " + s + " must have size greater than 2." << std::endl;
        return false;
    }
    if (((s[0] != '-') || (s[1] != '-')))
    {
        std::cout << "Arg name must start with --" << std::endl;
        return false;
    }
    return true;
}

[[nodiscard]] ArgMap read_cmd(int argc, char **argv)
{
    // some conventions
    // all args are named and use double dash --
    // no empty args are permitted
    std::unordered_map<std::string, std::string> out;
    if (argc == 0)
    {
        std::cout << "No arguments supplied. This is an error." << std::endl;
        return out;
    }

    std::vector<std::string> args(argv + 1, argv + argc);
    if (args.size() % 2 != 0)
    {
        std::cout << "Each arg must be named.  As such an even number or args is required." << std::endl;
        return out;
    }
    
    for (size_t i = 0; i < args.size(); i += 2)
    {
        const std::string s0{args[i]};
        const std::string s1{args[i + 1]};
        if (!valid_arg_name(s0))
        {
            std::cout << "Invalid argument name." << std::endl;
            return out;
        }
        out[std::string{s0.begin() + 2, s0.end()}] = s1;
    }
    return out;
}

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
    std::cout << "Constructing SimulationWriter." << std::endl;
    util::sim::SimulationWriter writer { args.at("optionOutput"), args.at("underlyingOutput") };

    // construct the {KxT} option grid 
    std::cout << "Constructing SimulationOptionGrid." << std::endl;
    option::infra::SimulationOptionGrid grid { expiries.size(), std::stoul(args.at("strikes")), std::stod(args.at("rate")) };
   
    // construct the SimulationEngine, which takes the calendars above
    // creates option grids, and begins the simulation
    std::cout << "Constructing SimulationEngine." << std::endl;
    util::sim::SimulationEngine engine { underlyingCalendar, optionCalendar, grid, writer, args };

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

    std::cout << "Initialising Engine." << std::endl;
    engine.initialise( atmVol, gatheral, std::stod(args.at("s0")) );
    std::cout << "Beginning simulation." << std::endl;
    engine.simulate(atmVol, gatheral, optionNoiseMult);
  
}