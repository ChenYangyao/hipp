#ifndef _HIPPCNTL_STREAM_ARRAY_H_
#define _HIPPCNTL_STREAM_ARRAY_H_
#include "../incl/incl.h"
#include "../error/error.h"
namespace HIPP{

/**
 * PrtArray - formatted printing for array-like containers.
 * 
 * Format
 * ----------
 * indent padding item[0]    sep padding item[1]        ... sep padding item[ncol-1]  sep endline
 * indent padding item[ncol] sep padding item[ncol+1]   ... sep padding item[2ncol-1] sep endline
 * ... indent padding item[n-2]  sep padding item[n-1] endlast
 * 
 * If coloffset > 0, no indent is added at the start of the first line, and 
 * new lines are started at (index + coloffset) % ncol == 0.
 * 
 * Default Format
 * --------------
 * @indent: 0 (no ident).
 * @width:  -1 (no padding).
 * @ncol:   -1 (no line-break).
 * @coloffset:  0.
 * @sep:    ",".
 * @endline: "\n".
 * @endlast: "".
 * 
 * Examples
 * ----------
 * vector<double> arr1 = {1,2,3,4,5,6,7,8};
 * PrtArray p_arr1(arr1);
 * cout << p_arr1 << endl;
 * // Print as:
 * // 1,2,3,4,5,6,7,8
 *  
 * p_arr1.ncol(3);
 * cout << p_arr1 << endl;
 * // print as
 * // 1,2,3,
 * // 4,5,6,
 * // 7,8
 *
 * p_arr1.width(4);
 * cout << p_arr1 << endl;
 * // print as
 * //   1,   2,   3,
 * //   4,   5,   6,
 * //   7,   8
 *  
 * cout << p_arr1;
 * p_arr1.coloffset( arr1.size()%3 );
 * cout << p_arr1 << endl;
 * // print as
 * //   1,   2,   3,
 * //   4,   5,   6,
 * //   7,   8,   1,
 * //   2,   3,   4,
 * //   5,   6,   7,
 * //   8
 * //
 */
template<typename InputIterator> class PrtArray;
template<typename InputIterator>
ostream & operator<<( ostream &os, const PrtArray<InputIterator> &arr );

template<typename InputIterator>
class PrtArray{
public:
    typedef InputIterator iter_t;

    template<typename Container>
    explicit PrtArray( const Container &array );
    PrtArray( iter_t b, iter_t e );

    ~PrtArray() noexcept { }
    PrtArray( const PrtArray & ) = default;
    PrtArray( PrtArray && ) noexcept = default;
    PrtArray & operator=( const PrtArray & ) = default;
    PrtArray & operator=( PrtArray && ) noexcept = default;

    ostream & prt( ostream &os = cout ) const;
    friend ostream & 
        operator<< <iter_t>( ostream &os, const PrtArray &arr );
    
    std::pair<iter_t, iter_t> get_iter() const;
    std::ptrdiff_t indent() const noexcept;
    const string & endline() const noexcept;
    const string & sep() const noexcept;
    const string & endlast() const noexcept;
    std::ptrdiff_t ncol() const noexcept;
    std::ptrdiff_t coloffset() const noexcept;
    std::ptrdiff_t width() const noexcept;

