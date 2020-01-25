all: build
.PHONY: all build install tidy clean

build: 
	$(MAKE) -C cntl
	$(MAKE) -C io

install:
	if [ -e 'include' ]; then rm include/*; else mkdir include; fi
	if [ -e 'lib' ]; then rm lib/*; else mkdir lib; fi
	cp -r cntl/obj/*.so io/obj/*.so lib/
	cp -r cntl/header/* io/header/* include/

uninstall:
	rm -rf include
	rm -rf lib

clean:
	$(MAKE) -C cntl clean 
	$(MAKE) -C io clean
	$(MAKE) uninstall

tidy:
	$(MAKE) -C cntl tidy
	$(MAKE) -C io tidy
	$(MAKE) uninstall

