---
description: Under developed
---

# HIPParcos - HIgh Performance computation Package for cosmological simulations

## Get Started

### Requirements

* C++ compiler \(standard &gt;= 11\)
* CMake compiling system generator \(version &gt;= 3.9\)

The followings are optional:

* MPI implementation \(headers, libraries, compiler wrappers, MPI-luanchers; must with standard &gt;= 3.0\)
* HDF5 implementation

### Install

#### Quick Install

Clone or download this repository, then enter the root directory of this repository and execute

```bash
cmake ./ -Dprefix=./
make
make install
```

These configure, build and install HIPP in the current directory. If successful, two folders named `lib`, `include` appear, which include the libraries and header files, respectively. You can now modify the environment lists \(e.g., `LIBRARY_PATH`, `LD_LIBRARY_PATH` and `CPLUS_INCLUDE_PATH`\) to allow the compiler system searching the installed package, e.g.,

```bash
export CPLUS_INCLUDE_PATH="/path/to/repo/include:${CPLUS_INCLUDE_PATH}"
export LIBRARY_PATH="/path/to/repo/lib:${LIBRARY_PATH}"
export LD_LIBRARY_PATH="/path/to/repo/lib:${LD_LIBRARY_PATH}"
```

where you should replace the "/path/to/repo" with the directory of this repositoty. You may add these lines into you  shell init scripts, such as `${HOME}/.bashrc` or `${HOME}/.bash_profile`, then it will set the environment when you login the shell.

## Other Issues

### License

HIPP is distributed under GPL version 3. See the LICENSE file for the detail license terms.

