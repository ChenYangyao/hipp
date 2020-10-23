*************************************
HIPP::MPI Module
*************************************

.. _api-mpi-include:

The ``HIPP::MPI`` module includes high-level OOP APIs that encapsulates **Standard** Message Passing Interface 
(MPI) library. 
It also provides non-standard extensions of MPI, which are useful in synchronization, task management and 
task distribution, etc.

Although the high-level APIs are always preferred to use, sometimes, user may need to 
go back to the oringinal MPI calls, possibly due to the demands of better performance 
or more flexible and detailed control of the communication behavior. The intermediate-level 
APIs, which is only semi-OOP, therefore provide convient tools for this purpose.

All the definitions in ``HIPP::MPI`` can be accessed by including the header file ``<hippmpi.h>``.
To generate executable binary, the links to **"libhippmpi.so"** and **"libhippcntl.so"** are necessary,
e.g., by using **"-lhippmpi -lhippcntl"** compiler options.

.. _api-mpi-convention:

Terms and Conventions
------------------------

The conventions in terms of MPI are:

* We use term **Standard** to means the MPI standard APIs (types, functions, variables, macros, etc.) defined by the `MPI Forum <https://www.mpi-forum.org/mpi-31/>`_ . E.g. The **Standard** call ``MPI_Init()`` is the entrance of (almost) all **Standard** MPI applications.

.. _api-mpi-toc:

.. toctree:: 
    :maxdepth: 1
    :caption: High-level API

    High-level API Overview <high-level/index>
    MPI Environment (definitions, Env) <high-level/env>
    Error Handling (ErrMPI) <high-level/error>
    Process Group and Communication (Comm, Group, Requests, Status) <high-level/comm>
    Datatype and Operation (Datatype, Datapacket, Op) <high-level/datatype>
    Remote Memory Access (Win) <high-level/rma>
    Parallel I/O (File) <high-level/file>
    Other Standard Facilities (Info, memory management) <high-level/other>
    
.. toctree:: 
    :maxdepth: 1
    :caption: Extended API

    Extended API Overview <mpe/index.rst>
    Synchronization (locks, sequential block) <mpe/sync>
    Master-slave Models <mpe/master-slave>
    Symmetric Models <mpe/symm>

.. toctree::
    :maxdepth: 1
    :caption: Intermediate-level API

    Intermediate-level API Overview <intermediate-level/index>