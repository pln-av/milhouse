CXX := /opt/homebrew/bin/g++-11
CXXFLAGS := -std=c++20 -Wall -Werror
SHELL=/bin/bash
# this rule wont work inside make, for some reason... ? i 
# cant be bothered working out why.  d
# the rest is ok
sysheader : 
	./sysheaders.sh

OptionModuleObjects := iOption.o iOptionBlackScholes.o iOptionVolCurve.o OptionBlackScholes.o OptionVolCurve.o iOptionUtil.o OptionUtil.o
libOption: $(OptionModuleFiles)
	$(CXX) $(CXXFLAGS) -fmodules-ts -c -fpic iOptionBlackScholes.cpp OptionBlackScholes.cpp
	$(CXX) $(CXXFLAGS) -fmodules-ts -c -fpic iOptionVolCurve.cpp OptionVolCurve.cpp
	$(CXX) $(CXXFLAGS) -fmodules-ts -c -fpic iOptionUtil.cpp OptionUtil.cpp -I${MILHOUSE}/code
	$(CXX) $(CXXFLAGS) -fmodules-ts -c -fpic iOption.cpp
	$(CXX) $(CXXFLAGS) -shared -o libOption.so $(OptionModuleObjects)

# make sure env var MILHOUSE is set to dir where milhouse is 
LIBS := ${MILHOUSE}/code/lib/Option
test: test.cpp libOption.so
	$(CXX) $(CXXFLAGS) -fmodules-ts test.cpp -lOption -L$(LIBS) -o test

clean :
	rm -f *.o *.so 
	rm -f -R gcm.cache