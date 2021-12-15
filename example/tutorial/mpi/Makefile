MPICXX = mpicxx 
CXXFLAGS = -std=c++17 -O3 -Wall
LDFLAGS = 
LDLIBS = -lhippmpi -lhippcntl -lhippnumerical -lgsl -lgslcblas -lhippio -lhdf5

%.out: %.cpp
	$(MPICXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
