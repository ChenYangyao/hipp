*************************************
HIPP::MPI Module
*************************************

Terms and Conventions
------------------------

The conventions in terms of MPI are:

* We use term **Standard** to means the MPI standard APIs (types, functions, variables, macros, etc.) defined by the `MPI Forum <https://www.mpi-forum.org/mpi-31/>`_ . E.g. The **Standard** call ``MPI_Init()`` is the entrance of (almost) all MPI programs.


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