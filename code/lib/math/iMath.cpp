// primary module interface unit for the module Math
// i got this info from here:
// https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Modules.html
// to compile this i did
// math % g++ -std=c++20 -fmodules-ts -xc++-system-header numbers
// math % g++ -std=c++20 -fmodules-ts -xc++-system-header cmath 
// math % g++ -fmodules-ts -c -std=c++20 imath.cpp math.cpp
// math % g++ -fmodules-ts -c -std=c++20 test.cpp
// math % g++ -std=c++20 test.o imath.o math.o -o test
// or do this to get it in one go
// math % g++ -fmodules-ts -std=c++20 test.cpp imath.cpp math.cpp -o test

// the series of pages at 
// https://vector-of-bool.github.io/2019/03/10/modules-1.html
// are super helpful.  highly recommend


// start the global module fragment, which signals that we 
// are declaring a module, and we include some preprocessor fluff
module; 

// declare the Math module from here
export module Math;
export import MathIntegration;
export import MathSpecialFunction;

