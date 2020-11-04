*********************************
API Reference of the MPI Module
*********************************

.. _api-mpi-usage:

The ``HIPP::MPI`` module includes high-level OOP APIs that encapsulates **Standard** Message Passing Interface 
(MPI) library. 
It also provides non-standard extensions of MPI, which are useful in synchronization, task management and 
task distribution, etc.

Although the high-level APIs are always preferred to use, sometimes, user may need to 
go back to the oringinal MPI calls, possibly due to the demands of better performance 
or more flexible and detailed control of the communication behavior. The intermediate-level 
APIs, which is only semi-OOP, therefore provide convient tools for this purpose.

All the definitions in the MPI module can be accessed by including the header file **<hippmpi.h>**.
Classes, functions and variables are all defined within the ``HIPP::MPI`` namespace.
Code that uses the MPI module would be like ::

    /* src.cpp */
    #include <hippmpi.h>                    // include all definitions in HIPP MPI

    int main(int argc, char const *argv[]){
        HIPP::MPI::Env env;                 // initialize the MPI environment
        auto world = env.world();           // the communication world object

        // use HIPP MPI to make communications
    }

To generate the executable binary, the links to **"libhippmpi.so"** and **"libhippcntl.so"** are necessary.
The compiling command in an Unix-like system would be like

.. code-block:: bash 

    $ mpicxx -std=c++17 src.cpp -lhippmpi -lhippcntl

.. _api-mpi-convention:

To avoid confusion, we list the conventions used in this API reference:

* The term **Standard** means the MPI standard APIs (types, functions, variables, macros, etc.) 
  defined by the `MPI Forum <https://www.mpi-forum.org/mpi-31/>`_ . E.g. The **Standard** call ``MPI_Init()`` 
  is the entrance of (almost) all **Standard** MPI applications.
* The HIPP MPI library has three components: the **High-level API**, the **MPI Extensions** and 
  the **Intermediate-level API** (see below for the detail API references of each component). 
  Therefore, when we say "high-level" class communicator, we mean the class :class:`HIPP::MPI::Comm` 
  in the high-level API, and when we say "extension" class Mutex, we mean the class :class:`HIPP::MPI::Mutex` 
  in the extension API.  
* The documentation text of one definition would probably refer to other definitions. If they are in the 
  same namespace (``HIPP::MPI`` here), we drop the namespace specifiers for clarity 
  (e.g., class :class:`Comm <HIPP::MPI::Comm>` means class :class:`HIPP::MPI::Comm`). 
* The example code that uses C++ standard library may ignore the ``std::`` namespace specifier (namely, assume 
  a global ``using namespace std`` is used). This is not good in practice, but makes the example code more compact. 

.. toctree::
    :maxdepth: 2
    :caption: High-level API

    high-level/env
    high-level/error
    high-level/comm
    high-level/datatype
    high-level/rma
    high-level/file
    high-level/other
    
.. toctree::
    :maxdepth: 2
    :caption: MPI Extensions

    mpe/sync
    mpe/master-slave
    mpe/symm
    
.. toctree::
    :maxdepth: 2
    :caption: Intermediate-level API

    intermediate-level/index