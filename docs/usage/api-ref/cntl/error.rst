Exception Handling
=======================

**Macros** are defined for formated error priting:

.. c:macro::    emFF
                emFLF
                emFPF
                emFLPF
                emFLPFB

    Error macros. Each of them expands to a comma-separated string literals.

    - ``emFF``: file name and current function name.
    - ``emFLF``: file name, line number and current function name.
    - ``emFPF``: file name, "pretty" function name (i.e., a verbose version).
    - ``emFLPF``: file name, line number and 'pretty' function name.
    - ``emFLPFB``: same as ``emFLPF``, with a line break at the end.

    **Example:**

    We detect the number of arguments passed to ``main()``. If it is 
    invalid, we print some information into the stream :var:`HIPP::perr` 
    and throw an instance of :class:`HIPP::ErrLogic` with 
    errno :enumerator:`HIPP::ErrLogic::eLENGTH`::

        #include <hippcntl.h>
        using namespace HIPP;

        int main(int argc, char const *argv[])
        {
            if( argc != 3 ){
                perr << emFLPFB, 
                    "  -  invalid no. of arguments.  \n",
                    "  -  expected 3, got ", argc, endl;
                throw ErrLogic(ErrLogic::eLENGTH);
            }
            return 0;
        }

    Invoking this program with no argument, we get the message:

    .. code-block:: text 

        [ file ] e_error.cpp
        [ line ] 7
        [ function ] int main(int, const char**)
        -  invalid no. of arguments.  
        -  expected 3, got 1
        terminate called after throwing an instance of 'HIPP::ErrLogic'
        what():  eLENGTH
        Aborted


The following variables, functions and classes are all defined within the namespace ``HIPP``.

.. namespace:: HIPP 

.. _api-cntl-exception-layers:

Overview of the Exception Layers
------------------------------------

One difficulty in the combination of HPC libraries is how to deal with their error/exception systems.
In modern C++, exceptions are handled by try-catch clauses. But in libraries writen in C, exceptions 
are usually hinted by the returned error code.

HIPP defines an unified model for exceptions. An exception in HIPP is represented by a C++ struct, 
which has an "application" and a "class". The "application" indicates which library causes the 
error, and the "class" indicates the type of error. 

The "application" and "class" information of an exception struct is encapsulated in 
two parent structs, i.e., each exception struct has a parent struct which represents its 
"application", and has a parent struct which represents its "class".

For example, the struct :class:`MPI::ErrMPI` represents the exceptions thrown on calling of the 
underlying MPI library. Hence, its "application" is :class:`ErrAppMPI` and its "class" 
is :class:`ErrClassDefault` ("default" means the error happens in the underlying MPI library,
not in the high-level wrappers).

Once the exception struct is chosen, the detail reason of the exception is represented 
by a member integer called error number or "errno".
In the :class:`MPI::ErrMPI`, the errno is just the retuned value of the underlying MPI library,
so that its meaning does not change when switching to the high-level wrappers.

All the exception "application" structs are derived from :class:`ErrAPP`, and a member 
integer (also called errno) is used to distinguish different applications. Such a design
enables the users to capture all exceptions by the common parent struct :class:`ErrApp`,
and dynamically check the "application" by the errno.
For the same logic, all the exception "class" structs are derined from :class:`ErrClass`, with 
a member integer distinguishing different error types.

At the root, all the exception structs have a common ancestor ``std::exception``, which 
is a typical strategy in designing C++ exception layers.

