CXX = c++
CXXFLAGS = -std=c++11 -O2 -Wall
LDFLAGS = 
LDLIBS = -lhippcntl

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
