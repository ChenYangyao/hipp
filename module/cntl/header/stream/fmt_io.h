/**
 * creat: Yangyao CHEN, 2019/12/28
 *      [write   ] prt, prt_f, str, str_f - formatted writes
 *                  and string constructors.
 */ 

#ifndef _HIPPCNTL_FMT_IO_H_
#define _HIPPCNTL_FMT_IO_H_
#include "../incl/incl.h"
#include "../error/error.h"

#define HIPPCNTL_CLASS_INFO(class_name) \
    #class_name " instance [loc=", (void *)this, ", size=", sizeof(class_name),\
    ", align=", alignof(class_name), "]\n----------\n"
#define HIPPCNTL_CLASS_INFO_INLINE(class_name) \
    #class_name " instance [", (void *)this, "] "

namespace HIPP{

/**
 * print any no. aruguments into stream `os`.
 * 
 * No padding is added between args and at endding.
 * Empty argument list is valid.
 * Each argument needs overloaded `<<` operator.
 * 
 * Examples
 * ----------
 *  string str = "hello";
 *  const char *cstr = "world!";
 *  prt( cout, str, ", ", cstr );
 */
template<typename Arg> ostream & prt( ostream &os, Arg &&arg )
    { return os << std::forward<Arg>(arg); }
template<typename Arg, typename ...Args>
ostream & prt( ostream &os, Arg &&arg, Args &&...args ){
    os  << std::forward<Arg>(arg);
    return prt( os, std::forward<Args>(args)... );
}
inline ostream & prt( ostream &os )noexcept{ return os; }

/**
 * formatted print
 * Similar to prt, but use c-style `fmt` and `args ...` to control format.
 * 
 * Examples
 * ----------
 *  const char *cstr = "world!";
 *  prt_f( cout, "hello, %s", cstr );
 */
template<typename ...Args>
ostream & prt_f( ostream &os, const char *fmt, Args && ...args ){
    vector<char> buff(256);
    auto ret = std::snprintf( buff.data(), buff.size(), 
        fmt, std::forward<Args>(args)... );
    if( ret < 0 )
        throw ErrRuntime( ErrRuntime::eSTRCONSTRUCT );
    if( ret >= decltype(ret)( buff.size() ) ){
        buff.resize( ret+1 );
        ret = std::snprintf( buff.data(), buff.size(), 
            fmt, std::forward<Args>(args)... );
        if( ret < 0 || ret >= decltype(ret)( buff.size() ) )
            throw ErrRuntime( ErrRuntime::eSTRCONSTRUCT );
    }
    return os << buff.data();
}

/**
 * str() and str_f() - string constructors
 * 
 * Similar to prt() and prt_f(), but return a string instead of printing to 
 * stream.
 */
template<typename ...Args>
string str( Args && ...args ){
    ostringstream oss;
    prt( oss, std::forward<Args>(args)... );
    return oss.str();
}
template<typename ...Args>
string str_f( const char *fmt, Args && ...args ){
    ostringstream oss;
    prt_f( oss, fmt, std::forward<Args>(args)... );
    return oss.str();
}
    
} // namespace HIPP
#endif	//_HIPPCNTL_FMT_IO_H_