#!/bin/bash
declare -a headers=(cmath numbers chrono iomanip memory string iostream algorithm iterator streambuf bit)
for h in "${headers[@]}"
do 
    /opt/homebrew/bin/g++-11 -std=c++20 -fmodules-ts -xc++-system-header $h 
done