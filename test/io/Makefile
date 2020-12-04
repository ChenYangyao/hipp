CXXFLAGS = $(CXX_BASE_FLAGS) -pthread
LDFLAGS = 
LDLIBS = $(CXX_BASE_LIBDIRS) -lhippcntl
.PHONY: clean gtest

EXECS := $(patsubst %.cpp,%.out,$(wildcard *.cpp))
$(EXECS): Makefile
%.out: %.cpp
	rm -f $@
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS) \
		-lhippio -lhdf5
%.mpout: %.cpp
	rm -f $@
	mpicxx $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LDLIBS) \
		-lhippio_parallel -lhippmpi -lhdf5_parallel

GTEST_OBJS := $(patsubst %.gtest.cpp,%.gtest.o,$(wildcard *.gtest.cpp))
%.gtest.o: %.gtest.cpp 
	$(CXX) $(CXXFLAGS) -c -o $@ $<
gtest.out: $(GTEST_OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS) -lhippio -lhdf5 -lgtest_main -lgtest 
gtest:
	make gtest.out && ./gtest.out

clean:
	rm -f *.gtest.o 
	rm -f *.gtest.out 
	rm *.out
	rm *.h5


