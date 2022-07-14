#include "lib/util/Sim.H"

namespace util
{
    namespace sim
    {
        SimulationWriter::SimulationWriter(const std::string &ofn, const std::string &ufn) : _underFileName(ufn), _optionFileName(ofn)
        {
            // there are issues with fstream opening a non-existant file.  this is
            // a workaround.
            _uf.open(_underFileName, std::ios_base::out);
            _uf.close(); // now this exists.
            _uf.open(_underFileName);
            _uf << "Time,Spot\n";

            _of.open(_optionFileName, std::ios_base::out);
            _of.close();
            _of.open(_optionFileName);
            _of << "Time,Expiry,TTE,Strike,Price,X,Rate,IVol,LVol\n";
        }
        
        SimulationWriter::~SimulationWriter()
        {
            _of.close();
            _uf.close();
        }

        void SimulationWriter::write(const util::time::DateTime &dt, double s)
        {
            _uf << util::time::to_string(dt) << "," << std::to_string(s) << std::endl;
        }

        void SimulationWriter::write(const util::time::DateTime &dt, const option::infra::OptionGrid &grid)
        {
            auto [nTerms, nStrikes] = grid.size();
            const std::string t{util::time::to_string(dt)};
            for (size_t i = 0; i < nTerms; ++i)
            {
                const option::infra::Strip &strip = grid(i);
                const std::string expiry{util::time::to_string(strip.expiry)};
                const std::string tte{std::to_string(strip.tte)};
                const std::string rate{std::to_string(strip.rate)};
                for (size_t j = 0; j < nStrikes; ++j)
                {
                    _of << t << "," << expiry
                        << "," << tte
                        << "," << std::to_string(strip.k[j])
                        << "," << std::to_string(strip.c[j])
                        << "," << std::to_string(strip.x[j])
                        << "," << rate
                        << "," << std::to_string(strip.iv[j])
                        << "," << std::to_string(strip.lv[j])
                        << std::endl;
                }
            }
        }

        SimulationReader::SimulationReader(const std::string& ofn, const std::string& ufn, size_t nT, size_t nK) 
        : _optionFileName(ofn), _underFileName(ufn), _nT(nT), _nK(nK)
        {
            assert(std::filesystem::exists( std::filesystem::path(_optionFileName)) );
            assert(std::filesystem::exists( std::filesystem::path(_underFileName)) );
        }

        SimulationReader::~SimulationReader() { }

        util::time::Date SimulationReader::initialise(void)
        {
            // to initialise we:
            // 1. read the header
            // 2. get first date of data set
         
            std::string l;
            _of.open(_optionFileName, std::ios::in);
            if (_of.is_open())
            {
                std::getline(_of, l);
            }
            assert(countFields(l) == _fields);
            _header.resize(_fields);
            _line.resize(_fields);

            // read in the header
            split_line(l, _header);

            // detect the first date
            if (_of.is_open())
            {
                std::getline(_of, l);
            }
            assert(countFields(l) == _fields);
            split_line(l, _line);
            util::time::Date retDate { util::time::parse_date(_line[HEADER::Time])};

            // this is a little ugly, but for the loops etc to work close it up
            // and reopen, ready for the read() functions first call (ie past the 
            // header line)
            _of.close();
            _of.open(_optionFileName, std::ios::in);
            std::getline(_of, l);
            return retDate;
        }

        util::time::Date SimulationReader::read(util::time::Date date, option::infra::EstimationOptionGrid& grid)
        {
        
            // you must have been initialised for this to be true
            assert(_header.size() == _line.size());

            // you will read from file WHILE file.date == date
            // when this is not true stop and return the new date

            // file should be ready and open to read.  the current date
            // should EQUAL date.  assert both of these
            assert (_of.is_open());            

            // these will come from command args.  counters for positional reading
            size_t idx {0};
            std::string l;
            util::time::Date currDate;

            // NB the use of the streampos means the file is ready for the NEXT day
            // if we have K days in N lines, this means we only read K lines twice.
            std::streampos prevPos = _of.tellg();
            while ( std::getline(_of, l))
            {
                // now _line contains this line
                split_line(l, _line);

                // NB im making assumptions about the ordering in each day in the data set
                // assert that here
                currDate =  util::time::parse_date(_line[HEADER::Time]);
                if (currDate!=date)
                {
                    // found the next day. return!
                    _of.seekg(prevPos);  // ie read this line again!
                    return currDate;
                }
                
                // location inside data structure
                const size_t kLoc = idx % _nK;
                const size_t tLoc = idx / _nK;

                option::infra::Strip& strip = grid(tLoc);
                if (tLoc==0) 
                {
                    strip.tte = std::stod( _line[HEADER::TTE] );
                    strip.rate = std::stod( _line[HEADER::Rate]);
                }
                strip.k[kLoc]  = std::stod(_line[HEADER::Strike]);
                strip.iv[kLoc] = std::stod( _line[HEADER::IVol]);
                strip.lv[kLoc] = std::stod( _line[HEADER::LVol]);
                strip.c[kLoc]  = std::stod( _line[HEADER::Price]);
                strip.x[kLoc]  = std::stod( _line[HEADER::X]);
                

                // next row
                ++idx;
                prevPos = _of.tellg();
            }
            return currDate;
        }
    }
}