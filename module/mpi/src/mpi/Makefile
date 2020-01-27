TARGET_DIR := ../../obj
OBJS := $(patsubst %.cpp, $(TARGET_DIR)/%.o, $(wildcard *.cpp))
.PHONY: all
all: $(OBJS)

libbase = ../../..
CXX = mpicxx
CXXFLAGS = -std=c++11 -O3 -Wall -fPIC \
	-I$(libbase)/mpi/header -I$(libbase)/mpi/header/mpi -I$(libbase)/cntl/header

$(TARGET_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<