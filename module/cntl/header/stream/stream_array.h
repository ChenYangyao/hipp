#ifndef _HIPPCNTL_STREAM_ARRAY_H_
#define _HIPPCNTL_STREAM_ARRAY_H_
#include "../incl/incl.h"
#include "../error/error.h"
namespace HIPP{

template<typename InputIterator> class PrtArray;
template<typename InputIterator>
ostream & operator<<( ostream &os, const PrtArray<InputIterator> &arr );

template<typename InputIterator>
class PrtArray{
public:
    template<typename Container>
    PrtArray( const Container &array );
    template<typename _InputIterator>
    PrtArray( _InputIterator b, _InputIterator e );
    ~PrtArray(){ }
    PrtArray( const PrtArray & ) = default;
    PrtArray( PrtArray && ) = default;
    PrtArray & operator=( const PrtArray & ) = default;
    PrtArray & operator=( PrtArray && ) = default;

    ostream & prt( ostream &os = cout ) const;
    friend ostream & 
        operator<< <InputIterator>( ostream &os, const PrtArray &arr );
    
    template<typename Container>
    PrtArray & set_array( const Container &array );
    template<typename _InputIterator>
    PrtArray & set_array( _InputIterator b, _InputIterator e );

    std::ptrdiff_t indent() const noexcept;
    const string & endline() const noexcept;

    const string & sep() const noexcept;
    const string & endlast() const noexcept;

    std::ptrdiff_t ncol() const noexcept;
    std::ptrdiff_t coloffset() const noexcept;

    std::ptrdiff_t width() const noexcept;

    PrtArray & indent( std::ptrdiff_t value );
    PrtArray & endline( const string &value );

    PrtArray & sep( const string &value );
    PrtArray & endlast( const string &value );

    PrtArray & ncol( std::ptrdiff_t value );
    PrtArray & coloffset( std::ptrdiff_t value );

    PrtArray & width( std::ptrdiff_t value );

    PrtArray & reset_fmt();
protected:
    InputIterator _b, _e; 
    std::ptrdiff_t _indent = 0, _width = -1, 
        _ncol = -1, _coloffset = 0;
    string _sep = ",", _endline = "\n", _endlast = "";

    ostream & _prt_item( ostream &os,
        InputIterator it, std::ptrdiff_t count ) const;
};

template<typename InputIterator>
template<typename Container>
PrtArray<InputIterator>::PrtArray( const Container &array )
: _b( std::begin(array) ), _e( std::end(array) ){ }

template<typename InputIterator>
template<typename _InputIterator>
PrtArray<InputIterator>::PrtArray( _InputIterator b, _InputIterator e )
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
template<typename Container>
PrtArray<InputIterator> & 
PrtArray<InputIterator>::set_array( const Container &array ){
    _b = std::begin(array);
    _e = std::end(array);
    return *this;
}

template<typename InputIterator>
template<typename _InputIterator>
PrtArray<InputIterator> & 
PrtArray<InputIterator>::set_array( _InputIterator b, _InputIterator e ){
    _b = b;
    _e = e;
    return *this;
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
PrtArray<InputIterator> & 
PrtArray<InputIterator>::indent( std::ptrdiff_t value ){
    if( value < 0 )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... indent value must be non-negative. Got ", 
            value, '\n' );
    _indent = value;
    return *this;
}
template<typename InputIterator>
PrtArray<InputIterator> & 
PrtArray<InputIterator>::endline( const string &value ){
    _endline = value;
    return *this;
}

template<typename InputIterator>
PrtArray<InputIterator> & PrtArray<InputIterator>::sep( const string &value ){
    _sep = value;
    return *this;
}

template<typename InputIterator>
PrtArray<InputIterator> & 
PrtArray<InputIterator>::endlast( const string &value ){
    _endlast = value;
    return *this;
}

template<typename InputIterator>
PrtArray<InputIterator> & PrtArray<InputIterator>::ncol( std::ptrdiff_t value ){
    if( value == 0 || value < -1 )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... ncol value must be -1 or positive. Got", value, '\n' );
    _ncol = value;
    return *this;
}

template<typename InputIterator>
PrtArray<InputIterator> & 
PrtArray<InputIterator>::coloffset( std::ptrdiff_t value ){
    if( value < 0 )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... coloffset value must be non-negative. Got", value, '\n' );
    _coloffset = value;
    return *this;
}

template<typename InputIterator>
PrtArray<InputIterator> & 
PrtArray<InputIterator>::width( std::ptrdiff_t value ){
    if( value < -1 )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... width value must be -1 or positive. Got", value, '\n' );
    _width = value;
    return *this;
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

template<typename InputIterator>
PrtArray<InputIterator> & PrtArray<InputIterator>::reset_fmt(){
    _indent = 0; _width = -1; _ncol = -1; _coloffset = 0;
    _sep = ","; _endline = "\n"; _endlast = "";
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
    typedef typename Container::const_iterator it_t;
    return PrtArray<it_t>( array ).prt( os );
}
template<typename InputIterator>
ostream & prt_a( ostream &os, InputIterator b, InputIterator e ){
    return PrtArray<InputIterator>( b, e ).prt( os );
}
    
} // namespace HIPP
#endif	//_HIPPCNTL_STREAM_ARRAY_H_