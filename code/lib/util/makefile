CXX := /opt/homebrew/bin/g++-11
CXXFLAGS := -std=c++20 -Wall -Werror
SHELL=/bin/bash

libutil: Time.H Time.C 
	$(CXX) $(CXXFLAGS) -c -fpic -I${MILHOUSE}/code Time.C 
	$(CXX) $(CXXFLAGS) -shared -o libutil.so Time.o 

# make sure env var MILHOUSE is set to dir where milhouse is 
LIBS := ${MILHOUSE}/code/lib/util
test: test.cpp libutil.so
	$(CXX) $(CXXFLAGS) test.cpp -lutil -L$(LIBS) -I${MILHOUSE}/code -o test

clean :
	rm -f *.o *.so 
	rm -f -R gcm.cache