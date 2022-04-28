CXX := /opt/homebrew/bin/g++-11
CXXFLAGS := -std=c++20 -Wall -Werror
HDR := ${MILHOUSE}/code
LIB := ${MILHOUSE}/code/lib
SHELL=/bin/bash

BlackScholes.o: BlackScholes.C BlackScholes.H
	$(CXX) $(CXXFLAGS) -c -fpic -lmath -L$(LIB)/math -I$(HDR) BlackScholes.C 

Infra.o: Infra.C Infra.H
	$(CXX) $(CXXFLAGS) -c -fpic -I$(HDR) Infra.C 

Vol.o: Vol.C Vol.H
	$(CXX) $(CXXFLAGS) -c -fpic -I$(HDR) Vol.C 

liboption.so: BlackScholes.o Infra.o Vol.o
	$(CXX) $(CXXFLAGS) -lutil -lmath -L$(LIB)/util -L$(LIB)/math -shared -o liboption.so BlackScholes.o Infra.o Vol.o

# make sure env var MILHOUSE is set to dir where milhouse is 
LIBS := ${MILHOUSE}/code/lib/option
test: test.cpp liboption.so
	$(CXX) $(CXXFLAGS) test.cpp -loption -lutil -lmath -L$(LIB)/util -L$(LIB)/option -L$(LIB)/math -I${MILHOUSE}/code -o test

test_vol: test_vol.cpp liboption.so 
	$(CXX) $(CXXFLAGS) test_vol.cpp -loption -lutil -lmath -L$(LIB)/util -L$(LIB)/option -L$(LIB)/math -I${MILHOUSE}/code -o test_vol

clean :
	rm -f *.o *.so 
	rm -f -R gcm.cache