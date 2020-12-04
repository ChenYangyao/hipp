Installation
===============================

.. _install-requirement:

System Requirements
--------------------------

- A Modern C++ compiler (supports C++ standard 17).
- CMake (version >= 3.9).

The library is tested to work on a GNU-compliant compiling system, but have not tested on other compilers. Feel free 
to contact us if you have any problem.

Depending on the modules you want to install, other libraries may be necessary. 
See the :ref:`Installation Options <install-opt>`.


How to Install
---------------------------

(1) Clone or download the `Github <https://github.com/ChenYangyao/hipp>`_ repository 
    (unzip it if compressed), then enter the root directory of it. 

(2) Execute in you command.

    .. code-block:: bash 

        cmake ./ -Dprefix=./
        make 
        make install 

    This configures, builds and installs **HIPP** in the current directory. 
    If successful, two folders named **lib** and **include** are generated, which contain 
    the library and header files, respectively. 
    You can modify the environment lists (e.g., :bash:`LIBRARY_PATH`, :bash:`LD_LIBRARY_PATH` and :bash:`CPLUS_INCLUDE_PATH`) 
    to allow the installed files found by your compiling system.

To test whether the library is successfully installed, enter the directory **example/cntl** and try 

.. code-block:: bash 

    make print.out && ./print.out

This compiles and run an example file. If successful, it prints some meaningful results.

.. _install-opt:

**Optionally**, you can install other modules supported by HIPP. For example, if your 
system has a MPI environment, you can enable the installation of the MPI module in HIPP, by turning
the corresponding option on in the invoke of cmake:

.. code-block:: bash 

    cmake ./ -Dprefix=./ -Denable-mpi=ON

The supported modules and system-requirements are 

========================== ================================ =============================================
Module                     Description                      System Requirement
========================== ================================ =============================================
mpi                        Message Passing Interface        A MPI environment (supports standard >= 3.0)
hdf5                       IO library                       C environment for HDF5 (>=1.8.0)
simd                       Template SIMD library            x86 Arch that supports SIMD 
gsl                        The scientific library           GSL (>=2.6) 
========================== ================================ =============================================
        
These options can be turned on simultaneously. e.g. :bash:`-Denable-mpi=ON -Denable-gsl=ON`.

Other options that can be specified by :bash:`-D` in the invode of :bash:`cmake` are 

========================== ====================================================
Option                     Description                     
========================== ====================================================
cxx                        The C++ compiler (must support standard 17)
prefix                     The location to install the binary and header files
========================== ====================================================





