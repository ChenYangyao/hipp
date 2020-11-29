CXX = g++
CXX_BASE_FLAGS = -std=c++17 -O3 -Wall -I$(CURDIR)/../include 
CXX_BASE_LIBDIRS = -L$(CURDIR)/../lib
export CXX CXX_BASE_FLAGS CXX_BASE_LIBDIRS

subdirs = cntl io
.PHONY: gtest $(subdirs)

gtest: $(subdirs)

$(subdirs):
	$(MAKE) -C $@ gtest
	
clean:
	for d in $(subdirs); do $(MAKE) -C $$d clean; done



