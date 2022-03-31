
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
            std::vector<util::time::DateTime> expiries;
            util::time::Date e1 = util::time::last_business_day(y, m);
            expiries.push_back(util::time::combine(e1, t));
            uint32_t idx{0};
            while (idx < e)
            {
                const util::time::Date e2 = util::time::last_business_day(util::time::next_year_month(e1));
                expiries.push_back(util::time::combine(e2, t));
                e1 = e2;
                ++idx;
            }
            return expiries;
        }
     
     }
 }