CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall -I../header/
LDFLAGS = 
LDLIBS = -L../obj -lhippcntl

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
