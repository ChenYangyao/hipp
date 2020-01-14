CXX = mpicxx
CXXFLAGS = -std=c++11 -O2 -Wall -I../header -I../../cntl/header -Wno-noexcept-type
LDFLAGS = 
LDLIBS = -lmymathCC -L../../cntl/obj -lhippcntl -L../obj -lhippmpi

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
