CXX = c++
CXXFLAGS = -std=c++17 -O3 -Wall -I../../include
LDFLAGS = 
LDLIBS = -lhippio -lhippcntl -lhdf5

MPICXX = mpicxx 
MPILDLIBS = -lhippmpi

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

%.mp.out: %.cpp
	$(MPICXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(MPILDLIBS) $(LDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp)) 
$(EXECS): Makefile

.PHONY: clean

clean:
	rm *.out *.mp.out *.h5
