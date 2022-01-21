/**
create: Yangyao CHEN, 2019/12/28
    [write   ] 
    ErrType:   parent class for error-number based exception types.
    ErrSystem, ErrRuntime, ErrLogic: concrete types for system-related, 
        run-time, and logic exceptions, respectively.
*/ 

#ifndef _HIPPCNTL_ERROR_TYPE_H_
#define _HIPPCNTL_ERROR_TYPE_H_
#include "error_application.h"
#include "error_class.h"
#include "../hippcntl_stream/stream_pretty.h"

namespace HIPP{

/**
Parent class for error-number based exceptions.
Should not be used directly.
*/
template<typename ErrAppT, typename ErrClassT>
class ErrType: public ErrAppT, public ErrClassT {
public:
    typedef int errno_t;
    typedef int flag_t;
    typedef ErrAppT err_app_t;
    typedef ErrClassT err_class_t;

    ErrType(errno_t new_errno) noexcept;

    virtual const char *what() const noexcept override;
    virtual string whats() const override;

    errno_t get_errno() const noexcept;
    void set_errno(errno_t new_errno) noexcept;

    /**
    The no-argument version retrieve the current error-controlling flag, the
    single-argument version set the error-controlling flag.
    
    Error-control flag is a thread-local, static variable controlling the
    exception reporting when raised.
    The current possible value for this flag is:
    0: silent before throw the exception.
    1: print error information to std::cerr and then throw the 
       exception.
    Other values are treated as 1 in the current implementation.
    */
    static flag_t err_cntl_flag() noexcept;
    static void err_cntl_flag(flag_t flag) noexcept;
protected:
    errno_t _errno;
    inline thread_local static flag_t _err_cntl_flag = 1;

    template<typename Arg, typename ...Args>
    static void prints(Arg &&arg, Args && ...args) noexcept;
    static void prints() noexcept;
};

/**
System call exceptions.
Error NO.s are consistent with system call error numbers.
*/
class ErrSystem: public ErrType<ErrAppSystem, ErrClassDefault>{
public:
    typedef ErrType<ErrAppSystem, ErrClassDefault> err_type_t;

    /**
    Constructor.
    @new_errorno: system call error number.
    
    An error number should be a positive number, however, 0 is also 
    acceptable.
    Maximum error number (included) can be obtained by errmsg_maxno().
    Error number can be retrived by get_errno(), or reset by set_error().
    */
    using err_type_t::ErrType;

    /**
    Return information of the exception instance.
    what(): return a static C-style string indicating the type of system 
        call error.
        Maximum string length can be obtained by errmsg_maxsize().
    whats(): return a string instead. Contain also the information from
        parent classes.
    */
    virtual const char *what() const noexcept override;
    virtual string whats() const override;

    static size_t errmsg_maxsize() noexcept;
    static errno_t errmsg_maxno() noexcept;
    
    /**
    Check whether a system call return value has an error 
    (i.e. `new_errno` > 0).
    
    If so, print `args...` into std::cerr, and raise an ErrSystem with 
    corresponding `new_errorno`.
    */
    template<typename ...Args>
    static void check(errno_t new_errno, Args &&... args);
    template<typename ...Args>
    static void throw_(errno_t new_errno, Args &&... args);
    template<typename ...Args>
    static void abort(errno_t e, Args &&... args);
private:
    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 132;
    static const char * _errmsgs[];
};

/**
Runtime errors
*/
class ErrRuntime: public ErrType<ErrAppDefault, ErrClassRuntime>{
public:
    typedef ErrType<ErrAppDefault, ErrClassRuntime> err_type_t;

    enum ERRNOS: errno_t {
        eDEFAULT=1, eOVERFLOW=2, eUNDERFLOW=3, eRANGE=4, eSTRCONSTRUCT=5
    };

    /**
    Constructors
    @new_errorno: error number.
    
    Valid error numbers are defiend in enumerate type ERRNOS.
    An error number should be a positive number, however, 0 is also 
    acceptable.
    Maximum error number (included) can be obtained by errmsg_maxno().
    */
    using err_type_t::ErrType;
    
    /**
    Return information of the exception instance.
    what(): return a static C-style string indicating the type of runtime
        call error.
        Maximum string length can be obtained by errmsg_maxsize().
    whats(): return a string instead. Contain also the information from
        parent classes.
    */
    virtual const char *what() const noexcept override;
    virtual string whats() const override;

    static size_t errmsg_maxsize() noexcept;
    static errno_t errmsg_maxno() noexcept;

    template<typename ...Args>
    static void throw_(errno_t new_errno, Args &&... args);
    template<typename ...Args>
    static void abort(errno_t e, Args &&... args);
private:
    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 5;
    static const char * _errmsgs[];
};


/**
Logic errors
*/
class ErrLogic: public ErrType<ErrAppDefault, ErrClassLogic>{
public:
    typedef ErrType<ErrAppDefault, ErrClassLogic> err_type_t;
    
    enum ERRNOS: errno_t {
        eDEFAULT=1, eDOMAIN=2, eINVALIDARG=3, eOUTOFRANGE=4, eLENGTH=5,
        eRECIPE_INCOMPLETE=6, eRECIPE_INCONSISTENT=7
    };
    
    /**
    Constructors
    @new_errorno: error number.
    
    Valid error numbers are defiend in enumerate type ERRNOS.
    An error number should be a positive number, however, 0 is also 
    acceptable.
    Maximum error number (included) can be obtained by errmsg_maxno().
    */
    using err_type_t::ErrType;

