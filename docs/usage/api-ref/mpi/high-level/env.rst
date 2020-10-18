High-level API - MPI Environment
================================================

Preprocessing Macros
----------------------

    .. c:macro::    HIPPMPI_STD_VERSION
                    HIPPMPI_STD_SUBVERSION
                    HIPPMPI_STD

        These three macros are defined to be **Standard** ``MPI_VERSION``, ``MPI_SUBVERSION`` and ``( MPI_VERSION*100 + MPI_SUBVERSION )``.
        The last one provides a unique value that identifies the MPI library version.

        These three macros are used to detect the library version at preprocess-time. If it is not neccessary to perform such detections 
        at preprocess-time, it is better to use the methods of :class:`Env <HIPP::MPI::Env>` class.

.. namespace:: HIPP::MPI

.. class:: Env

    The construction of the ``Env`` object is the entrance of (almost) each program that uses MPI library,
    which perform the MPI environment initialization. The MPI environment is finalized at the destruction
    of the ``Env`` object. Because of the semantics of ``Env``, the ``Env`` instance should be defined once and only once, 
    typically at the begining of main(), and should not be copied or moved.

    Env object also provide methods that detect the MPI environment details, e.g., the host name, the libversion, etc.

    .. function:: Env()
            Env(int &argc, char **&argv )
            Env(int &argc, char **&argv, int required, int &provided )
            ~Env() noexcept
        
        Constructors and destructors. MPI **Standard** guarantees it is always valid 
        to use the default constructor. But it does no harm to pass the ``argc`` and 
        ``argv`` of ``main()`` to the constructor. Note that the ``argc`` and ``argv``
        args is not declared as const, since MPI **Standard** permits implementation
        to modify these two.

        In a threaded application, the third constructor is the standard way of 
        initialize MPI. The required thread-safty-level and supported thread-safety-level 
        are passed and returned through the last two args.
        Four thread-safety levels are defined as ``enum: int`` type in the scope of 
        :class:`Env`: ``THREAD_SINGLE``, ``THREAD_FUNNELED``, ``THREAD_SERIALIZED`` and ``THREAD_MULTIPLE```
        (see MPI **Standard** for semantics).

    .. function::   ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const
                    friend ostream & operator<<( ostream &os, const Env &)
        
        ``info()`` displays some basic information of the env instance to ``os``.
        
        :arg fmt_cntl:  Control the display format. 0 for inline information and 1 \
                for a verbose, multiple-line information.
        :return: the argument ``os`` is returned.
        
        The overloaded `<<` operator is equivalent to info() with the default 
        ``fmt_cntl``.
            
    
    .. function::   static void version( int &version, int &subversion )
                    static string library_version()

        ``version()`` returns the library version and subversion by arguments ``version`` and ``subversion``.
        ``library_version()`` returns a string that represents the library version.

    .. function::   int tag_ub() const
                    int host() const
                    int io() const
                    int wtime_is_global() const
                    static string processor_name()

        Query information of the library implementation: upper bound for a tag value, rank of 
        the host process (if none, returns PROC_NULL), rank of process that has I/O facilities 
        (possibly myrank), a boolean value indicate whether clocks are synchronized, and 
        the name of the host machine.

    .. function::   static Comm world() noexcept

        Get the global **World** communicator that is predefined in MPI. New communicators 
        can be created, if neccessary, from this global communicator.


    **Examples:**

    A typical usage of the MPI interface is like the following. Instead of calling *Standard* ``MPI_Init()`` and ``MPI_Finalize()``,
    we just define a variable of type :class:`Env`. This automatically initialize the MPI environment, and at the return of 
    the ``main()``, it automatically finalizes the MPI environment. For example::

        int main(int argc, char *argv[]){    
            HIPP::MPI::Env env;            // Start the MPI environment. 
            
            // Do something using MPI, such as:
            cout << env;                   // Output environment information.

            return 0;                      // Return from main, MPI finalizes.
        }

    may output:

    .. code-block:: text

        HIPP::MPI::Env instance [loc=0x7ffc826431cc, size=4, align=4]
        ----------
        Standard: 3.1
        Runtime Environment (TAG UB=268435455, HOST=None, IO RANK=Any, WTIME GLOBAL=No)
        Processor name: local-linux

    An ``Env`` object provides many useful feature detection methods that allows
    you to get the detail of your MPI environment. For example::
        
        cout << env.processor_name() << '\n'
             << env.host() << endl;

    may outputs as follows, where -1 means rank of Null process:

    .. code-block:: text

        local-linux
        -1