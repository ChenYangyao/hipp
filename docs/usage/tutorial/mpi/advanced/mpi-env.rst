The MPI Environment
====================

.. include:: /global.rst

.. namespace:: HIPP::MPI

The Environment Object
---------------------------

The environment object is responsible for the initialization and finalization 
of MPI environment. It must be defined at the 
beginning of the ``main()`` function and get destroyed at the end. 
The constructor and destructor of the environment object call standard 
MPI subroutines to perform actual environment management.

Hence, we require that a MPI program using HIPP must be organized like the 
following::

    int main(int argc, char *argv[]) {
        mpi::Env env;

        // use HIPP MPI module

        return 0;
    }

The :class:`Env` object can be default-constructed. But the most-portable way 
is to pass the references of ``argc`` and ``argv`` to construct it::

    mpi::Env env(argc, argv);

In multi-thread applications, you may pass two additional arguments ``required``
and ``provided`` to ensure thread safety::

    
    int required = mpi::Env::THREAD_MULTIPLE, 
        provided;
    mpi::Env env(argc, argv, required, provided);

The ``required`` thread safety hints the MPI implementation and the out-argument
``provided`` returns the actual thread safety supported by the library.

After the initialization of MPI environment, it is safe to get the "WORLD"
communicator through the static method :expr:`Env::world` 
and perform communications on it::

    auto comm = mpi::Env::world();

Inquiry the Library Information
---------------------------------

To get a quick look about your MPI library, stream the environment object 
directly like::

    pout << env;

The output is:

.. code-block:: text

    HIPP::MPI::Env instance [loc=0x7ffd806a2d34, size=4, align=4]
    ----------
    Standard: 3.1
    Runtime Environment (TAG UB=268435455, HOST=None, IO RANK=Any, WTIME GLOBAL=No)
    Processor name: local-HP


To get the MPI standard version supported by the library, use either the preprocessor 
macro :c:macro:`HIPPMPI_STD_VERSION` and :c:macro:`HIPPMPI_STD_SUBVERSION`, or 
the static method :expr:`Env::version()`::

    pout << "version = ", HIPPMPI_STD_VERSION, ".", HIPPMPI_STD_SUBVERSION, endl;
    
    int ver, sub_ver;
    mpi::Env::version(ver, sub_ver);
    pout << "version = ", ver, ".", sub_ver, endl;

The output is (with a MPI-3.1 compliant implementation):

.. code-block:: text

    version = 3.1
    version = 3.1

Another method, :expr:`Env::library_version()`, returns a string that describes 
the version information of the library::

    pout << mpi::Env::library_version(), endl;

The output is (with a MPICH implementation):

.. code-block:: text

    MPICH Version:  3.3.2
    MPICH Release date:     Tue Nov 12 21:23:16 CST 2019
    MPICH ABI:      13:8:1
    MPICH Device:   ch3:nemesis
    ...

Other methods of :class:`Env` are defined to inquire different piecees of 
information about the library::

    pout << "Tag upper bound: ",     env.tag_ub(),           '\n',
            "Host rank: ",           env.host(),             '\n',
            "Rank of IO process: ",  env.io(),               '\n',
            "Wtime is global: ",     env.wtime_is_global(),  '\n',
            "Host machine: ",        env.processor_name(),   '\n';

The output on my compute is:

.. code-block:: text

    Tag upper bound: 268435455
    Host rank: -1
    Rank of IO process: -2
    Wtime is global: 0
    Host machine: local-HP


    