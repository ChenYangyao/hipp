/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] 
 *          ErrH5 - the HDF5 wrapped high-level interface exception 
 *                  class.
 */ 

#ifndef _HIPPIO_H5_ERROR_H_
#define _HIPPIO_H5_ERROR_H_
#include "h5_base.h"
namespace HIPP{
namespace IO{

class ErrAppH5: public ErrApp { public: ErrAppH5() noexcept: ErrApp(6){ } };

/**
 * the HDF5 wrapped high-level interface exception class.
 */
class ErrH5: public ErrAppH5, public ErrClassDefault{
public:
    typedef herr_t errno_t;
    typedef int flag_t;
    typedef ErrAppH5 err_app_t;
    typedef ErrClassDefault err_class_t;
    
    /**
     * by default, HDF5 oringinal library does not code the error by a single 
     * number, so the `new_errno` is of no meaning.
     */
    ErrH5( errno_t new_errno = -1 ) noexcept 
        : _errno(new_errno){ }

    virtual const char *what() const noexcept override
        { return "HDF5 internal error"; }
    virtual string whats() const
        { return err_app_t::whats() + " | " + 
            err_class_t::whats() + " | Type: " + what(); }

    errno_t get_errno() const noexcept
        { return _errno; }
    void set_errno( errno_t new_errno ) noexcept
        { _errno = new_errno; }

    /**
     * check(r, info) test if `r` < 0. If so call throw_(r, info).
     * throw_(r, info) printing some information `info` and always throws an 
     * `ErrH5(r)`, the printing is controlled by err_cntl_flag().
     */
    template<typename ReturnType, typename ...Args>
    static void check( ReturnType r, Args &&... args ){
        if( r < 0 ) throw_( r, std::forward<Args>(args)... );
    }
    template<typename ReturnType, typename ...Args>
    static void throw_( ReturnType r, Args &&... args ){
        if( _err_cntl_flag )
            prt( cerr, std::forward<Args>(args)... );
        throw ErrH5();
    }

    /**
     * the error-reporing controlling flag for our high-level wrapper.
     * If set to 0, no error is printed on an exception. Otherwise some
     * information is printed to `std::cerr` before an thrown exception.
     * 
     * err_cntl_flag() retrive the current flag, err_cntl_flag(flag) set the
     * flag.
     */
    static flag_t err_cntl_flag() noexcept
        { return _err_cntl_flag; }
    static void err_cntl_flag( flag_t flag ) noexcept
        { _err_cntl_flag = flag; }
private:
    errno_t _errno;
    thread_local static flag_t _err_cntl_flag;
};

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_ERROR_H_
