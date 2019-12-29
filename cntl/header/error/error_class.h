#ifndef _HIPPCNTL_ERROR_CLASS_H_
#define _HIPPCNTL_ERROR_CLASS_H_
#include "../incl/incl.h"
namespace HIPP{

class ErrClass: virtual public std::exception {
public:
    typedef std::uint16_t errno_t;

    ErrClass( error_t new_errno = 1 ) noexcept
        : _errno(new_errno){ }
    virtual const char *what()const noexcept override 
        { return _what(); }
    virtual string whats() const
        { return string("Class: ")  + _what(); }

    static size_t errmsg_maxsize() noexcept { return _errmsg_maxsize; }
    static errno_t errmsg_maxno() noexcept { return _errmsg_maxno; }
        
    errno_t get_errno() const noexcept 
        { return _errno; }
    void set_errno( errno_t new_errno ) noexcept 
        { _errno = new_errno; }  
protected:
    error_t _errno;

    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 7;
    static const char * _errmsgs[];

    const char *_what()const noexcept
        { return _errno > _errmsg_maxno ? _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; }
};

class ErrClassDefault: public ErrClass { public: ErrClassDefault() noexcept: ErrClass(1){ } };
class ErrClassUnknown: public ErrClass { public: ErrClassUnknown() noexcept: ErrClass(2){ } };
class ErrClassRuntime: public ErrClass { public: ErrClassRuntime() noexcept: ErrClass(3){ } };
class ErrClassLogic: public ErrClass { public: ErrClassLogic() noexcept: ErrClass(4){ } };
class ErrClassMemory: public ErrClass { public: ErrClassMemory() noexcept: ErrClass(5){ } };
class ErrClassCast: public ErrClass { public: ErrClassCast() noexcept: ErrClass(6){ } };
class ErrClassIO: public ErrClass { public: ErrClassIO() noexcept: ErrClass(7){ } };

} // namespace HIPP
#endif	//_HIPPCNTL_ERROR_CLASS_H_