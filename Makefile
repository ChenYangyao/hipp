all: build
.PHONY: all build install tidy clean


build: 
	make -C cntl
	make -C io

install:
	if [ -e 'include' ]; then rm include/*; else mkdir include; fi
	if [ -e 'lib' ]; then rm lib/*; else mkdir lib; fi
	cp -r cntl/obj/*.so io/obj/*.so lib/
	cp -r cntl/header/* io/header/* include/

clean:
	make -C cntl clean 
	make -C io clean
	rm -r include
	rm -r lib

tidy:
	make -C cntl tidy
	make -C io tidy
	rm -r include
	rm -r lib

