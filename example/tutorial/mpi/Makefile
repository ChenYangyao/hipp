MPICXX = mpicxx 
CXXFLAGS = -std=c++17 -O3 -Wall
LDFLAGS = 
LDLIBS = -lhippmpi -lhippcntl

%.mp.out: %.cpp
	$(MPICXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

EXECS := $(patsubst %.cpp,%.mp.out,$(wildcard *.cpp))
$(EXECS): Makefile