    /**
    return information of the exception instance.
    what(): return a static C-style string indicating the type of logic 
        call error.
        Maximum string length can be obtained by errmsg_maxsize().
    whats(): return a string instead. Contain also the information from
        parent classes.
    */
    virtual const char *what() const noexcept override;
    virtual string whats() const override;

    static size_t errmsg_maxsize() noexcept;
    static errno_t errmsg_maxno() noexcept;

    template<typename ...Args>
    static void throw_(errno_t new_errno, Args &&... args);
    template<typename ...Args>
    static void abort(errno_t e, Args &&... args);
private:
    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 7;
    static const char * _errmsgs[];
};


/**
Implementation of above classes.
*/

#define _HIPP_TEMPHD template<typename ErrAppT, typename ErrClassT>
#define _HIPP_TEMPARG <ErrAppT, ErrClassT>
#define _HIPP_TEMPCLS ErrType _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
ErrType(errno_t new_errno) noexcept
: _errno( new_errno ) 
{}

_HIPP_TEMPRET
what() const noexcept -> const char * { 
    return ""; 
}

_HIPP_TEMPRET
whats() const -> string 
    { return ErrAppT::whats() + " | " + ErrClassT::whats(); }

_HIPP_TEMPRET
get_errno() const noexcept -> errno_t { 
    return _errno; 
}

_HIPP_TEMPRET
set_errno(errno_t new_errno) noexcept -> void { 
    _errno = new_errno; 
}

_HIPP_TEMPRET
err_cntl_flag() noexcept -> flag_t { 
    return _err_cntl_flag; 
}

_HIPP_TEMPRET
err_cntl_flag(flag_t flag) noexcept -> void { 
    _err_cntl_flag = flag; 
}

_HIPP_TEMPHD
template<typename Arg, typename ...Args>
void _HIPP_TEMPCLS::prints(Arg &&arg, Args && ...args) noexcept { 
    constexpr size_t n_args = sizeof...(Args);
    if constexpr ( n_args >= 1 ) {
        perr << std::forward<Arg>(arg); 
        prints( std::forward<Args>(args)... ); 
    } else {
        perr << std::forward<Arg>(arg), endl;
    }
}

_HIPP_TEMPRET
prints() noexcept -> void
{}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

inline const char * ErrSystem::what() const noexcept { 
    return ( (_errno > _errmsg_maxno) | (_errno < 0) ) ? 
        _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; 
}

inline string ErrSystem::whats() const { 
    return err_type_t::whats() + " | Type: " + what(); 
}

inline size_t ErrSystem::errmsg_maxsize() noexcept { 
    return _errmsg_maxsize; 
}

inline auto ErrSystem::errmsg_maxno() noexcept -> errno_t { 
    return _errmsg_maxno; 
}

template<typename ...Args>
void ErrSystem::check(errno_t new_errno, Args &&... args) { 
    if( new_errno ) 
        throw_(new_errno, std::forward<Args>(args)...); 
}

template<typename ...Args>
void ErrSystem::throw_(errno_t new_errno, Args &&... args) {
    if( _err_cntl_flag )
        prints( std::forward<Args>(args)... );
    throw ErrSystem( new_errno ); 
}

template<typename ...Args>
void ErrSystem::abort( errno_t e, Args &&... args ) {
    prints( std::forward<Args>(args)... );
    exit( e );
}

inline const char * ErrRuntime::what() const noexcept { 
    return ( (_errno > _errmsg_maxno) | (_errno < 0) ) ? 
        _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; 
}

inline string ErrRuntime::whats() const { 
    return err_type_t::whats() + " | Type: " + what(); 
}

inline size_t ErrRuntime::errmsg_maxsize() noexcept { 
    return _errmsg_maxsize; 
}

inline auto ErrRuntime::errmsg_maxno() noexcept -> errno_t { 
    return _errmsg_maxno; 
}

template<typename ...Args>
void ErrRuntime::throw_(errno_t new_errno, Args &&... args) {
    if( _err_cntl_flag )
        prints( std::forward<Args>(args)... );
    throw ErrRuntime( new_errno ); 
}

template<typename ...Args>
void ErrRuntime::abort(errno_t e, Args &&... args) {
    prints( std::forward<Args>(args)... );
    exit(e);
}

inline const char * ErrLogic::what() const noexcept { 
    return ( (_errno > _errmsg_maxno) | (_errno < 0) ) ? 
        _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; 
}

inline string ErrLogic::whats() const { 
    return err_type_t::whats() + " | Type: " + what(); 
}

inline size_t ErrLogic::errmsg_maxsize() noexcept { 
    return _errmsg_maxsize; 
}

inline auto ErrLogic::errmsg_maxno() noexcept -> errno_t { 
    return _errmsg_maxno; 
}

template<typename ...Args>
void ErrLogic::throw_(errno_t new_errno, Args &&... args){
    if( _err_cntl_flag )
        prints( std::forward<Args>(args)... );
    throw ErrLogic( new_errno ); 
}
template<typename ...Args>
void ErrLogic::abort(errno_t e, Args &&... args){
    prints( std::forward<Args>(args)... );
    exit( e );
}

} // namespace HIPP
#endif	//_HIPPCNTL_ERROR_TYPE_H_