CXX := /opt/homebrew/bin/g++-11
CXXFLAGS := -std=c++20 -Wall -Werror
SHELL=/bin/bash

SpecialFunction: SpecialFunction.H SpecialFunction.C
	$(CXX) $(CXXFLAGS) -c -fpic SpecialFunction.C -I${MILHOUSE}/code -o SpecialFunction.o 

Integration: Integration.H Integration.C
	$(CXX) $(CXXFLAGS) -c -fpic Integration.C -I${MILHOUSE}/code -o Integration.o 

libmath: Integration.o SpecialFunction.o
	$(CXX) $(CXXFLAGS) -I${MILHOUSE}/code -shared -o libmath.so SpecialFunction.o Integration.o 


# make sure env var MILHOUSE is set to dir where milhouse is 
test: libmath.so
	$(CXX) $(CXXFLAGS) test.cpp -lmath -L${MILHOUSE}/code/lib/math -I${MILHOUSE}/code/ -o test

clean :
	rm -f *.o *.so 
	rm -f -R gcm.cache