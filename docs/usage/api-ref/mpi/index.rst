*********************************
MPI Module
*********************************

.. include:: /global.rst

.. _api-mpi-usage:

MPI Module Overview
=====================

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


List of Module Components 
===========================

All functions and classes avaiable in the HIPP MPI module is listed below. The "Standard MPI counterpart" gives 
the MPI type defined by the MPI standard on the top of which HIPP is built.

.. namespace:: HIPP::MPI

.. table::
    :class:   fix-width-table tight-table
    :widths:  20 15 63

    ======================================= =================================== ================================================================================
    Functions / Classes                     Standard MPI counterpart            Details 
    ======================================= =================================== ================================================================================
    :class:`Env`                                                                MPI environment manager.
    :class:`ErrMPI`                                                             Exception that is thrown on the MPI-related error.
    :class:`Comm`                           ``MPI_Comm``                        Communicator.
    :class:`Group`                          ``MPI_Group``                       Process group.
    :class:`Requests`                       ``MPI_Request``                     Handler for non-blocking commnications.
    :class:`Status`                         ``MPI_Status``                      Return status of commnications.
    :class:`Datatype`                       ``MPI_Datatype``                    Data type which describes the data layout and representation.
    :class:`Datapacket`                                                         Data buffer descriptor.
    :class:`Pack`                                                               Buffers for packing/unpacking operations.
    :class:`ExternalPack`                                                       Buffers for canonical packing/unpacking operations.
    :class:`Op`                             ``MPI_Op``                          Reduction operation on data item.
    :class:`Oppacket`                                                           Operation descriptor.
    :class:`Win`                            ``MPI_Win``                         One-side commnication window.
    :class:`File`                           ``MPI_File``                        Parallel IO handler.
    :class:`Message`                        ``MPI_Message``                     Message object returned by probing operation.
    :class:`Info`                           ``MPI_Info``                        Hints to the implementation.
    :class:`SpinLock`                                                           Spin locks for synchronization.
    :class:`Mutex`                                                              Mutual exclusion locks for synchronization.
    :class:`SeqBlock`                                                           Defining operations to be sequentially executed on procecess.
    :class:`WorkDecomp1D`                                                       Work decomposition functions.
    :func:`alloc_mem`                       ``MPI_Alloc_mem``                   Memory allocation.
    :func:`free_mem`                        ``MPI_Free_mem``                    Memory deallocation.
    :func:`buffer_attach`                   ``MPI_Buffer_attach``               Buffer attachment for buffer sending mode.
    :func:`buffer_detach`                   ``MPI_Buffer_detach``               Buffer detachment for buffer sending mode.
    :func:`wtime`                           ``MPI_Wtime``                       Clock.
    :func:`wtick`                           ``MPI_Wtick``                       Clock interval.
    ======================================= =================================== ================================================================================

API References
=================

High-level API
------------------

.. toctree::
    :maxdepth: 2

    high-level/env
    high-level/error
    high-level/comm
    high-level/datatype
    high-level/rma
    high-level/file
    high-level/other
    
MPI Extensions
------------------

.. toctree::
    :maxdepth: 2

    mpe/sync
    mpe/master-slave
    mpe/symm
    mpe/work-decomp
    

Intermediate-level API
-----------------------

.. toctree::
    :maxdepth: 2

    intermediate-level/index

References 
-----------

.. [MPIForum-STDv31] `Message Passing Interface Forum <https://www.mpi-forum.org/docs/>`_, MPI: A Message Passing Interface Standard (version 3.1).
.. [GroppW-UMPIv3] William Gropp, Ewing Lusk, and Anthony Skjellum, Using MPI - Portable Parallel Programming with the Message-Passing Interface (3ed).
.. [GroppW-UAMPI] William Gropp, Torsten Hoefler, Rajeev Thakur, Ewing Lusk, Using Advanced MPI - Modern Features of the Message-Passing Interface.
.. [DuZH-MPIv2001] Zhihui Du, 高性能计算之并行编程技术 - MPI并行程序设计 (2001年版).