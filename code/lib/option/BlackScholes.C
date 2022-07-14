#include "lib/option/BlackScholes.H"
namespace option
{
    namespace bs 
    {
        void priceBSGrid(option::infra::SimulationOptionGrid& grid, const option::vol::Gatheral& gatheral)
        {
            auto [nTerms, nStrikes] = grid.size();
            for (size_t i=0; i<nTerms; ++i)
            {
                option::infra::Strip& strip = grid(i);
                const double tte { strip.tte };
                const double s { strip.spot };
                const double rate { strip.rate };
                

                // compute lvol at each strike, then price
                for (size_t j=0; j<strip.size(); ++j)
                {
                    const double kj = strip.k[j];
                    const double xj = strip.x[j];
                    const double ivj = gatheral.v_x(xj, tte);
                    const double lvj = gatheral.lvol(s, kj, rate, tte);
                    strip.c[j] = option::bs::bs_call(s, kj, rate, tte, ivj);
                    strip.iv[j] = ivj;
                    strip.lv[j] = lvj;
                }
            }
        }  
    }  // namespace bs
} // namespace option