.. graphviz::
    :class: inherit-graph
    
    digraph ExceptionLayers {
        
        bgcolor="#ffffff00";
        label="Inheritance Diagram";
        node [font="helvetica", shape="box", fontsize="9", height="0.2", width="0.4"];
        edge [arrowsize="0.5", dir="back"];
        rankdir="LR";
        nodesep="0.05";

        a [label="std::exception"];

        a2 [label="ErrApp"];
        a1 [label="ErrClass"];
        
        a21 [label="ErrAppDefault"];
        a22 [label="ErrAppUnknown"];
        a23 [label="ErrAppSystem"];
        a24 [label="ErrAppMPI"];
        a25 [label="ErrAppOpenMP"];
        a26 [label="ErrAppH5"];
        a27 [label="ErrAppGSL"];
        a28 [label="ErrAppPy"];
        
        a11 [label="ErrClassDefault"];
        a12 [label="ErrClassUnknown"];
        a13 [label="ErrClassRuntime"];
        a14 [label="ErrClassLogic"];
        a15 [label="ErrClassMemory"];
        a16 [label="ErrClassCast"];
        a17 [label="ErrClassIO"];

        asystem [label="ErrSystem"];
        aruntime [label="ErrRuntime"];
        alogic [label="ErrLogic"];
        agsl [label="NUMERICAL::ErrGSL"];
        ampi [label="MPI::ErrMPI"];
        ah5 [label="IO::ErrH5"];
        
        subgraph subeclass {
            rank="same";
            edge [style="invisible", dir="none"];
            a2->a1;
        }
        subgraph subeappsub {
            rank="same";
            edge [style="invisible", dir="none"];
            a21->a22->a23->a24->a25->a26->a27->a28->a11->a12->a13->a14->a15->a16->a17;
        }

        a -> a2;
        a -> a1;

        a2-> a21;
        a2 -> a22;
        a2 -> a23;
        a2 -> a24;
        a2 -> a25;
        a2 -> a26;
        a2 -> a27;
        a2 -> a28;
        
        a1 -> a11;
        a1 -> a12;
        a1 -> a13;
        a1 -> a14;
        a1 -> a15;
        a1 -> a16;
        a1 -> a17;
        
        edge [color="red"];
        a23 -> asystem;
        a11 -> asystem;

        edge [color="blue"];
        a24 -> ampi;
        a11 -> ampi;

        edge [color="green"];
        a26 -> ah5;
        a11 -> ah5;

        edge [color="purple"];
        a27 -> agsl;
        a11 -> agsl;

        edge [color="black"];
        a21 -> aruntime;
        a13 -> aruntime;

        edge [color="orange"];
        a21 -> alogic;
        a14 -> alogic;
    }


The following example shows how the throw an exception and catch it.
Member function :func:`whats() <ErrLogic::whats>` is used to get the detail error information,
:func:`get_errno() <ErrLogic::get_errno>` in the exception struct and its two parent structs are used to 
get the error numbers::

    try {
        throw ErrLogic(ErrLogic::eLENGTH);
    }catch( const ErrLogic &e ){
        /* priting the detail of error */
        pout << e.whats(), endl;

        /* retrieve its errno for application, errno for class, and errno */
        pout << e.ErrApp::get_errno(), ", ", 
                e.ErrClass::get_errno(), ", ",
                e.get_errno(), endl;
    }

The output is 

.. code-block:: text

    Application: default | Class: logic error | Type: eLENGTH
    1, 4, 5

Here, errno 1 represents the default "application", errno 4 represents 
the logic error "class", and errno 5 represents the length error.

Applications, Classes and Exceptions
-----------------------------------------

