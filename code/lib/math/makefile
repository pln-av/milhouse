CXX := /opt/homebrew/bin/g++-11
CXXFLAGS := -std=c++20 -Wall -Werror
SHELL=/bin/bash
# this rule wont work inside make, for some reason... ? i 
# cant be bothered working out why.  d
# the rest is ok
sysheader : 
	./sysheaders.sh

MathModuleObjects := iMath.o iMathIntegration.o iMathSpecialFunction.o MathIntegration.o MathSpecialFunction.o
libmath.so: $(MathModuleFiles)
	$(CXX) $(CXXFLAGS) -fmodules-ts -c -fpic iMathIntegration.cpp MathIntegration.cpp
	$(CXX) $(CXXFLAGS) -fmodules-ts -c -fpic iMathSpecialFunction.cpp MathSpecialFunction.cpp
	$(CXX) $(CXXFLAGS) -fmodules-ts -c -fpic iMath.cpp
	$(CXX) $(CXXFLAGS) -shared -o libmath.so $(MathModuleObjects)

# make sure env var MILHOUSE is set to dir where milhouse is 
LIBS := ${MILHOUSE}/code/lib/math
test: test.cpp libmath.so
	$(CXX) $(CXXFLAGS) -fmodules-ts test.cpp -lmath -L$(LIBS)

clean :
	rm -f *.o *.so 
	rm -f -R gcm.cache