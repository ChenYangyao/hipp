all: libhippmpi.so
.PHONY: all

CXX = mpicxx
CXXFLAGS = -std=c++11 -O3 -Wall
LDFLAGS = -fPIC -shared
LDLIBS = -L../../cntl/obj -lhippcntl

OBJS := $(wildcard *.o)
%.so: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