.. class:: ErrApp: virtual public std::exception

    ``ErrApp`` is the base class of all "Error in Application" classes. 
    Each of the subclass represents errors in a specific application, 
    e.g., MPI. OpenMP, HDF5, etc. 

    ``ErrApp`` uses a single member, called "errno", typed :type:`errno_t` to 
    represent the application to which the error belongs.
    The subclasses have no extra member, and each of them is pinned
    to a fixed value of errno. Such a design enables the user to
    
    - catch all errors with ``ErrApp``, and dynamically check its host application
      by using the errno.
    - or, catch errors in a specific application with one of the subclass of ``ErrAPP``.

    ``ErrAPP`` can be copy/move constructed/assigned.
    
    .. type:: std::uint16_t errno_t

        Type of the errno.

    .. enum:: ERRNOS: errno_t

        .. enumerator:: \  
            eDEFAULT=1
            eUNKNOWN=2
            eSYSTEM=3
            eMPI=4
            eOPENMP=5
            eH5=6
            eGSL=7
            ePY=8

        Predefined errnos for different applications.

    .. function:: ErrApp( errno_t new_errno = 1 ) noexcept

        Construct the instance by providing an errno - possibly used in the ``throw`` statement.
        The errno can be any value defined as the enumerator typed :enum:`ERRNOS`.

        Usually, we do not throw :class:`ErrApp`, but we throw a subclass of it. For example, we 
        throw :class:`ErrAppMPI` to hint a general error in the Message Passing Interface 
        (which is derived from :class:`ErrApp`) when the type of error does not matter.
        Or we throw :class:`HIPP::MPI::ErrMPI` to hint the implementation-defined error in MPI.
        (which is derived from :class:`ErrAppMPI` and :class:`ErrClassDefault`).

    .. function:: \
        virtual const char *what() const noexcept override
        virtual string whats() const
        static size_t errmsg_maxsize() noexcept
        static errno_t errmsg_maxno() noexcept

        Retrieve the detail of the exception instance. 

        ``what()`` gives a short report, which is short and quick, with no obvious 
        overhead (because the content returned by ``what()`` is stored statically).
        The maximal length of the error message returned by ``what()`` can be 
        obtained by ``errmsg_maxsize()`` (the NULL-terminate is not counted).
        
        ``whats()`` reports more details, but with larger overhead because the error 
        message is dynamically constructed.

        ``errmsg_maxno()`` gives the maximal errno that can be thrown with 
        :class:`ErrApp`.

    .. function:: \
        errno_t get_errno() const noexcept
        void set_errno( errno_t new_errno ) noexcept

        Get the current errno or set the errno in the instance. ``new_errno`` can be 
        any of the enumerators defined in :enum:`ERRNOS`.



.. class:: \
    ErrAppDefault: public ErrApp
    ErrAppUnknown: public ErrApp
    ErrAppSystem: public ErrApp
    ErrAppMPI: public ErrApp
    ErrAppOpenMP: public ErrApp
    ErrAppH5: public ErrApp
    ErrAppGSL: public ErrApp
    ErrAppPy: public ErrApp

    Each of these classes defines errors in a specific application.
    
    They all have default constructors.

.. class::  ErrClass: virtual public std::exception

    ``ErrClass`` is the base class of all "Error Class" classes. Each of the subclass
    represents a type of error, e.g., runtime error, logic error, cast error, etc. 

    ``ErrClass`` uses a single member typed :type:`errno_t` to represent which 
    type of error it is, which we called "errno" or error number. 
    The subclasses have no extra member, and each of them is pinned
    to a fixed value of errno. Such a design enables the user to
    
    - catch all errors with ``ErrClass``, and dynamically check its type 
      by using the errno.
    - or, catch a specific type of error with one of its subclass.

    ``ErrClass`` can be copy/move constructed/assigned.

    .. type:: std::uint16_t errno_t

        Type of the errno.

    .. enum:: ERRNOS : errno_t

        .. enumerator:: \
            eDEFAULT=1
            eUNKNOWN=2
            eRUNTIME=3
            eLOGIC=4
            eMEMORY=5 
            eCAST=6
            eIO=7

        Predefined errnos for different types of error.

    .. function:: \
        ErrClass( errno_t new_errno = 1 ) noexcept

        Construct the instance by providing an errno - possibly used in the ``throw`` statement.
        The errno can be any value defined as the enumerator typed :enum:`ERRNOS`.

        Usually, we do not throw :class:`ErrClass`, but we throw a subclass of it. For example, we 
        throw :class:`ErrClassLogic` to hint a general logic error (which is derived from :class:`ErrLogic`) 
        when the application does not matter. 
        Or we throw :class:`ErrLogic` to hint the logic error in the default application 
        (which is derived from :class:`ErrAppDefault` and :class:`ErrClassLogic`).
    
    .. function:: \ 
        virtual const char *what()const noexcept override
        virtual string whats() const
        static size_t errmsg_maxsize() noexcept
        static errno_t errmsg_maxno() noexcept

        Retrieve the detail of the exception instance. 

        ``what()`` gives a short report, which is short and quick, with no obvious 
        overhead (because the content returned by ``what()`` is stored statically).
        The maximal length of the error message returned by ``what()`` can be 
        obtained by ``errmsg_maxsize()`` (the NULL-terminate is not counted).
        
        ``whats()`` reports more details, but with larger overhead because the error 
        message is dynamically constructed.

        ``errmsg_maxno()`` gives the maximal errno that can be thrown with 
        :class:`ErrClass`.
    
    .. function:: \    
        errno_t get_errno() const noexcept
        void set_errno( errno_t new_errno ) noexcept

        Get the current errno or set the errno in the instance. ``new_errno`` can be 
        any of the enumerators defined in :enum:`ERRNOS`.

