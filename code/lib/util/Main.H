#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

namespace util
{
    namespace main
    {
        
        bool valid_arg_name(const std::string &s);

        [[nodiscard]] std::unordered_map<std::string, std::string> read_cmd(int argc, char **argv);

    }
}