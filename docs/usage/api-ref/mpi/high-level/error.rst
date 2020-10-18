High-level API - Error Handling
===============================================================

.. namespace:: HIPP::MPI

.. class::  ErrMPI: public HIPP::ErrType<HIPP::ErrAppMPI, HIPP::ErrClassDefault>

    ``ErrMPI`` is the exception which is thrown when an internal call of **Standard** MPI API
    failed. 
    
    You may capture it with property exception declaration in the ``catch``
    clause. This exception is eventually rooted at ``std::exception``, so you may use 
    ``std::exception`` declaration to capture all exceptions.

    .. type::   ErrType<ErrAppMPI, ErrClassDefault> err_type_t
                err_type_t::ErrType

    .. function::   virtual const char *what() const noexcept override
                    virtual string whats() const override

        This two functions report the detail of the exception instance.
        ``what()`` only roughly indicates the error at application level,
        but ``whats()`` returns more detailed information, including the info 
        associated with thrown error number.

    .. function::   static size_t errmsg_maxsize() noexcept
                    static errno_t last_errno() noexcept
                    static errno_t last_usederrno()
        
        Defined by MPI **Standard**, the max length an original 
        MPI error string (including the space for ``'\0'``), the implementation-
        defined last error number (inclusive), and the last error number if
        user-defined error class/number is added.
    
        The MPI **Standard** does not require the error number to be dense. 
        That is, not all error number below ``last_usederrorno()`` is valid.
    
    .. function::    template<typename ...Args>\
                        static void check( errno_t e, Args &&... args )
                    template<typename ...Args>\
                        static void throw_( errno_t e, Args &&... args )
                template<typename ...Args>\
                        static void print_err( Args &&... args )
                template<typename ...Args>\
                        static void abort( errno_t e, Args &&... args )
        
        Convinient function for detect, report errors and throw exceptions. These
        function all print the detail location of the error, as well as the information 
        provided by the caller through ``args``. The prints can be turned on/off by 
        methods :func:`ErrMPI::err_cntl_flag` with arguments 1/0.
        
        ``check()`` examine the reuturned value of a MPI **Standard** call. If ``e`` is 
        a number that indicates an error, it prints args into ``cerr``, and 
        throw an :class:`ErrMPI` exception with error number ``e``. ``throw()`` does 
        not perform examine, just prints args and throw.
        ``print_err()`` does nothing but print args.

        ``abort()`` always prints and always abort the program. 
        By default, only the un-recoverable error calls ``abort()`` in the high-level
        interface. An error when destructing a MPI internal object
        is usually considered un-recoverable, and causes the high-level interface 
        abort the program.

    **Examples:**

    Exception handling is an important part for any well-developped application. A typical
    exception handling process in C++ is to catch the exception, check its info/status, 
    perform actions to recover. If the exception is too severe to be handled, just rethrow 
    it or abort the whole program. The following code sample illustrates this process::

        int *out_buff = nullptr;        // A must-fail operation, read from 0-address.
        int dest = 1, tag = 0;
        try{
            comm.send(dest, tag, out_buff, 1, "int");
        }catch( HIPP::ErrAppMPI &e ){   // catch the exception
            // Now you handle the error. Here we just print its content and rethrow it.
            auto error_number = e.get_errno();
            HIPP::MPI::ErrMPI::print_err(
                emFLPFB, "rank ", comm.rank(), ", out buff ", out_buff, 
                ", error no ", error_number, '\n');
            cout << e.whats() << endl;
            throw;      
        }

    where we send a message starting at a protected address, to the rank-1 process 
    in the communicator ``comm``. 
    This call fails on most modern platforms,
    which throws an exception of type ``ErrMPI`` thrown. Here we catch
    it by exception declaration of one of its parent class 
    :class:`HIPP::ErrAppMPI`. We can get the error number by :func:`ErrMPI::get_errno` 
    method and use it to handle the error, or get the detail error information by 
    :func:`ErrMPI::whats` method.
    Here we just print some information using :func:`ErrMPI::print_err`, and print the 
    exception detail output by :func:`ErrMPI::whats`, and rethrow the 
    exception, which eventually aborts the program.
    Note that The :c:macro:`emFLPFB` is a 
    useful preprocess-macro which gives the file, line number and function signature.

    The output is like the following, 
    where we see the error stack is printed: the first block is printed by 
    the internal intermediate HIPP API call, the second is by the user, and 
    the third is returned by :func:`whats` which depends on your MPI **Standard**
    implementation behind the HIPP high-level API.

    .. code-block:: text 

        [ file ] /path/to/include/mpi/mpi_raw_comm.h
        [ line ] 208
        [ function ] void HIPP::MPI::_Comm::send(const void*, int, MPI_Datatype, int, int) const

        [ file ] /path/to/example.cpp
        [ line ] 19
        [ function ] int main(int, const char**)
        rank 0, out buff 0, error no 4

        Application: MPI | Class: default | Type: Invalid buffer pointer, error stack:
        MPI_Send(174): MPI_Send(buf=(nil), count=1, MPI_INT, dest=1, tag=0, MPI_COMM_WORLD) failed
        MPI_Send(119): Null buffer pointer