.. class:: \
    ErrClassDefault: public ErrClass
    ErrClassUnknown: public ErrClass
    ErrClassRuntime: public ErrClass
    ErrClassLogic: public ErrClass
    ErrClassMemory: public ErrClass
    ErrClassCast: public ErrClass
    ErrClassIO: public ErrClass

    Each of these classes defines a specific type of error. 
    
    They all have default constructors.

    **Examples:**

    We throw exceptions of different classes. They can be catched by the common 
    parent class :class:`ErrClass`, with the errno hinting the real error class::

        try{
            throw ErrClassIO();
        }catch( const ErrClass &e ){
            perr << e.whats(), '\n',
                "errno=", e.get_errno(), endl;
        }

        try{
            throw ErrClassLogic();
        }catch( const ErrClass &e ){
            perr << e.whats(), '\n',
                "errno=", e.get_errno(), endl;
        }

    The output is:

    .. code-block::

        Class: IO error
        errno=7
        Class: logic error
        errno=4

.. class:: ErrSystem: public ErrAppSystem, ErrClassDefault

    Exception that is thrown on a failed system call.

    .. type::   int errno_t
                int flag_t

        Types of errno and error controlling flag.

    .. function:: ErrType( errno_t new_errno ) noexcept

        Constructor. 

        Initialize the exception instance with an errno. The errno should be 
        a valid return value of the underlying operating system.

    .. function:: \
        virtual const char *what() const noexcept override
        virtual string whats() const override
        static size_t errmsg_maxsize() noexcept 
        static errno_t errmsg_maxno() noexcept 

        Get the exception details. ``what()`` gives a brief description. ``whats()`` gives 
        a more detailed description (with larger overhead). 
        
        The maximal length 
        of the C-style string retuned by ``what()`` can be obtained by ``errmsg_maxsize()`` 
        (NULL-terminate is not counted).  The maximal errno can be obtained by ``errmsg_maxno()``.

    .. function:: \
        errno_t get_errno() const noexcept
        void set_errno( errno_t new_errno ) noexcept 

        Retrieve the current errno or set the errno.

    .. function:: \
        static flag_t err_cntl_flag() noexcept
        static void err_cntl_flag( flag_t flag ) noexcept

        Retrieve or set the thread-local, static error controlling flag. By default
        the flag is ``1`` - error message is printed to the standard error stream 
        on the throwing of each exception. If the flag is set to ``0``, the exception
        throwing is silent. 

    .. function:: \
        template<typename ...Args> static void check( errno_t new_errno, Args &&... args )
        template<typename ...Args> static void throw_( errno_t new_errno, Args &&... args )
        template<typename ...Args> static void abort( errno_t e, Args &&... args )

        ``check()`` checks if ``new_errno`` indicates an error. If it does, ``throw_(new_errno, args...)``
        is invoked.

        ``throw_()`` prints error message ``args...`` into standard error stream (if the error controlling flag is not disabled),
        and throws an exception ``ErrSystem(new_errno)``.

        ``abort()`` always prints the error message ``args...`` into standard error stream, and abort 
        the program with exit code ``e``.

