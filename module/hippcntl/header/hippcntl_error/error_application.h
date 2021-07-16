/**
 * creat: Yangyao CHEN, 2019/12/28
 *      [write   ] ErrApp - parent class for error-number based exceptions.
 */ 

#ifndef _HIPPCNTL_ERROR_APPLICATION_H_
#define _HIPPCNTL_ERROR_APPLICATION_H_
#include "../hippcntl_incl/incl.h"
namespace HIPP{

/**
 * parent class for error-number based exceptions.
 * Should not be used directly.
 * 
 * Application-level exceptions: a 16-bit short integer type numbers the
 * error. The first bit reserved for future usage, while the remaining 15
 * bits are used for error coding.
 */
class ErrApp: virtual public std::exception {
public:
    typedef std::uint16_t errno_t;

    enum ERRNOS: errno_t { 
        eDEFAULT=1, eUNKNOWN=2, eSYSTEM=3, eMPI=4, eOPENMP=5, 
        eH5=6, eGSL=7, ePY=8 };

    ErrApp( errno_t new_errno = 1 ) noexcept;
    virtual const char *what() const noexcept override;
    virtual string whats() const;

    static size_t errmsg_maxsize() noexcept;
    static errno_t errmsg_maxno() noexcept;

    errno_t get_errno() const noexcept;
    void set_errno( errno_t new_errno ) noexcept;
protected:
    errno_t _errno;

    static const size_t _errmsg_maxsize = 32;
    static const errno_t _errmsg_maxno = 8;
    static const char * _errmsgs[];

    const char *_what() const noexcept;
};


inline ErrApp::ErrApp( errno_t new_errno ) noexcept
: _errno(new_errno){}
inline const char * ErrApp::what() const noexcept { 
    return _what(); 
}
inline string ErrApp::whats() const { 
    return string("Application: ")  + _what(); 
}
inline size_t ErrApp::errmsg_maxsize() noexcept { 
    return _errmsg_maxsize; 
}
inline auto ErrApp::errmsg_maxno() noexcept -> errno_t { 
    return _errmsg_maxno; 
    }
inline auto ErrApp::get_errno() const noexcept -> errno_t { 
    return _errno; 
}
inline void ErrApp::set_errno( errno_t new_errno ) noexcept { 
    _errno = new_errno; 
}
inline const char * ErrApp::_what() const noexcept { 
    return _errno > _errmsg_maxno ? 
        _errmsgs[_errmsg_maxno+1] : _errmsgs[_errno]; 
}

class ErrAppDefault: public ErrApp { 
    public: ErrAppDefault() noexcept: ErrApp(1){ } };
class ErrAppUnknown: public ErrApp { 
    public: ErrAppUnknown() noexcept: ErrApp(2){ } };
class ErrAppSystem: public ErrApp { 
    public: ErrAppSystem() noexcept: ErrApp(3){ } };
class ErrAppMPI: public ErrApp { 
    public: ErrAppMPI() noexcept: ErrApp(4){ } };
class ErrAppOpenMP: public ErrApp { 
    public: ErrAppOpenMP() noexcept: ErrApp(5){ } };
class ErrAppH5: public ErrApp { 
    public: ErrAppH5() noexcept: ErrApp(6){ } };
class ErrAppGSL: public ErrApp { 
    public: ErrAppGSL() noexcept: ErrApp(7){ } };
class ErrAppPy: public ErrApp { 
    public: ErrAppPy() noexcept: ErrApp(8){ } };

} // namespace HIPP
#endif	//_HIPPCNTL_ERROR_APPLICATION_H_

