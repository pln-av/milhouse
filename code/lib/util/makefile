CXX := /opt/homebrew/bin/g++-11
CXXFLAGS := -std=c++20 -Wall -Werror
SHELL=/bin/bash

LIBS := ${MILHOUSE}/code/lib
libutil: Time.H Time.C Sim.H Sim.C
	$(CXX) $(CXXFLAGS) -c -fpic -I${MILHOUSE}/code Time.C 
	$(CXX) $(CXXFLAGS) -c -fpic -I${MILHOUSE}/code Main.C
	$(CXX) $(CXXFLAGS) -c -fpic -loption -L$(LIBS)/option -I${MILHOUSE}/code Sim.C
	$(CXX) $(CXXFLAGS) -loption -L$(LIBS)/option -shared -o libutil.so Time.o Sim.o Main.o

# make sure env var MILHOUSE is set to dir where milhouse is 
LIBS := ${MILHOUSE}/code/lib
test: test.cpp libutil.so
	$(CXX) $(CXXFLAGS) test.cpp -lutil -L$(LIBS)/util -I${MILHOUSE}/code -o test

reader: test_read.cpp 
	$(CXX) $(CXXFLAGS) test_read.cpp -lutil -loption -L$(LIBS)/util -L$(LIBS)/option -I${MILHOUSE}/code -o reader

clean :
	rm -f *.o *.so 
	rm -f -R gcm.cache