.. class:: ErrRuntime: public ErrAppDefault, ErrClassRuntime

    Exception that is thrown on a run-time error.

    .. type::   int errno_t
                int flag_t

        Types of errno and error controlling flag.

    .. enum:: ERRNOS: errno_t

        .. enumerator:: \  
            eDEFAULT=1
            eOVERFLOW=2
            eUNDERFLOW=3
            eRANGE=4
            eSTRCONSTRUCT=5

        Predefined errnos for different run-time errors.

    .. function:: ErrType( errno_t new_errno ) noexcept

        Constructor. 

        Initialize the exception instance with an errno. 
        The errno can be any value defined as the enumerator typed :enum:`ERRNOS`.

    .. function:: \
        virtual const char *what() const noexcept override
        virtual string whats() const override
        static size_t errmsg_maxsize() noexcept 
        static errno_t errmsg_maxno() noexcept 

        Get the exception details. ``what()`` gives a brief description. ``whats()`` gives 
        a more detailed description (with larger overhead). 
        
        The maximal length 
        of the C-style string retuned by ``what()`` can be obtained by ``errmsg_maxsize()`` 
        (NULL-terminate is not counted).  The maximal errno can be obtained by ``errmsg_maxno()``.

    .. function:: \
        errno_t get_errno() const noexcept
        void set_errno( errno_t new_errno ) noexcept 

        Retrieve the current errno or set the errno.

    .. function:: \
        static flag_t err_cntl_flag() noexcept
        static void err_cntl_flag( flag_t flag ) noexcept

        Retrieve or set the thread-local, static error controlling flag. By default
        the flag is ``1`` - error message is printed to the standard error stream 
        on the throwing of each exception. If the flag is set to ``0``, the exception
        throwing is silent. 

    .. function:: \
        template<typename ...Args> static void throw_( errno_t new_errno, Args &&... args )
        template<typename ...Args> static void abort( errno_t e, Args &&... args )

        ``throw_()`` prints error message ``args...`` into standard error stream (if the error controlling flag is not disabled),
        and throws an exception ``ErrRuntime(new_errno)``.

        ``abort()`` always prints the error message ``args...`` into standard error stream, and abort 
        the program with exit code ``e``.

.. class:: ErrLogic: public ErrAppDefault, ErrClassLogic

    Exception that is thrown on a logic error.

    .. type::   int errno_t
                int flag_t

        Types of errno and error controlling flag.

    .. enum:: ERRNOS: errno_t

        .. enumerator:: \  
            eDEFAULT=1
            eDOMAIN=2
            eINVALIDARG=3
            eOUTOFRANGE=4
            eLENGTH=5
            eRECIPE_INCOMPLETE=6
            eRECIPE_INCONSISTENT=7

        Predefined errnos for different logic errors.

    .. function:: ErrType( errno_t new_errno ) noexcept

        Constructor. 

        Initialize the exception instance with an errno. 
        The errno can be any value defined as the enumerator typed :enum:`ERRNOS`.

    .. function:: \
        virtual const char *what() const noexcept override
        virtual string whats() const override
        static size_t errmsg_maxsize() noexcept 
        static errno_t errmsg_maxno() noexcept 

        Get the exception details. ``what()`` gives a brief description. ``whats()`` gives 
        a more detailed description (with larger overhead). 
        
        The maximal length 
        of the C-style string retuned by ``what()`` can be obtained by ``errmsg_maxsize()`` 
        (NULL-terminate is not counted).  The maximal errno can be obtained by ``errmsg_maxno()``.

    .. function:: \
        errno_t get_errno() const noexcept
        void set_errno( errno_t new_errno ) noexcept 

        Retrieve the current errno or set the errno.

    .. function:: \
        static flag_t err_cntl_flag() noexcept
        static void err_cntl_flag( flag_t flag ) noexcept

        Retrieve or set the thread-local, static error controlling flag. By default
        the flag is ``1`` - error message is printed to the standard error stream 
        on the throwing of each exception. If the flag is set to ``0``, the exception
        throwing is silent. 

    .. function:: \
        template<typename ...Args> static void throw_( errno_t new_errno, Args &&... args )
        template<typename ...Args> static void abort( errno_t e, Args &&... args )

        ``throw_()`` prints error message ``args...`` into standard error stream (if the error controlling flag is not disabled),
        and throws an exception ``ErrLogic(new_errno)``.

        ``abort()`` always prints the error message ``args...`` into standard error stream, and abort 
        the program with exit code ``e``.
    

