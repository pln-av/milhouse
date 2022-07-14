#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <ctime>

#include "lib/util/Time.H"
#include "lib/util/Sim.H"

void read_line(const std::string& s)
{
    std::stringstream ss(s);
    std::string w;
    size_t idx { 0 };
    while (std::getline(ss, w, ','))
    {
        std::cout << "word(" << idx << ")=" << w << std::endl;
        ++idx;
    }
}

uint64_t countFields(const std::string& s, char ch=',')
{ return std::count(s.begin(), s.end(), ch)+1; }

void split_line(const std::string& str, std::vector<std::string>& v, char ch=',')
{
    assert(countFields(str)==v.size());
    std::string l;
    std::stringstream ss(str);
    size_t idx {0};
    while (std::getline(ss, l, ',')) 
    {
        v[idx]=l;
        ++idx;
    }
}

std::vector<std::string> split_line(const std::string& str, char ch=',')
{
    std::vector<std::string> tmp( countFields(str, ch) );
    split_line(str, tmp, ch);
    return tmp;
}
/*
class SimulationReader
{
public:
    SimulationReader(const std::string& of) : _optionFileName(of) {};
    enum HEADER { Time, Expiry, TTE, Strike, Price, X, Rate, IVol, LVol };

private:
    const std::string _optionFileName;
    
    const size_t _fields { 9 };

    std::vector<std::string> _header;
    std::vector<std::string> _line;
public:
 
    // read the header line and detect the first date
    util::time::Date initialise(void)
    {

        std::ifstream of;
        std::string l;
        of.open(_optionFileName, std::ios::in);
        if (of.is_open()) { std::getline(of, l); }
        assert(countFields(l)==_fields);
        _header.resize(_fields);
        _line.resize(_fields);

        // read in the header
        split_line(l, _header);  

        // detect the first date
        if (of.is_open()) { std::getline(of, l); }
        assert(countFields(l)==_fields);
        split_line(l, _line);
        return util::time::parse_date(_line[HEADER::Time]);

    }

    void read(void)
    {
        
        util::time::Date prevDate = initialise();

        assert(_header.size() == _line.size());


        // this is a little ugly, but weve already read the first line.
        // close, and start again. were only one line in, ok?
        std::ifstream of;
        of.open(_optionFileName, std::ios::in);
        assert(of.is_open());
        std::string l;
        std::getline(of, l);  // skip the first line...
        

        // these will come from command args.  counters for positional reading
        //size_t ic {0};
        size_t nStrikes { 10 };
        size_t idx {0};
        while (std::getline(of, l) )
        {
            split_line(l, _line);

            // NB im making assumptions about the ordering in each day in the data set
            const util::time::Date currDate { util::time::parse_date(_line[HEADER::Time]) };
            assert (currDate>=prevDate);
            if (prevDate != currDate) { prevDate = currDate; idx=0;  }
            const double strike { std::stod(_line[HEADER::Strike]) };
            const double tte { std::stod(_line[HEADER::TTE]) };

            // location inside data structure
            const size_t kLoc = idx % nStrikes;
            const size_t tLoc = idx / nStrikes;
            std::cout << "Entry(" << kLoc << "," << tLoc << ")=" << currDate << "(" << strike << "," << tte << ")" << std::endl;
            ++idx;

        }

    }
    
};
*/
void print(option::infra::EstimationOptionGrid& grid)
{
    auto [nTerms, nStrikes] = grid.size();
    for (size_t it = 0; it<nTerms; ++it)
    {
        const option::infra::Strip& strip = grid(it);
        std::cout << " - tte: " << strip.tte << std::endl;
        for (size_t ik=0; ik<nStrikes; ++ik)
        {
            std::cout << " -- lv[" << it << "," << ik << "]=" << strip.lv[ik] << std::endl;
        }
    }
}
int main()
{

    std::cout << "Test SimulationReader" << std::endl;
    const std::string bin {"/Users/patricknoble/Documents/Projects/milhouse/code/bin"};
    const std::string of = bin + "/option.csv";
    const std::string uf = bin + "/underlying.csv";
    const size_t nK { 10 };
    const size_t nT { 3 };
    //const double rate { 0.02 };
    util::sim::SimulationReader reader {of, uf, nT, nK };
    option::infra::EstimationOptionGrid grid{ nT, nK };
    util::time::Date startDate = reader.initialise();
    util::time::Date curr { startDate };
    size_t idx {0};
    std::cout << "Begin data load at date: " << curr << std::endl;
    while ((idx<2) && (curr.ok()))
    {
        std::cout << "Reading data for date: " << curr << std::endl;
        curr = reader.read(curr, grid);
        print(grid);
        ++idx;
    }
    std::cout << "End date: " << curr << " is date: " << idx << std::endl;

    // lets check the contents of the loaded grid.
    /*
    std::cout << "Check date: " << curr << std::endl;
    
    */
    /*bool doRead { true };
    while (doRead)
    {
        std::cout << "Reading: " << curr << std::endl;
        const util::time::Date next = reader.read(curr);
        if (next==util::time::Date()) 
        {
            std::cout << "Breaking!" << std::endl;
            break;
        }
        curr = next;
    }
    */
    //std::cout << "Curr: " << curr << std::endl; 
    
    //util::sim::SimulationWriter writer("foo", "bar");
    /*
    SimulationReader reader {of};
    reader.read();

    std::string t { "20000103 15:00:00 UTC" };
    std::cout << "Target: " << t << std::endl;
    */

    // for simplicity, lets ignore the time and use the date instead
    //util::time::Date d = util::time::parse_date(t);
    //std::cout << "date: " << d << std::endl;
    /*
    std::cout << "Reading header: " << header(of) << std::endl;
    std::ifstream o;
    std::string line;
    o.open(of, std::ios::in);
    size_t idx { 0 };
    size_t maxIdx { 10 };
    if (o.is_open())
    {
        while ( (idx < maxIdx) && (std::getline( o, line)))
        {
            std::cout << "line: " << line << std::endl;
            read_line(line);
            ++idx;
        }
    }
    else{
        std::cout << "file " << of << " not open." << std::endl;
    }
    o.close();
    */
}