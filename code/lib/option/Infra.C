
#include "lib/option/Infra.H"
namespace option
{
    namespace infra
    {

        void roll_expiries(std::vector<util::time::DateTime> &expiries)
        {
            const size_t n = expiries.size();

            // compute next expiry
            const util::time::DateTime tf = expiries.back();
            const util::time::Date df = util::time::Date{std::chrono::floor<std::chrono::days>(tf)};
            const util::time::Time exp_time = util::time::get_time(tf);
            const util::time::Date ndf = util::time::last_business_day(util::time::next_year_month(df.year(), df.month()));

            // roll forward
            for (size_t i = 0; i < n - 1; ++i)
            {
                expiries[i] = expiries[i + 1];
            }

            // place at the end
            expiries.back() = util::time::combine(ndf, exp_time);
            return;
        }

        std::vector<util::time::DateTime> compute_expiries(date::year y, date::month m, util::time::Time t, uint32_t e)
        {
            // construct e expiries from (y, m)
            std::vector<util::time::DateTime> expiries(e);

            // initialise a day to march forward with
            util::time::Date d0 {util::time::last_business_day(y, m)};

            for (size_t i = 0; i < e; ++i)
            {
                expiries[i] = util::time::combine(d0, t);
                d0 = util::time::last_business_day(util::time::next_year_month(d0));
            }

            return expiries;
        }

        UnderlyingCalendar::UnderlyingCalendar(util::time::DateTime t0, util::time::DateTime tf) : _t0(t0), _tf(tf)
        {
            // _simDays are inclusive of BOTH t0 and tf
            _simDays = util::time::days_between(t0, tf, true);
            _simDates.resize(_simDays);
            _simTimeIncr.resize(_simDays);

            // first index is the initial condition
            _simDates[0] = t0;
            _simTimeIncr[0] = 0.0;
            t0 = util::time::next_day(t0, 1, true);

            // do the increment manually to have non-standard days per year etc
            const double hrs_per_year { util::time::days_per_year * util::time::mins_per_hour * util::time::hrs_per_day };
            const double frac{1.0 / hrs_per_year };

            for (size_t i = 1; i < _simDays; ++i)
            {
                // set time here
                _simDates[i] = t0;

                // set time incr
                const double dt = (_simDates[i] - _simDates[i - 1]).count() * frac;
                _simTimeIncr[i] = dt;
                t0 = util::time::next_day(t0, 1, true);
            }
        }

        OptionCalendar::OptionCalendar(const option::infra::UnderlyingCalendar &calendar, const std::vector<util::time::DateTime> &expiries, uint32_t min_tte)
        {
            // UnderlyingCalendar determines the number of simulations days
            // expiry vector determines the number of expiries at each simulation day
            _simDays = calendar.size();
            _simExpiries = expiries.size();
            _optionTerms.resize(_simDays);

            for (size_t i = 0; i < _simDays; ++i)
            {
                _optionTerms[i].resize(_simExpiries);
            }

            // make a copy of expiries, so we can implement roll logic
            std::vector<util::time::DateTime> copyExpiries{expiries};

            // fill the option calendar
            //const double hrs_per_year { util::time::days_per_year * util::time::mins_per_hour * util::time::hrs_per_day };
            const double frac{1.0 / util::time::days_per_year };
            for (size_t i = 0; i < calendar.size(); ++i)
            {
                // extract the current simulation date.  roll if need be
                const util::time::DateTime simDate = calendar(i).first;
                std::string currstr{util::time::to_string(simDate)};
                const uint64_t days = util::time::days_between(simDate, copyExpiries.front(), true);
                if (days < min_tte)
                {
                    option::infra::roll_expiries(copyExpiries);
                }

                auto &thisTerm = _optionTerms[i];
                for (size_t j = 0; j < copyExpiries.size(); ++j)
                {
                    const util::time::DateTime e_j {copyExpiries[j]};
                    const double tte_j { frac* util::time::days_between(simDate, e_j, true) };
                    thisTerm[j] = std::make_pair(e_j, tte_j);
                }
            }
        }

        Strip::Strip(uint64_t nStrikes, double rate) : nStrikes(nStrikes), rate (rate){};
        
        void Strip::resize(size_t n)
        {
            assert(n>0);
            x.resize(n);
            k.resize(n);
            c.resize(n);
            sns.resize(n); 
            lv.resize(n);
            iv.resize(n);       
        }

        void Strip::setSNS(void)
        {
            assert(sns.size()>0);
            const double ds { (_maxSNS - _minSNS)/(nStrikes - 1.0) };
            for (size_t i=0; i<sns.size(); ++i) { sns[i] = _minSNS + i*ds; }
        }

        OptionGrid::OptionGrid(size_t nT, size_t nK, double rate) : _nTerms(nT), _nStrikes(nK)
        {
            _optionGrid.reserve(_nTerms);
            for (size_t i=0; i<_nTerms; ++i) { _optionGrid.emplace_back ( Strip(_nStrikes, rate) ); }
            
            for (size_t i = 0; i < _nTerms; ++i)
            {
                _optionGrid[i].resize(_nStrikes);
                _optionGrid[i].setSNS();   // don't really like this here...
            }
        }

        const Strip& OptionGrid::operator()(size_t i) const { return _optionGrid[i]; };
        Strip& OptionGrid::operator()(size_t i) { return _optionGrid[i]; }
        
        SimulationOptionGrid::SimulationOptionGrid(size_t nT, size_t nK, double r) : OptionGrid(nT,nK,r) {};
        void SimulationOptionGrid::updateCalendar(const OptionCalendar &calendar, size_t i)
        {
            assert(i < calendar.size());

            // OptionCalendar has precomputed ttes etc across the grid
            // use these expiries to update tte for each strip
            auto &&expiries = calendar(i);
            for (size_t j=0; j<expiries.size(); ++j)
            { 
                auto&& tStruct = expiries[j];
                _optionGrid[j].expiry = tStruct.first;
                _optionGrid[j].tte = tStruct.second; 
            }

        }

        void SimulationOptionGrid::updateStrike(double currUnderlying, double rate, size_t idx)
        {
            // use curr spot to compute x = log(k/F) = log(k/S) - r*tau across each strip
            for (auto&& strip : _optionGrid)
            {
                strip.spot = currUnderlying;
                const double fwd { currUnderlying * std::exp(rate*strip.tte) };
                const double fwdVol { strip.atmImpliedVol * std::sqrt(strip.tte) };
                // fill in strikes, and log atfm.  strikes are linear in sns
                for (size_t i=0; i<strip.nStrikes; ++i)
                {
                    strip.k[i] = fwd*std::exp(strip.sns[i] * fwdVol);  
                    strip.x[i] = strip.sns[i] * fwdVol;   // do we need this?
                }
            }
        }

        EstimationOptionGrid::EstimationOptionGrid(size_t nT, size_t nK, double r) : OptionGrid(nT,nK,r) {};
    }
}