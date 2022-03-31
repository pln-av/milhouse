#!/bin/bash
declare -a headers=(cmath numbers vector locale)
for h in "${headers[@]}"
do 
    /opt/homebrew/bin/g++-11 -std=c++20 -fmodules-ts -xc++-system-header $h 
done