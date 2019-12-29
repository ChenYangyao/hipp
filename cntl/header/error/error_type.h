#ifndef _HIPPCNTL_ERROR_TYPE_H_
#define _HIPPCNTL_ERROR_TYPE_H_
#include "error_application.h"
#include "error_class.h"
namespace HIPP{

/**
 * Error NO.s are consistent with system call error No.
 */
template<typename ErrAppT, typename ErrClassT>
class ErrType: public ErrAppT, public ErrClassT {
public:
    typedef int errno_t;
    typedef ErrAppT err_app_t;
    typedef ErrClassT err_class_t;

    ErrType( errno_t new_errno ) noexcept
        : _errno( new_errno ) { }

    virtual const char *what() const noexcept override { return ""; }
    virtual string whats() const 
        { return ErrAppT::whats() + " | " + ErrClassT::whats(); }

    errno_t get_errno() const noexcept 
        { return _errno; }
    void set_errno( errno_t new_errno ) noexcept 
        { _errno = new_errno; }
protected:
    errno_t _errno;
};


class ErrSystem: public ErrType<ErrAppSystem, ErrClassDefault>{
public:
    typedef ErrType<ErrAppSystem, ErrClassDefault> err_type_t;
    using err_type_t::ErrType;

    virtual const char *what() const noexcept override
        { return ( (_errno > _errmsg_maxno) | (_errno < 0) ) ? _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; }
    virtual string whats() const
        { return err_type_t::whats() + " | Type: " + what(); }

    static size_t errmsg_maxsize() noexcept 
        { return _errmsg_maxsize; }
    static errno_t errmsg_maxno() noexcept 
        { return _errmsg_maxno; }
    
    template<typename ...Args>
    static void check( errno_t new_errno, Args &&... args ){
        if( new_errno ){
            prints( std::forward<Args>(args)... );
            throw ErrSystem( new_errno );
        } 
    }
private:
    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 132;
    static const char * _errmsgs[];

    template<typename Arg, typename ...Args>
    static void prints( Arg &&arg, Args && ...args )
        { cerr << std::forward<Arg>(arg); prints( std::forward<Args>(args)... ); }
    template<typename Arg>
    static void prints( Arg &&arg )
        { cerr << std::forward<Arg>(arg) << endl; }
};

class ErrRuntime: public ErrType<ErrAppDefault, ErrClassRuntime>{
public:
    typedef ErrType<ErrAppDefault, ErrClassRuntime> err_type_t;
    using err_type_t::ErrType;

    virtual const char *what() const noexcept override
        { return ( (_errno > _errmsg_maxno) | (_errno < 0) ) ? _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; }
    virtual string whats() const
        { return err_type_t::whats() + " | Type: " + what(); }

    static size_t errmsg_maxsize() noexcept { return _errmsg_maxsize; }
    static errno_t errmsg_maxno() noexcept { return _errmsg_maxno; }

    enum ERRNOS: errno_t {
        eDEFAULT=1, eOVERFLOW=2, eUNDERFLOW=3, eRANGE=4, eSTRCONSTRUCT=5
    };
private:
    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 5;
    static const char * _errmsgs[];
};


} // namespace HIPP
#endif	//_HIPPCNTL_ERROR_TYPE_H_