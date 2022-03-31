CXX := /opt/homebrew/bin/g++-11
CXXFLAGS := -std=c++20 -Wall -Werror
SHELL=/bin/bash

libutil: iUtil.H iUtil.C 
	$(CXX) $(CXXFLAGS) -c -fpic iUtil.C
	$(CXX) $(CXXFLAGS) -shared -o libutil.so iUtil.o

# make sure env var MILHOUSE is set to dir where milhouse is 
LIBS := ${MILHOUSE}/code/lib/util
test: test.cpp libutil.so
	$(CXX) $(CXXFLAGS) -fmodules-ts test.cpp -lutil -L$(LIBS) -o test

clean :
	rm -f *.o *.so 
	rm -f -R gcm.cache