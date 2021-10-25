/**
create: Yangyao CHEN, 2020/01/11
    [write   ] 
    ErrH5 - the high-level HDF5 IO exception class.
*/ 

#ifndef _HIPPIO_H5_ERROR_H_
#define _HIPPIO_H5_ERROR_H_
#include "h5_base.h"

namespace HIPP::IO::H5 {

/**
The exception thrown on the error of any HDF5 call.
*/
class ErrH5: public ErrAppH5, public ErrClassDefault {
public:
    /**
    ``errno_t`` is the type of the error number hosted by :class:`ErrH5`.
    ``flat_t`` is the type of the static flag attribute which controls the 
        behavior of :class:`ErrH5`.
    ``err_app_t``, ``err_class_t`` are the parent classes which define the 
        structure of the exception model.
    */
    typedef herr_t errno_t;
    typedef int flag_t;
    typedef ErrAppH5 err_app_t;
    typedef ErrClassDefault err_class_t;
    
    /**
    By default, HDF5 original library does not code the error by a single 
    number, so the ``new_errno`` is of no meaning but kept for consistency
    with other exception classes.
    */
    explicit ErrH5( errno_t new_errno = -1 ) noexcept;

    /**
    what(): returns a short error message under this exception.
    whats(): returns a verbose error mesaage.
    */
    virtual const char *what() const noexcept override;
    virtual string whats() const;

    errno_t get_errno() const noexcept;
    void set_errno( errno_t new_errno ) noexcept;

    /** 
    If r < 0, call throw_(args) with perfect forwarding.
    */
    template<typename ReturnType, typename ...Args>
    static void check( ReturnType r, Args &&... args );

    /** print args and throw ErrH5(-1). */
    template<typename ReturnType, typename ...Args>
    static void throw_( ReturnType r, Args &&... args );

    /** 
    Get ot set the error reporting controlling flag.
    1 for reporting error, 0 for not.
    The flag is thread-specific.
    */
    static flag_t err_cntl_flag() noexcept;
    static void err_cntl_flag( flag_t flag ) noexcept;

private:
    errno_t _errno;
    inline thread_local static flag_t _err_cntl_flag = 1;
};

inline ErrH5::ErrH5( errno_t new_errno ) noexcept 
: _errno(new_errno) { }

inline const char * ErrH5::what() const noexcept { 
    return "HDF5 internal error"; 
}

inline string ErrH5::whats() const
    { return err_app_t::whats() + " | " + 
        err_class_t::whats() + " | Type: " + what(); }

inline ErrH5::errno_t ErrH5::get_errno() const noexcept { 
    return _errno; 
}

inline void ErrH5::set_errno( errno_t new_errno ) noexcept { 
    _errno = new_errno; 
}

template<typename ReturnType, typename ...Args>
void ErrH5::check( ReturnType r, Args &&... args ) {
    if( r < 0 ) throw_( r, std::forward<Args>(args)... );
}

template<typename ReturnType, typename ...Args>
void ErrH5::throw_( ReturnType r, Args &&... args ) {
    if( _err_cntl_flag )
        prt( cerr, std::forward<Args>(args)... );
    throw ErrH5();
}

inline ErrH5::flag_t ErrH5::err_cntl_flag() noexcept { 
    return _err_cntl_flag; 
}

inline void ErrH5::err_cntl_flag( flag_t flag ) noexcept { 
    _err_cntl_flag = flag; 
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_ERROR_H_
