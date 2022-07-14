#include <memory>
#include <unordered_map>
#include <fstream>
#include <random>
#include <cassert>
#include <filesystem>

#include "lib/option/Option.H"
#include "lib/option/Infra.H"
#include "lib/math/Constant.H"
#include "lib/util/date.h"
#include "lib/util/Time.H"
#include "lib/util/Util.H"


namespace util
{
    namespace sim
    {
        // not so sure where to put these...
        uint64_t countFields(const std::string &s, char ch = ',')
        {
            return 1 + std::count(s.begin(), s.end(), ch);
        }

        void split_line(const std::string &str, std::vector<std::string> &v, char ch = ',')
        {
            assert(countFields(str) == v.size());
            std::string l;
            std::stringstream ss(str);
            size_t idx{0};
            while (std::getline(ss, l, ',')){ v[idx] = l; ++idx; }
        }

        std::vector<std::string> split_line(const std::string &str, char ch = ',')
        {
            std::vector<std::string> tmp(countFields(str, ch));
            split_line(str, tmp, ch);
            return tmp;
        }

        class SimulationWriter
        {
        public:
            SimulationWriter(const std::string &ofn, const std::string &ufn); 
            ~SimulationWriter();
            void write(const util::time::DateTime &dt, double s);
            void write(const util::time::DateTime &dt, const option::infra::OptionGrid &grid);

        private:
            const std::string _underFileName;
            const std::string _optionFileName;
            std::fstream _of;
            std::fstream _uf;
        };

        class SimulationReader
        {
        public:
            enum HEADER { Time, Expiry, TTE, Strike, Price, X, Rate, IVol, LVol };
            size_t _fields {9};
        // object to read from data files.  
        // reads day by day, and can pass data for estimation
            SimulationReader(const std::string& ofn, const std::string& ufn, size_t nT, size_t nK);
            ~SimulationReader();
      
            // read the header, and return what the first date in the data set is
            util::time::Date initialise(void);

            // read the data and populate the EstimationOptionGrid
            util::time::Date read(util::time::Date, option::infra::EstimationOptionGrid& grid);

        private:

            // files and their names
            const std::string _optionFileName;
            const std::string _underFileName;
            
            std::ifstream _of;
            std::ifstream _uf;

            // number of strikes and expiries per day, and a grid for writing
            const size_t _nT;
            const size_t _nK;

            // two vectors for savings results, to prevent too many allocations
            std::vector<std::string> _header;
            std::vector<std::string>  _line;
        };

        class SimulationEngine
        {
        public:
            
            SimulationEngine(const option::infra::UnderlyingCalendar &uCal,
                             const option::infra::OptionCalendar &oCal,
                             option::infra::SimulationOptionGrid &grid,
                             util::sim::SimulationWriter &writer,
                             const std::unordered_map<std::string, std::string>& args)
                : _uCal(uCal), _oCal(oCal), _grid(grid), _writer(writer), _args(args), _simSize(uCal.size())
            {
                // SimulationEngine organises simulation data for the simulation.
                // It contains a number of objects, so construction is a bit involved...
                // In a next iteration we can template on vol types in particular
                std::cout << " -- Simulation arguments: " << std::endl;
                _printArgs(_args);
                _uPrice.assign(_uCal.size(), 0.0);
                _rate = std::stod(args.at("rate"));
            }

            template <typename AVol, typename Vol>
            void initialise(AVol &&avol, Vol &&vol, double s0)
            {
                // this is here because in the future the options will be initialised
                // from market data.
                _update(std::forward<AVol>(avol), std::forward<Vol>(vol), s0, 0);
                _initialised = true;
            }

            template <typename AVol, typename Vol>
            void _update(AVol &&avol, Vol &&vol, double s0, size_t idx)
            {
                assert(idx < _uCal.size());
                _uPrice[idx] = s0;

                _grid.updateCalendar(_oCal, idx);
                _grid.updateATMVol(avol);
                _grid.updateStrike(s0, _rate, idx);

                option::bs::priceBSGrid(_grid, std::forward<Vol>(vol));
                return;
            }
            template <typename AVol, typename Vol>
            void simulate(AVol &&avol, Vol &&vol, double noiseMult)
            {
                // verbose simulation
                // idx=0 is the initial condition
                if (!_initialised)
                {
                    std::cout << "simulate() called, but engine is not initialised. " << std::endl;
                    std::exit(1);
                }

                // write out initial data

                auto [currTime, _] = _uCal(0);
                _writer.write(currTime, _uPrice[0]);
                _writer.write(currTime, _grid);

                // NB my time convention.  dt[i] = t[i]-t[i-1].  so when generating
                // s[i], we are using dt[i]
                for (size_t idx = 1; idx < _simSize; ++idx)
                {
                    const size_t prevIdx{idx - 1};
                    const size_t currIdx{idx};

                    // generate data for currIdx, using prevIdx
                    // currTime = t[i]
                    // dt = t[i] - t[i-1]
                    auto [currTime, dt] = _uCal(currIdx);

                    // data at prevIdx
                    const double x0{0.0};
                    const double t0{_grid(0).tte};
                    const double s0{_uPrice[prevIdx]};
                    const double k0{option::vol::atfm_k(s0, _rate, t0, x0)};

                    // compute vols at prevIdx (ie lvolFM).  then interp back in tte to lvol0
                    const double lvolFM{vol.lvol(s0, k0, _rate, t0)};
                    const double lvol0{std::sqrt(_volFloor * _volFloor + t0 * lvolFM * lvolFM)};

                    const double mean0{s0 + _rate * s0 * dt};
                    const double vol0{lvol0 * s0 * std::sqrt(dt)};
                    const double s1{mean0 + vol0 * _normdist(_gen)};

                    // do updates, then print
                    _update(avol, vol, s1, currIdx);
                    _writer.write(currTime, s1);
                    _writer.write(currTime, _grid);
                }
            }

        private:
            void _printArgs(const std::unordered_map<std::string, std::string> &s)
            {
                for (auto &&kv : s)
                {
                    std::cout << "    -- arg: " << kv.first << " || val: " << kv.second << "\n";
                }
            }

        private:
            // private data
            const option::infra::UnderlyingCalendar &_uCal;
            const option::infra::OptionCalendar &_oCal;
            option::infra::SimulationOptionGrid &_grid;
            SimulationWriter &_writer;
            const std::unordered_map<std::string, std::string> &_args;
            double _rate{0.0};
            std::vector<double> _uPrice;

            // internal data for tracking simulation
            const size_t _simSize;
            bool _initialised{false};

            // random engines.  probably replace this
            std::random_device _rd{};
            std::mt19937_64 _gen{_rd()};
            std::normal_distribution<double> _normdist{0.0, 1.0};

            const double _volFloor{0.05};
        };
    }
}