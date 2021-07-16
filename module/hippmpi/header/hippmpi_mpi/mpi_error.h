/**
 * creat: Yangyao CHEN, 2020/01/15
 *      [write   ] ErrMPI - the high-level exception for MPI module
 */ 

#ifndef _HIPPMPI_MPI_ERROR_H_
#define _HIPPMPI_MPI_ERROR_H_
#include "mpi_base.h"
namespace HIPP{
namespace MPI{

/**
 * the MPI high-level interface exception class.
 * 
 * By default, the high-level interface turns off the MPI's default error
 * handler and check the error after each MPI call returned.
 * This type of exception is thrown if a call to MPI original function returns
 * an error, and the error number is recored in the intance of this class.
 */
class ErrMPI: public ErrType<ErrAppMPI, ErrClassDefault>{
public:
    typedef ErrType<ErrAppMPI, ErrClassDefault> err_type_t;
    using err_type_t::ErrType;


    /**
     * report the detail exception
     * what():  only roughly indicating the error at application level.
     * whats(): return the detail information, including the info associated 
     *          witherror no.
     */
    virtual const char *what() const noexcept override
        { return "MPI internal error"; }
    virtual string whats() const override{ 
        vector<char> estr( errmsg_maxsize(), '\0' );
        int reslen;
        check(
            MPI_Error_string( _errno, estr.data(), &reslen),
            emFLPFB );
        return err_type_t::whats() + " | Type: " + (const char *)estr.data(); 
    }

    /**
     * followings are defined by MPI standard, the max length an original 
     * MPI error string (including the space for '\0'), the implementation-
     * defined last error number (inclusive), and the last error number if
     * user-defined error class/number is added.
     *
     * The MPI standard does not require the error number to be dense. That is,
     * not all error number below last_usederrorno() is valid.
     */
    static size_t errmsg_maxsize() noexcept
        { return static_cast<size_t>(MPI_MAX_ERROR_STRING); }
    static errno_t last_errno() noexcept
        { return MPI_ERR_LASTCODE; }
    static errno_t last_usederrno(){
        int *val=NULL, flag=0;
        check( 
            MPI_Comm_get_attr( MPI_COMM_WORLD, MPI_LASTUSEDCODE, &val, &flag ), 
            emFLPFB );
        return *val;
    }

    /**
     * check whether an MPI original call's return is an error. If it is, call
     * throw_().
     */
    template<typename ...Args>
    static void check( errno_t e, Args &&... args ){
        if( e != MPI_SUCCESS )
            throw_( e, std::forward<Args>(args)... );
    }
    /**
     * Print info `args` to std::cerr, and directly throw an error with error 
     * number `e`.
     * The priting can be turned off by calling err_cntl_flag(0), or turned on 
     * by calling err_cntl_flag(1).
     * 
     * print_err() - just print, do not throw.
     */ 
    template<typename ...Args>
    static void throw_( errno_t e, Args &&... args ){
        print_err(std::forward<Args>(args)...);
        throw ErrMPI(e);
    }
    template<typename ...Args>
    static void print_err( Args &&... args ){
        if( _err_cntl_flag )
            prints(std::forward<Args>(args)... );
    }
    /**
     * directly abort the program.
     * By default, only the un-recoverable error calls abort() in the high-level
     * interfalce. Typically an error when deconstructing a MPI internal object
     * is un-recoverable, and causes the high-level interface abort the program.
     */
    template<typename ...Args>
    static void abort( errno_t e, Args &&... args ){
        prints(std::forward<Args>(args)... );
        MPI_Abort( MPI_COMM_WORLD, e );
    }
};

    
} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_ERROR_H_