CXXFLAGS = $(CXX_BASE_FLAGS) -pthread
LDFLAGS = 
LDLIBS = $(CXX_BASE_LIBDIRS) -lhippcntl

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
%.out: %.cpp
	rm -f $@
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS)

GTEST_OBJS := $(patsubst %.gtest.cpp,%.gtest.o,$(wildcard *.gtest.cpp))
%.gtest.o: %.gtest.cpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
gtest.out: $(GTEST_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lgtest_main -lgtest
gtest:
	make gtest.out && ./gtest.out

clean:
	rm -f *.gtest.o 
	rm -f *.gtest.out 
	rm -f *.out


