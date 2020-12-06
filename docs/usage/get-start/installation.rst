Installation
===============================

.. include:: /global.rst

.. _install-requirement:

System Requirements
--------------------------

- Modern C++ compiler (supports C++ standard 17).
- CMake (version >= 3.9).

The library is tested to work on a GNU-compliant compiling system, but have not tested on other compilers. Feel free 
to contact us if you have any problem.

Depending on the modules you want to install, other libraries may be necessary. 
See the :ref:`Installation Options <install-opt>` below.


How to Install
---------------------------

(1) Clone or download the `Github <https://github.com/ChenYangyao/hipp>`_ repository 
    (unzip it if compressed), then enter the root directory of it. 

(2) Execute the following shell commands:

    .. code-block:: bash 

        mkdir build && cd build
        cmake ..
        make 
        make install 

    This configures, builds the library in the :bash:`build/` directory, and install it into 
    the default system location. 
    
    If you want to specify you own installation location, use :bash:`cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install` instead. 
    If successful, two folders named **lib** and **include** are generated, which contain 
    the library and header files, respectively. You may add them into environment list. 

    To compile the library parallelly, use :bash:`make -j number-of-processes`.

    If your CMake version is recent enough, you can use :bash:`cmake --build . -j number-of-processes` to compile and 
    use :bash:`cmake --build . --target install` to install.

(3) To test whether the library is successfully installed, execute

    .. code-block:: bash 

        make test 

    This runs the test cases and outputs (if any) the errors discovered. 
    Instead, you may use :bash:`ctest -VV` to run the tests and print more detailed information.

.. _install-opt:


Install Optional Modules
---------------------------


**Optionally**, you can install other modules supported by HIPP. For example, if your 
system has a MPI environment, you can enable the installation of the MPI module in HIPP, by turning
the corresponding option on in the invoke of cmake:

.. code-block:: bash 

    cmake .. -Denable-mpi=ON

The supported modules and system-requirements are (can be turned on simultaneously): 

========================== ========================== ================================ =============================================
Module                     CMake Option               Description                      System Requirement
========================== ========================== ================================ =============================================
MPI                        -Denable-mpi=ON            Message Passing Interface        MPI environment (standard >= 3.0)
IO                         -Denable-hdf5=ON           IO library                       HDF5 C library (version >=1.8.0)
SIMD                       -Denable-simd=ON           Template SIMD library            x86 Arch that supports SIMD 
NUMERICAL                  -Denable-gsl=ON            The scientific library           GSL (>=2.6) 
ALGORITHM                  Automatically installed    The algorithms                   
========================== ========================== ================================ =============================================
        
Other options that can be specified by :bash:`-D` in the invode of :bash:`cmake` are 

======================================= ===================================================================
Option                                  Description                     
======================================= ===================================================================
-DCMAKE_CXX_COMPILE=/path/to/compiler   The C++ compiler (must support standard 17)
-DCMAKE_CXX_FLAGS="flag1 flag2 ..."     Compiling and linking flags (default: "-O3 -Wall")
-DBUILD_TESTING=OFF                     Whether to build test cases (default: ON)
======================================= ===================================================================





