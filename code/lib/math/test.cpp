#include <iostream>

#include "lib/math/Math.H"

int main()
{
    std::cout << "Test Option module."  << std::endl;
    
    for (int i=0; i<5; ++i) 
    {
        std::cout << i << ", " << math::integration::func(double(i)) 
                       << ", " << math::sf::normcdf(double(i)) << '\n';
    }
}