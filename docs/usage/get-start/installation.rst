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
See the :ref:`Options <install-opt>` below.


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
    the default system location. To test the installation, use
    
    .. code-block:: bash 

        ctest -VV
    
Note that 

- To specify your own installation location, use :bash:`cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install`. 
  If successful, two folders named **lib** and **include** are generated, which contain 
  the library and header files, respectively. You may add them into the environment lists (e.g., :bash:`CPLUS_INCLUDE_PATH`, :bash:`LIBRARY_PATH` and :bash:`LD_LIBRARY_PATH`). 
- To compile the library parallelly, use :bash:`make -j number-of-processes`.
- For a CMake version that is recent enough, use :bash:`cmake --build .` to compile and 
  use :bash:`cmake --build . --target install` to install. They are equivalent to :bash:`make` and :bash:`make install`.


.. _install-opt:

Options
---------------------------

Other options can be specified by using :bash:`-D` when invoking :bash:`cmake`. For example 

.. code-block:: bash 

    cmake .. -Denable-mpi=ON -DCMAKE_CXX_COMPILER=g++ 

Allowed options are listed below.

HIPP supports a set of optional modules for MPI, IO, etc.

+--------------------------+-----------------------------+------------------------------+---------------------------+
| Module                   | CMake Option                |  Description                 |    System Requirement     |
+==========================+=============================+==============================+===========================+
| MPI                      | -Denable-mpi=ON             | Message Passing Interface    |  MPI environment |br|     |
|                          |                             |                              |  (standard >= 3.0)        |
+--------------------------+-----------------------------+------------------------------+---------------------------+                                                                                       
| IO                       | -Denable-hdf5=ON            | IO library                   |  HDF5 C library |br|      |
|                          |                             |                              |  (version >=1.8.0)        |
+--------------------------+-----------------------------+------------------------------+---------------------------+                                                                                    
| SIMD                     | -Denable-simd=ON            | Template SIMD library        | x86 Arch |br|             |
|                          |                             |                              | (supporting SIMD)         |
+--------------------------+-----------------------------+------------------------------+---------------------------+
| NUMERICAL                | -Denable-gsl=ON             | The scientific library       |  GSL (version >=2.6)      |
+--------------------------+-----------------------------+------------------------------+---------------------------+
| ALGORITHM                | Automatically |br|          | The algorithms               |                           |
|                          | installed                   |                              |                           |
+--------------------------+-----------------------------+------------------------------+---------------------------+

Note that 

- The options for modules can be turned on simultaneously.
- GSL (GNU scientific libraries) must be installed when :bash:`enable-gsl` is turned on. By default, the library 
  in the system environmental list (e.g., :bash:`CPLUS_INCLUDE_PATH` and :bash:`LIBRARY_PATH`). You may override 
  this by :bash:`-DGSL_ROOT_DIR=/path/to/gsl`, which must include the :bash:`lib` and :bash:`include/gsl` subdirectories.
- HDF5 must be installed when :bash:`enable-hdf5` is turned on. CMake uses the commands like :bash:`h5cc` to detect 
  the information of HDF5 library. You may add the bin directory of HDF5 to your system environmental list (e.g., :bash:`PATH`).

Other options that can be specified by :bash:`-D` in the invode of :bash:`cmake` are 

======================================= ======================================================================================
Option                                  Description                     
======================================= ======================================================================================
-DCMAKE_C_COMPILER=/path/to/compiler    The C compiler used to compile third-party libraries. |br| (default: detected by CMake)
-DCMAKE_CXX_COMPILER=/path/to/compiler  The C++ compiler (must support standard 17). |br| (default: detected by CMake)
-DMPI_CXX_COMPILER=/path/to/compiler    The MPI compiler wrapper. |br| (default: detected by CMake)
-DCMAKE_CXX_FLAGS="flag1 flag2 ..."     Compiling and linking flags. |br| (default: "-O3 -Wall")
-DBUILD_TESTING=ON|OFF                  Whether to build test cases. |br| (default: ON)
======================================= ======================================================================================






