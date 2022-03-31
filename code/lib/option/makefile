CXX := /opt/homebrew/bin/g++-11
CXXFLAGS := -std=c++20 -Wall -Werror
HDR := ${MILHOUSE}/code
LIB := ${MILHOUSE}/code/lib
SHELL=/bin/bash

BlackScholes.o: BlackScholes.C
	$(CXX) $(CXXFLAGS) -c -fpic -I$(HDR) BlackScholes.C 

Infra.o: Infra.C 
	$(CXX) $(CXXFLAGS) -c -fpic -I$(HDR) Infra.C 

liboption.so: BlackScholes.o Infra.o
	$(CXX) $(CXXFLAGS) -lutil -L$(LIB)/util -shared -o liboption.so BlackScholes.o Infra.o

# make sure env var MILHOUSE is set to dir where milhouse is 
LIBS := ${MILHOUSE}/code/lib/option
test: test.cpp liboption.so
	$(CXX) $(CXXFLAGS) test.cpp -loption -lutil -L$(LIB)/util -L$(LIB)/option -I${MILHOUSE}/code -o test

clean :
	rm -f *.o *.so 
	rm -f -R gcm.cache