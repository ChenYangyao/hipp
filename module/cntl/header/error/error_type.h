/**
 * creat: Yangyao CHEN, 2019/12/28
 *      [write   ] 
 *          ErrType -   parent class for error-number based exceptions.
 *          ErrSystem - system call exceptions.
 *          ErrRuntime- runtime errors.
 */ 

#ifndef _HIPPCNTL_ERROR_TYPE_H_
#define _HIPPCNTL_ERROR_TYPE_H_
#include "error_application.h"
#include "error_class.h"
namespace HIPP{

/**
 * parent class for error-number based exceptions.
 * Should not be used directly.
 */
template<typename ErrAppT, typename ErrClassT>
class ErrType: public ErrAppT, public ErrClassT {
public:
    typedef int errno_t;
    typedef int flag_t;
    typedef ErrAppT err_app_t;
    typedef ErrClassT err_class_t;

    ErrType( errno_t new_errno ) noexcept
        : _errno( new_errno ) { }

    virtual const char *what() const noexcept override { return ""; }
    virtual string whats() const override 
        { return ErrAppT::whats() + " | " + ErrClassT::whats(); }

    errno_t get_errno() const noexcept 
        { return _errno; }
    void set_errno( errno_t new_errno ) noexcept 
        { _errno = new_errno; }

    /**
     * the no-argument version retrive the current error-controlling flag, the
     * single-argument version set the error-controlling flag.
     * 
     * Error-controlling flag is a thread-local, static varible controlling the
     * behavior of library when an error is detected.
     * The current possible value for this flag is:
     *      0:  silent before throw the exception.
     *      1:  print error information to std::cerr and then throw the 
     *          exception.
     * Other values are treat as 1.
     */
    static flag_t err_cntl_flag() noexcept
        { return _err_cntl_flag; }
    static void err_cntl_flag( flag_t flag ) noexcept
        { _err_cntl_flag = flag; }
protected:
    errno_t _errno;
    thread_local static flag_t _err_cntl_flag;

    template<typename Arg, typename ...Args>
    static void prints( Arg &&arg, Args && ...args ) noexcept
        { cerr << std::forward<Arg>(arg); prints( std::forward<Args>(args)... ); }
    template<typename Arg>
    static void prints( Arg &&arg ) noexcept
        { cerr << std::forward<Arg>(arg) << endl; }
    static void prints() noexcept {}
};

template<typename ErrAppT, typename ErrClassT>
thread_local typename ErrType<ErrAppT, ErrClassT>::flag_t ErrType<ErrAppT, ErrClassT>::_err_cntl_flag = 1;

/**
 * system call exceptions.
 * Error NO.s are consistent with system call error numbers.
 */
class ErrSystem: public ErrType<ErrAppSystem, ErrClassDefault>{
public:
    typedef ErrType<ErrAppSystem, ErrClassDefault> err_type_t;

    /**
     * constructor.
     * @new_errorno: system call error number.
     * 
     * An error number should be a positive number, however, 0 is also 
     * acceptable.
     * Maximum error number (included) can be obtained by errmsg_maxno().
     * Error number can be retrived by get_errno(), or reset by set_error().
     */
    using err_type_t::ErrType;

    /**
     * return information of the exception instance.
     * what() - return a static C-style string indicating the type of system 
     *          call error.
     *          Maximum string length can be obtained by errmsg_maxsize().
     * whats()- return a string instead. Contain also the information from
     *          parent classes.
     */
    virtual const char *what() const noexcept override
        { return ( (_errno > _errmsg_maxno) | (_errno < 0) ) ? 
            _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; }
    virtual string whats() const override
        { return err_type_t::whats() + " | Type: " + what(); }

    static size_t errmsg_maxsize() noexcept 
        { return _errmsg_maxsize; }
    static errno_t errmsg_maxno() noexcept 
        { return _errmsg_maxno; }
    
