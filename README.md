---
description: Under Development
---

# HIPParcos - HIgh Performance computation Package for cosmological simulations

## Get Started

### Requirements

* GNU compatible C++ compiler \(standard &gt;= 11\)
* CMake - the compiling system generator \(version &gt;= 3.9\)

The followings are optional:

* MPI implementation \(headers, libraries, compiler wrappers, MPI-luanchers; must with standard &gt;= 3.0\)
* HDF5 implementation \(headers, libraries; version &gt;=1.8.0\)

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

#### Building and Installation Options

To change the build direcoty, first create a new directory, enter it, and invokde cmake with `/path/to/repository`.  This will use the repository '/path/to/repository' to configure the build system in the current directory. Then you may use `make; make install` to finish the build and installation. 

To change installation directory, use `-Dprefix`.  By default, if you do not specify `prefix`,  HIPP will installed into the system directory, e.g. /usr/local. 

To speed up the building with multiple processors, use `make -j num_of_processes`.

```bash
cmake /path/to/repository -Dprefix=/path/to/install
make -j N
make install
```

The above commands configure and build HIPP in the current directory, use the source code in the /path/to/repository, with N processes, and install HIPP into the path '/path/to/install'. 

The -D option is additive. You may specify multiple -D options.

HIPP provides some optional functions. If your system has the depencency it needs, You may install them  by specifying the options when running the cmake command.

| Options | Dependency | Decription |
| :--- | :--- | :--- |
| -Denable-mpi=ON | MPI \(&gt;=3.0\) | The HIPP MPI Module |
| -Denable-hdf5=ON | HDF5 \(&gt;=1.8.0\) | The HIPP IO Module |
| -Denable-simd=ON | Intel x86 architecture | The HIPP SIMD Module |
| -Dcxx=any\_compiler | Any C++ compiler \(&gt;=11\) | Use other C++ compiler instead  of the default |

## Documentation

The documentation of HIPP is organized into three layers, with increasing depth.  

* The [Tutorial](documentation/tutorial/) provides a basic guide of how to use HIPP to perform basic enhancement of C++ language, perform parallel computation with OOP style MPI interface,  or doing I/O with the IO interface. 
* The [User's Guide](documentation/users-guide.md) gives a more deep description about the design, the structure and implementation detail of HIPP.
* The [Interfaces by Modules](documentation/interfaces-by-modules/) provides a introduction of HIPP categorized by Modules and Submodules.

## Other Issues

### License

HIPP is distributed under GPL version 3. See the LICENSE file for the detail license terms.

