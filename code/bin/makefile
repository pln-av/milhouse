CXX := /opt/homebrew/bin/g++-11
CXXFLAGS := -std=c++20 -Wall -Werror
HDR := ${MILHOUSE}/code
LIB := ${MILHOUSE}/code/lib
SHELL=/bin/bash

LIBS := ${MILHOUSE}/code/lib/option
sim: sim.C
	$(CXX) $(CXXFLAGS) sim.C -loption -lutil -lmath -L$(LIB)/util -L$(LIB)/option -L$(LIB)/math -I${MILHOUSE}/code -o sim 

clean :
	rm -f *.o *.so 
	rm -f -R gcm.cache