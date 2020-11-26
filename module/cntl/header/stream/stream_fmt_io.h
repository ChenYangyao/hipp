/**
 * creat: Yangyao CHEN, 2019/12/28
 *      [write   ] prt, prt_f, prt_a, str, str_f, str_a - formatted writes
 *                  and string constructors.
 */ 

#ifndef _HIPPCNTL_STREAM_FMT_IO_H_
#define _HIPPCNTL_STREAM_FMT_IO_H_
#include "stream_array.h"

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
template<typename ...Args>
ostream & prt(ostream &os, Args &&... args) {
    return ( os << ... << std::forward<Args>(args) );
}

/**
 * formatted print.
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
 * print a list of elements into stream `os`.
 * 
 * Adjacent elements are separated by a comma ','. No padding is added at the
 * beginning or at the end of the printed list.
 * 
 * For a more fine-tuning controlling, use PrtArray<> class instead.
 * 
 * Examples
 * ----------
 *  // Print all elements in 'arr' into `cout`. Or print the first 4 elements
 *  // into `cout`.
 *  vector<int> arr(5, 1);
 *  prt_a(cout, arr) << endl;
 *  prt_a(cout, arr.begin(), arr.begin()+4) << endl;
 *  // In the output devide, printed results would be like
 *  // 1,1,1,1,1
 *  // 1,1,1,1
 */
template<typename Container>
ostream & prt_a( ostream &os, const Container &array ){
    return PrtArray( array ).prt( os );
}
template<typename InputIterator>
ostream & prt_a( ostream &os, InputIterator b, InputIterator e ){
    return PrtArray( b, e ).prt( os );
}

/**
 * str() and str_f() and str_a() - string constructors.
 * 
 * Similar to prt() and prt_f() and prt_a(), but return a string instead of 
 * printing to stream.
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
template<typename Container>
string str_a( const Container &array ) {
    ostringstream oss;
    prt_a(oss, array);
    return oss.str();
}
template<typename InputIterator>
string str_a( InputIterator b, InputIterator e ){
    ostringstream oss;
    prt_a(oss, b, e);
    return oss.str();
}
    
} // namespace HIPP
#endif	//_HIPPCNTL_STREAM_FMT_IO_H_