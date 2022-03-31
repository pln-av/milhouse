#include <iostream>

import Math;

int main()
{
    std::cout << "Test Option module."  << std::endl;
    
    for (int i=0; i<5; ++i) 
    {
        std::cout << i << ", " << math::integration::func1(static_cast<double>(i)) 
                       << ", " << math::integration::func2(static_cast<double>(i)) 
                       << ", " << math::sf::normcdf(static_cast<float>(i)) 
                       << ", " << math::sf::normcdf(static_cast<double>(i)) << '\n';
    }
}