
#include "lib/util/Main.H"

namespace util
{
    namespace main
    {

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

        [[nodiscard]] std::unordered_map<std::string, std::string> read_cmd(int argc, char **argv)
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
    }
}