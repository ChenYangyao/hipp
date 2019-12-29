CXX = g++
CXXLFAGS = -std=c++11 -Wall -O3 -mavx2
LDFLAGS = 
LDLIBS = -lmymathCC


%.out: %.cpp
	$(CXX) $(CXXLFAGS) $(LDFLAGS) $< -o $@ $(LDLIBS)

%.s: %.cpp
	$(CXX) $(CXXLFAGS) -S $< -o $@

SRCS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(SRCS): 