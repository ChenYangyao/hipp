LIBDIR = $(M_LIB_ROOTDIR_DFLT)/lib

CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall
LDFLAGS = -L$(LIBDIR) -Wl,-rpath,$(LIBDIR)
LDLIBS = -lhippcntl -lhippnumerical -lgsl -lgslcblas

%.out: %.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
