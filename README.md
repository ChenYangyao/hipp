# HIPP - the HIPParcos HIgh-Performance computation Package

## Get Started

### Requirements

- C++ compiler (standard >= 11)
- CMake compiling system generator (version >= 3.9)

The followings are optional.

- MPI implementation (headers, libraries, compiler wrappers, MPI-luanchers; must with standard >= 3.1)
- HDF5 implementation

### Install

#### Quick Install
Enter the root directory of this repository. Then execute
```bash
cmake ./ -Dprefix=./
make
make install
```
This configure, build and install HIPP in the current directory. If successful, two folders named `lib`, `include` appear, which include the libraries and header files, respectively. You can now modify the environment lists (e.g. `LIBRARY_PATH`, `LD_LIBRARY_PATH` and `CPLUS_INCLUDE_PATH`) to allow the compiler system searching the installed package.

## Other Issues

### License

HIPP is distributed under GPL version 3. See the LICENSE file for the detail license terms.
