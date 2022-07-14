#pragma once
#include <vector>
#include <iostream>
#include <limits>
#include <iostream>

#include "lib/util/Time.H"

namespace option 
{
    namespace infra
    {
        using Expiry = std::pair<util::time::DateTime, double>;

        // some utilities for dealing with expiries
        void roll_expiries(std::vector<util::time::DateTime> &expiries);
        std::vector<util::time::DateTime> compute_expiries(date::year y, date::month m, util::time::Time t, uint32_t e);

        class UnderlyingCalendar
        {
        public:
            UnderlyingCalendar(util::time::DateTime t0, util::time::DateTime tf); 

        public:
            size_t size(void) const { return _simDates.size(); };
            std::pair<util::time::DateTime, double> operator()(size_t idx) const { return std::make_pair(_simDates[idx], _simTimeIncr[idx]); }
        private:
            const util::time::DateTime _t0;
            const util::time::DateTime _tf;
            uint64_t _simDays;
            std::vector<util::time::DateTime> _simDates;
            std::vector<double> _simTimeIncr;
        };

        class OptionCalendar
        {
            //using Expiry = std::pair<util::time::DateTime, double>;
        public:
            OptionCalendar(const UnderlyingCalendar& calendar, const std::vector<util::time::DateTime>& expiries, uint32_t min_tte);            
            std::vector<std::vector<Expiry>> _optionTerms;
            Expiry operator()(size_t i, size_t j) const { return _optionTerms[i][j]; }
            const std::vector<Expiry>& operator()(size_t i) const { return _optionTerms[i]; }
            size_t size(void) const { return _optionTerms.size(); }
        private:
            uint64_t _simDays;
            uint64_t _simExpiries;
        };

        struct Strip
        {
            // utility class for holding required data.
      
            Strip(uint64_t nStrikes=0, double rate=0.0);
            void resize(size_t n);
            size_t size(void) const { return nStrikes; }
            std::vector<double> x;   // log moneyness
            std::vector<double> k;   // dollar strike
            std::vector<double> c;   // call prices
            std::vector<double> iv;  // implied vol
            std::vector<double> lv;  // local vol
            std::vector<double> sns; // our strikes in sns space. maybe wasted here...

            uint64_t nStrikes;

            util::time::DateTime expiry { util::time::BaseTime };
            double atmImpliedVol{ std::numeric_limits<double>::quiet_NaN() };
            double tte { std::numeric_limits<double>::quiet_NaN() } ;
            double spot { std::numeric_limits<double>::quiet_NaN() };
            double rate { std::numeric_limits<double>::quiet_NaN() };

            const double _minSNS { -3.0 };
            const double _maxSNS { 3.0 };
            
            void setSNS(void);
          
        };

        // basic class holding information regarding the {K x T} option grid used
        // in the fitter.  ideally this will do all shared computations and be able
        // to provide raw data to the fitter in an efficient way
        class OptionGrid
        {
        public:
            OptionGrid(size_t nTerms, size_t nStrikes, double rate);
            size_t _nTerms {0};
            size_t _nStrikes {0};

            std::pair<size_t, size_t> size(void) const { return std::make_pair(_nTerms, _nStrikes); };
            const Strip& operator()(size_t i) const;
            Strip& operator()(size_t i);
        protected:
            std::vector<Strip> _optionGrid;
            
        };

        class SimulationOptionGrid : public OptionGrid 
        {
        public:
            SimulationOptionGrid(size_t nTerms, size_t nStrikes, double rate);
            void updateCalendar(const OptionCalendar& calendar, size_t i);
            void updateStrike(double currUnderlying, double rate, size_t idx);
            template <typename ATMVol>
            void updateATMVol(ATMVol vf)
            {
                // use vol function vf to update each strip's ATMVol
                for (size_t j=0; j<_nTerms; ++j) 
                { 
                    _optionGrid[j].atmImpliedVol = vf.atm(_optionGrid[j].tte);
                }
            }
        };

        class EstimationOptionGrid : public OptionGrid 
        {
        public:
            EstimationOptionGrid(size_t nTerms, size_t nStrikes, double rate=0.0);
        };
    }  // namespace infra
} // namespace option


