/**
 * creat: Yangyao CHEN, 2019/12/28
 *      [write   ] ErrClass - parent class for error-number based exceptions.
 */ 
#ifndef _HIPPCNTL_ERROR_CLASS_H_
#define _HIPPCNTL_ERROR_CLASS_H_
#include "../incl/incl.h"
namespace HIPP{

/**
 * parent class for error-number based exceptions.
 * Should not be used directly.
 */
class ErrClass: virtual public std::exception {
public:
    typedef std::uint16_t errno_t;

    enum ERRNOS: errno_t { 
        eDEFAULT=1, eUNKNOWN=2, eRUNTIME=3, eLOGIC=4, eMEMORY=5, 
        eCAST=6, eIO=7 };

    ErrClass( errno_t new_errno = 1 ) noexcept;
    virtual const char *what()const noexcept override;
    virtual string whats() const;
    static size_t errmsg_maxsize() noexcept;
    static errno_t errmsg_maxno() noexcept;
        
    errno_t get_errno() const noexcept;
    void set_errno( errno_t new_errno ) noexcept;
protected:
    errno_t _errno;

    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 7;
    static const char * _errmsgs[];

    const char *_what()const noexcept
        { return _errno > _errmsg_maxno ? 
            _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; }
};

inline ErrClass::ErrClass( errno_t new_errno ) noexcept
: _errno(new_errno){ }
inline const char * ErrClass::what()const noexcept { 
    return _what(); 
}
inline string ErrClass::whats() const { 
    return string("Class: ")  + _what(); 
}
inline size_t ErrClass::errmsg_maxsize() noexcept { 
    return _errmsg_maxsize; 
}
inline auto ErrClass::errmsg_maxno() noexcept -> errno_t { 
    return _errmsg_maxno; 
}    
inline auto ErrClass::get_errno() const noexcept -> errno_t { 
    return _errno; 
}
inline void ErrClass::set_errno( errno_t new_errno ) noexcept { 
    _errno = new_errno; 
}  
inline const char * ErrClass::_what()const noexcept { 
    return _errno > _errmsg_maxno ? 
            _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; 
}

class ErrClassDefault: public ErrClass { 
    public: ErrClassDefault() noexcept: ErrClass(1){ } };
class ErrClassUnknown: public ErrClass { 
    public: ErrClassUnknown() noexcept: ErrClass(2){ } };
class ErrClassRuntime: public ErrClass { 
    public: ErrClassRuntime() noexcept: ErrClass(3){ } };
class ErrClassLogic: public ErrClass { 
    public: ErrClassLogic() noexcept: ErrClass(4){ } };
class ErrClassMemory: public ErrClass { 
    public: ErrClassMemory() noexcept: ErrClass(5){ } };
class ErrClassCast: public ErrClass { 
    public: ErrClassCast() noexcept: ErrClass(6){ } };
class ErrClassIO: public ErrClass { 
    public: ErrClassIO() noexcept: ErrClass(7){ } };

} // namespace HIPP
#endif	//_HIPPCNTL_ERROR_CLASS_H_