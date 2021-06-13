/**
create: Yangyao CHEN, 2021/06/01
    [write   ] String manupulation functions.
*/

#ifndef _HIPPCNTL_STREAM_STR_H_
#define _HIPPCNTL_STREAM_STR_H_
#include "stream_base.h"
#include "stream_fmt_io.h"
#include <cstring>
namespace HIPP{

namespace _hippcntl_STREAM_str_helper{
    inline size_t sepsize( char c ){ return 1; }
    inline size_t sepsize( const char *s ){ return std::strlen(s); }
    inline size_t sepsize( const char *s, size_t n ){ return n; }
    inline size_t sepsize( const string &s ){ return s.size(); }
} // namespace _hippcntl_STREAM_str_helper

/**
Split string `s` into segments, separated by `sep`.
`sep` can be any one of the following:
(1)  a single `char` type.
(2)  a single `std::string` type.
(3)  a single `const char *` type.
(4)  `const char *` and `std::size_t n`, where the first n characters of the 
     c-style string are used as the separator.

Examples
----------
 auto substrs = str_split( "hello, world", ',' );
 // After returning, substrs is a std::vector<std::string> composed of two 
 // string "hello" and " world".

 substrs = str_split( "/usr/lib:/usr/local/lib:", ":" );
 // After returning, substrs is like { "/usr/lib", "/usr/local/lib", "" }.

*/
template<typename ...Sep>
vector<string> str_split( const string &s, Sep && ...sep ){
    size_t sepsize = 
        _hippcntl_STREAM_str_helper::sepsize( std::forward<Sep>(sep)... );
    vector<string> subs;
    size_t b = 0, e = 0;
    do{
        e = s.find( std::forward<Sep>(sep)..., b );
        subs.push_back( s.substr( b, e-b ) );
        if( e == string::npos ) break;
        b = e + sepsize;
    }while( true );
    return subs;
}

template<typename Container, typename Sep>
string str_join( const Container &list, const Sep &sep ){
    string ans;
    auto b = list.begin(), e = list.end();
    if( b != e ) ans += str(*b++);
    while( b != e ){
        ans += sep;
        ans += str( *b++ );
    }
    return ans;
}

}
#endif	//_HIPPCNTL_STREAM_STR_H_