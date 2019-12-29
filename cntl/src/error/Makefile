TARGET_DIR := ../../obj
OBJS := $(patsubst %.cpp, $(TARGET_DIR)/%.o, $(wildcard *.cpp))
.PHONY: all
all: $(OBJS)

CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall -I../../header

$(TARGET_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<