    template<typename Container>
    PrtArray & set_array( const Container &array );
    PrtArray & set_array( iter_t b, iter_t e );
    PrtArray & indent( std::ptrdiff_t value );
    PrtArray & endline( const string &value );
    PrtArray & sep( const string &value );
    PrtArray & endlast( const string &value );
    PrtArray & ncol( std::ptrdiff_t value );
    PrtArray & coloffset( std::ptrdiff_t value );
    PrtArray & width( std::ptrdiff_t value );
    PrtArray & reset_fmt();
protected:
    iter_t _b, _e; 
    std::ptrdiff_t _indent = 0, _width = -1, 
        _ncol = -1, _coloffset = 0;
    string  _sep = ",", _endline = "\n", _endlast = "";
    ostream & _prt_item( ostream &os,
        iter_t it, std::ptrdiff_t count ) const;
};

/* deduction guide for container input */
template<typename Container>
explicit PrtArray( const Container &array ) 
    -> PrtArray<decltype(std::begin(array))>;

template<typename InputIterator>
template<typename Container>
PrtArray<InputIterator>::PrtArray( const Container &array )
: _b( std::begin(array) ), _e( std::end(array) ){ }

template<typename InputIterator>
PrtArray<InputIterator>::PrtArray( iter_t b, iter_t e )
: _b(b), _e(e){ }

template<typename InputIterator>
ostream & PrtArray<InputIterator>::prt( ostream &os) const{
    auto it = _b; std::ptrdiff_t count = 0;
    if( it != _e ){
        if( _coloffset == 0 )
            for(std::ptrdiff_t i=0; i<_indent; ++i) os << ' ';
        else 
            os << _sep;
        if( _width > 0 ) os << std::setw(_width);
        os << *it++; ++count;
        while( it != _e ) _prt_item( os, it++, count++ );
    }
    return os << _endlast;
}
template<typename InputIterator>
ostream & operator<<( ostream &os, const PrtArray<InputIterator> &arr ){
    return arr.prt(os);
}
template<typename InputIterator>
auto PrtArray<InputIterator>::get_iter() const -> std::pair<iter_t, iter_t> {
    return {_b, _e};
}
template<typename InputIterator>
std::ptrdiff_t PrtArray<InputIterator>::indent() const noexcept{
    return _indent;
}
template<typename InputIterator>
const string & PrtArray<InputIterator>::endline() const noexcept{
    return _endline;
}

template<typename InputIterator>
const string & PrtArray<InputIterator>::sep() const noexcept{
    return _sep;
}
template<typename InputIterator>
const string & PrtArray<InputIterator>::endlast() const noexcept{
    return _endlast;
}

template<typename InputIterator>
std::ptrdiff_t PrtArray<InputIterator>::ncol() const noexcept{
    return _ncol;
}

template<typename InputIterator>
std::ptrdiff_t PrtArray<InputIterator>::coloffset() const noexcept{
    return _coloffset;
}
template<typename InputIterator>
std::ptrdiff_t PrtArray<InputIterator>::width() const noexcept{
    return _width;
}
template<typename InputIterator>
template<typename Container>
auto PrtArray<InputIterator>::set_array( const Container &array )
-> PrtArray & 
{
    _b = std::begin(array);
    _e = std::end(array);
    return *this;
}
template<typename InputIterator>
auto PrtArray<InputIterator>::set_array( iter_t b, iter_t e ) -> PrtArray & 
{
    _b = b; _e = e; return *this;
}
template<typename InputIterator>
auto PrtArray<InputIterator>::indent( std::ptrdiff_t value ) -> PrtArray &
{
    if( value < 0 )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... indent value must be non-negative. Got ", 
            value, '\n' );
    _indent = value;
    return *this;
}
template<typename InputIterator>
auto 
PrtArray<InputIterator>::endline( const string &value ) -> PrtArray & {
    _endline = value;
    return *this;
}
template<typename InputIterator>
auto PrtArray<InputIterator>::sep( const string &value ) -> PrtArray & {
    _sep = value;
    return *this;
}
template<typename InputIterator>
auto PrtArray<InputIterator>::endlast( const string &value ) -> PrtArray & {
    _endlast = value;
    return *this;
}
template<typename InputIterator>
auto PrtArray<InputIterator>::ncol( std::ptrdiff_t value ) -> PrtArray & {
    if( value == 0 || value < -1 )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... ncol value must be -1 or positive. Got", value, '\n' );
    _ncol = value;
    return *this;
}
template<typename InputIterator>
auto PrtArray<InputIterator>::coloffset( std::ptrdiff_t value ) -> PrtArray & 
{
    if( value < 0 )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... coloffset value must be non-negative. Got", value, '\n' );
    _coloffset = value;
    return *this;
}
template<typename InputIterator>
auto PrtArray<InputIterator>::width( std::ptrdiff_t value ) -> PrtArray & {
    if( value < -1 )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... width value must be -1 or positive. Got", value, '\n' );
    _width = value;
    return *this;
}
template<typename InputIterator>
auto PrtArray<InputIterator>::reset_fmt() -> PrtArray & {
    _indent = 0; _width = -1; _ncol = -1; _coloffset = 0;
    _sep = ","; _endline = "\n"; _endlast = "";
}
template<typename InputIterator>
ostream & PrtArray<InputIterator>::_prt_item( ostream &os,
    InputIterator it, std::ptrdiff_t count ) const{
    os << _sep;
    if(_ncol > 0 && (count+_coloffset) % _ncol == 0){
        os << _endline;
        for(std::ptrdiff_t i=0; i<_indent; ++i) os << ' ';
    } 
    if( _width > 0 ) os << std::setw( _width );
    os << *it;
    return os;
}
    
} // namespace HIPP
#endif	//_HIPPCNTL_STREAM_ARRAY_H_