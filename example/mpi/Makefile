CXX = mpicxx
CXXFLAGS = -std=c++17 -O3 -Wall
LDFLAGS = 
LDLIBS = -lhippmpi -lhippcntl

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
