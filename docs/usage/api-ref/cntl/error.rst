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

Error Classes and Error in Applications
-----------------------------------------

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


Predefined Error Classes and Applications
---------------------------------------------

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