    /**
     * check whether a system call return value has an error 
     * (i.e. `new_errno` > 0).
     * 
     * If so, print `args...` into std::cerr, and raise an ErrSystem with 
     * corresponding `new_errorno`.
     */
    template<typename ...Args>
    static void check( errno_t new_errno, Args &&... args )
        { if( new_errno ) throw_(new_errno, std::forward<Args>(args)...); }
    template<typename ...Args>
    static void throw_( errno_t new_errno, Args &&... args ){
        if( _err_cntl_flag )
            prints( std::forward<Args>(args)... );
        throw ErrSystem( new_errno ); 
    }
private:
    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 132;
    static const char * _errmsgs[];
};

/**
 * runtime errors
 */
class ErrRuntime: public ErrType<ErrAppDefault, ErrClassRuntime>{
public:
    typedef ErrType<ErrAppDefault, ErrClassRuntime> err_type_t;
    /**
     * constructors
     * @new_errorno: error number.
     * 
     * Valid error numbers are defiend in enumerate type ERRNOS.
     * An error number should be a positive number, however, 0 is also 
     * acceptable.
     * Maximum error number (included) can be obtained by errmsg_maxno().
     */
    using err_type_t::ErrType;

    /**
     * return information of the exception instance.
     * what() - return a static C-style string indicating the type of runtime
     *          call error.
     *          Maximum string length can be obtained by errmsg_maxsize().
     * whats()- return a string instead. Contain also the information from
     *          parent classes.
     */
    virtual const char *what() const noexcept override
        { return ( (_errno > _errmsg_maxno) | (_errno < 0) ) ? 
            _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; }
    virtual string whats() const override
        { return err_type_t::whats() + " | Type: " + what(); }

    static size_t errmsg_maxsize() noexcept { return _errmsg_maxsize; }
    static errno_t errmsg_maxno() noexcept { return _errmsg_maxno; }

    template<typename ...Args>
    static void throw_( errno_t new_errno, Args &&... args ){
        if( _err_cntl_flag )
            prints( std::forward<Args>(args)... );
        throw ErrRuntime( new_errno ); 
    }

    enum ERRNOS: errno_t {
        eDEFAULT=1, eOVERFLOW=2, eUNDERFLOW=3, eRANGE=4, eSTRCONSTRUCT=5
    };
private:
    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 5;
    static const char * _errmsgs[];
};


/**
 * logic errors
 */
class ErrLogic: public ErrType<ErrAppDefault, ErrClassLogic>{
public:
    typedef ErrType<ErrAppDefault, ErrClassLogic> err_type_t;
    /**
     * constructors
     * @new_errorno: error number.
     * 
     * Valid error numbers are defiend in enumerate type ERRNOS.
     * An error number should be a positive number, however, 0 is also 
     * acceptable.
     * Maximum error number (included) can be obtained by errmsg_maxno().
     */
    using err_type_t::ErrType;

    /**
     * return information of the exception instance.
     * what() - return a static C-style string indicating the type of logic 
     *          call error.
     *          Maximum string length can be obtained by errmsg_maxsize().
     * whats()- return a string instead. Contain also the information from
     *          parent classes.
     */
    virtual const char *what() const noexcept override
        { return ( (_errno > _errmsg_maxno) | (_errno < 0) ) ? 
            _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; }
    virtual string whats() const override
        { return err_type_t::whats() + " | Type: " + what(); }

    static size_t errmsg_maxsize() noexcept { return _errmsg_maxsize; }
    static errno_t errmsg_maxno() noexcept { return _errmsg_maxno; }

    template<typename ...Args>
    static void throw_( errno_t new_errno, Args &&... args ){
        if( _err_cntl_flag )
            prints( std::forward<Args>(args)... );
        throw ErrLogic( new_errno ); 
    }

    enum ERRNOS: errno_t {
        eDEFAULT=1, eDOMAIN=2, eINVALIDARG=3, eOUTOFRANGE=4, eLENGTH=5
    };
private:
    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 5;
    static const char * _errmsgs[];
};


} // namespace HIPP
#endif	//_HIPPCNTL_ERROR_TYPE_H_