#include <iostream>


import Option;

int main()
{
    std::cout << "Test Option module."  << std::endl;
    
    for (int i=0; i<5; ++i) 
    {
        std::cout << i << ", " << option::bs::func1(static_cast<double>(i)) 
                       << ", " << option::bs::func2(static_cast<double>(i)) 
                       << ", " << option::vc::func1(static_cast<float>(i)) 
                       << ", " << option::vc::func2(static_cast<double>(i)) 
                       << ", " << option::util::func1(2.0d) 
                       << ", " << option::util::func2(2.0f) << '\n';
    }
}

