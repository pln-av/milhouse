#include "lib/option/BlackScholes.H"
namespace option
{
    namespace bs 
    {
        void priceBSGrid(option::infra::OptionGrid& grid, const option::vol::Gatheral& gatheral)
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
                    strip.c[j] = option::bs::bs_call(s, strip.k[j], rate, tte, gatheral.v_x(strip.x[j], tte));
                }
            }
        }  
    }  // namespace bs
} // namespace option
