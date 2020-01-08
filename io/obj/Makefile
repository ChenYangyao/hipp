all: libhippio.so
.PHONY: all

CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall
LDFLAGS = -fPIC -shared
LDLIBS =

OBJS := $(wildcard *.o)
%.so: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
