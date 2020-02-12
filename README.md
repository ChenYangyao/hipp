---
description: Under Development
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

#### Test the Install

Once you successfully install the HIPP, you can enter the `example/cntl` directory in the repository.  Try

```bash
make print.out; ./print.out
```

to compile and run the example. If error, you may specify the compiling command by you self, such as `g++ -o print.out print.cpp` and run the executive by `./print.out`. If still error, you may check that if the lib and include environment is set in the install step. If successful, on the output device it shows

```text
Sum of 1 and 2 and 3 is 6
Up to precision 10, a is 1.0000000000
The array is 1,2,3,4,5
```

which means the install is successful. Enjoy using HIPP!

## Documentation

The documentation of HIPP is organized into three layers, with increasing depth.  

* The [Tutorial](documentation/tutorial/) provides a basic guide of how to use HIPP to perform basic enhancement of C++ language, perform parallel computation with OOP style MPI interface,  or doing I/O with the IO interface. 
* The [User's Guide](documentation/users-guide.md) gives a more deep description about the design, the structure and implementation detail of HIPP.
* The [Interfaces by Modules](documentation/interfaces-by-modules/) provides a introduction of HIPP categorized by Modules and Submodules.

## Other Issues

### License

HIPP is distributed under GPL version 3. See the LICENSE file for the detail license terms.

