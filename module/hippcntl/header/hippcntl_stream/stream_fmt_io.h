/**
create: Yangyao CHEN, 2019/12/28
    [write   ] prt, prt_f, prt_a, str, str_f, str_a - formatted writes
        and string constructors.
*/ 

#ifndef _HIPPCNTL_STREAM_FMT_IO_H_
#define _HIPPCNTL_STREAM_FMT_IO_H_
#include "stream_base.h"
#include "stream_array.h"

#define HIPPCNTL_CLASS_INFO(class_name) \
    "<" #class_name "> {", (void *)this, ", size=", sizeof(class_name), "}\n"
#define HIPPCNTL_CLASS_INFO_INLINE(class_name) \
    "<" #class_name "> "
#define HIPPCNTL_CLASS_INFO_INDENT 3
#define HIPPCNTL_CLASS_INFO_SUBHEAD "   |- "
#define HIPPCNTL_CLASS_INFO_INDENT_STR(level) \
    string(level*HIPPCNTL_CLASS_INFO_INDENT, ' ') + HIPPCNTL_CLASS_INFO_SUBHEAD

namespace HIPP{


/**
Print ``args`` into an output stream ``os``. ``os`` is returned.

``prt()``: print ``args`. No padding is added between adjacent arguments or at 
end. Each argument in ``args`` must have overloaded ``operator<<``.

``prt_f()``: use formatting string ``fmt`` to format the output. The formatting
is the same as standard output functions ``printf()``.

``prt_a()``: prints an array of elements. The array can be specified by a 
single container ``array`` (``std::begin`` and ``std::end`` are applied to 
determine its iterable range) or a pair of iterators ``b, e``. Adjacent elements 
are separated by a comma ",". No padding is added at the beginning or at the 
end of the printed list. For a more fine-tuning controlling, use 
:class:`PrtArray` class instead.
*/
template<typename ...Args>
ostream & prt(ostream &os, Args &&... args) {
    return ( os << ... << std::forward<Args>(args) );
}

template<typename ...Args>
ostream & prt_f(ostream &os, const char *fmt, Args && ...args) {
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

template<typename ...Args>
ostream & prt_f(ostream &os, const string &fmt, Args && ...args) {
    return prt_f(os, fmt.c_str(), std::forward<Args>(args)...);
}

template<typename Container>
ostream & prt_a( ostream &os, const Container &array ) {
    return PrtArray( array ).prt( os );
}

template<typename InputIterator>
ostream & prt_a( ostream &os, InputIterator b, InputIterator e ) {
    return PrtArray( b, e ).prt( os );
}

/**
String factories. These functions accept the same arguments as :func:`prt()` and 
its variants do, but return a ``std::string`` instead of printing the content 
into a stream.
*/
template<typename ...Args>
string str( Args && ...args ) {
    ostringstream oss;
    prt( oss, std::forward<Args>(args)... );
    return oss.str();
}

template<typename ...Args>
string str_f( const char *fmt, Args && ...args ) {
    ostringstream oss;
    prt_f( oss, fmt, std::forward<Args>(args)... );
    return oss.str();
}

template<typename ...Args>
string str_f( const string &fmt, Args && ...args ) {
    return str_f(fmt.c_str(), std::forward<Args>(args)...);
}

template<typename Container>
string str_a( const Container &array ) {
    ostringstream oss;
    prt_a(oss, array);
    return oss.str();
}

template<typename InputIterator>
string str_a( InputIterator b, InputIterator e ) {
    ostringstream oss;
    prt_a(oss, b, e);
    return oss.str();
}
    
} // namespace HIPP
#endif	//_HIPPCNTL_STREAM_FMT_IO_H_