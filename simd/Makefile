CXX = g++
CXXLFAGS = -std=c++11 -Wall -O3 -mavx2 -I./header/
LDFLAGS = 
LDLIBS = -lmymathCC


%.out: %.cpp
	$(CXX) $(CXXLFAGS) $(LDFLAGS) $^ -o $@ $(LDLIBS)

%.s: %.cpp
	$(CXX) $(CXXLFAGS) -S $^ -o $@