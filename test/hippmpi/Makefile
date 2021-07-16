CXX = mpicxx
CXXFLAGS = -std=c++17 -O3 -Wall
LDFLAGS = 
LDLIBS = -lhippmpi -lhippcntl

all: run_test
.PHONY: all run_test
	
EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

EXES_TEST = t_pack.test t_p2p.test

run_test: $(EXES_TEST)

%.test : %.out
	mpirun -n 8 ./$<

