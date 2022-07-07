/**
create: Yangyao CHEN, 2021/06/01
    [write   ] PStream - the pretty stream.
*/

#ifndef _HIPPCNTL_STREAM_PRETTY_H_
#define _HIPPCNTL_STREAM_PRETTY_H_

#include "../hippcntl_incl/incl.h"

namespace HIPP {

namespace _hippcntl_stream_pretty_helper {

/**
StreamOperand - the type of returned value by PStream.
The operand defines the operator,() and performs actual output operation 
according to the detail type of arguments.
*/
class StreamOperand {
public:
    template<typename It>
    struct it_pair_t {
        /* The type matching for a pair of iterators. */
        It b, e;
        it_pair_t(It _b, It _e): b(_b), e(_e) {} 
    };

    template<typename CB>
    struct call_back_t {
        /* The type matching for a callable on ostream. */
        CB cb;
        call_back_t(CB _cb) : cb(_cb) {}
    };

    explicit StreamOperand(ostream &os) noexcept;

    /** 
    The copy/move constructor results in object referring to the same 
    stream.
    */
    StreamOperand(const StreamOperand &) noexcept;
    StreamOperand(StreamOperand &&) noexcept;
    StreamOperand & operator=(const StreamOperand &) noexcept = delete;
    StreamOperand & operator=(StreamOperand &&) noexcept = delete;
    ~StreamOperand() noexcept;

    /**
    Stream operators.
    */
    StreamOperand & operator, (ostream& (*pf)(ostream&));
    StreamOperand & operator, (std::ios& (*pf)(std::ios&));
    StreamOperand & operator, (std::ios_base& (*pf)(std::ios_base&));

    /**
    STL components.
    */
    template<typename T, std::size_t N>
    StreamOperand & operator,(const std::array<T, N> &v);
    template<typename T, typename A>
    StreamOperand & operator,(const std::deque<T, A> &v);
    template<typename T, typename A>
    StreamOperand & operator,(const std::forward_list<T, A> &v);
    template<typename T, typename A>
    StreamOperand & operator,(const std::list<T, A> &v);
    template<typename K, typename T, typename Comp, typename A>
    StreamOperand & operator,(const std::map<K,T,Comp,A> &v);
    template<typename K, typename T, typename Comp, typename A>
    StreamOperand & operator,(const std::multimap<K,T,Comp,A> &v);
    template<typename T, typename Comp, typename A>
    StreamOperand & operator,(const std::set<T,Comp,A> &v);
    template<typename T, typename Comp, typename A>
    StreamOperand & operator,(const std::multiset<T,Comp,A> &v);
    template<typename K, typename T, typename Hash, typename Comp, typename A>
    StreamOperand & operator,(const std::unordered_map<K,T,Hash,Comp,A> &v);
    template<typename K, typename T, typename Hash, typename Comp, typename A>
    StreamOperand & operator,(
        const std::unordered_multimap<K,T,Hash,Comp,A> &v);
    template<typename T, typename Hash, typename Comp, typename A>
    StreamOperand & operator,(const std::unordered_set<T,Hash,Comp,A> &v);
    template<typename T, typename Hash, typename Comp, typename A>
    StreamOperand & operator,(const std::unordered_multiset<T,Hash,Comp,A> &v);
    template<typename T, typename A>
    StreamOperand & operator,(const vector<T, A> &v);
    
    /**
    Util components.
    */
    template<typename T1, typename T2>
    StreamOperand & operator,(const std::pair<T1, T2> &pr);
    template<typename ...Ts>
    StreamOperand & operator,(const std::tuple<Ts...> &tpl);
    template<typename It>
    StreamOperand & operator,(const it_pair_t<It> &it_pair);
    template<typename CB>
    StreamOperand & operator,(const call_back_t<CB> &cb);


    /**
    Any other type.
    
    With overloaded opearator<<(ostream &, const T&), this operator is adopated.
    Otherwise a compiler-dependent printing is made.
    */
    template<typename T>
    StreamOperand & operator,(const T &x);

    /**
    Retrieve the underlying stream object.
    */
    ostream & get_stream() const noexcept;
private:
    ostream &_os;

    template<typename ForwardIt>
    StreamOperand & _prt_range(ForwardIt b, ForwardIt e);
    template<typename ForwardIt>
    StreamOperand & _prt_pair_range(ForwardIt b, ForwardIt e);
    template<typename Tpl, std::size_t I>
    StreamOperand & _prt_tpl(const Tpl &tpl);

    template<typename T, typename RetT = 
        decltype(std::declval<ostream &>() << std::declval<const T&>() ) >
    StreamOperand & _prt_any(const T &x);
    template<typename T, typename ...Args>
    StreamOperand & _prt_any(const T &x, const Args & ...args);
}; 
} // namespace _hippcntl_stream_pretty_helper

/**
PStream - the pretty stream.
A replacement for `std::ostream`. PStream provides shortcuts which enable 
elegant output.

Features:
- Comma separated output sequence, e.g., pout << x, " = ", 1, endl;
- `Print anything`, like that for `std::vector` we can use 
  pout << std::vector {1,2,3}, endl;
*/
class PStream {
public:
    typedef _hippcntl_stream_pretty_helper::StreamOperand stream_op_t;

    PStream(ostream &os) noexcept;
    PStream(const PStream &that) noexcept;
    PStream(PStream &&that) noexcept;
    PStream & operator=(const PStream &that) = delete;
    PStream & operator=(PStream &&that) = delete;
    ~PStream() noexcept;

    stream_op_t & operator<< (ostream& (*pf)(ostream&));
    stream_op_t & operator<< (std::ios& (*pf)(std::ios&));
    stream_op_t & operator<< (std::ios_base& (*pf)(std::ios_base&));

    template<typename T>
    stream_op_t & operator<<(T &&x);

    /**
    Produce a printable wrapper w for an object (or a range of objects).

    (1): objects in a range defined by a pair of iterators [b, e).
    (2): callable ``cb`` on ``ostream``. ``cb(get_stream())`` is called for the
        actual printing.
    (3): an object implementing the info() method with HIPP convention.
        ``x.info( get_stream(), fmt_cntl, level )`` is called for the actual
        printing.

    Examples
    --------
    ```
    vector<int> a(5);       // STL container
    int *b = new int [N];   // raw buffer
    int c[M];               // raw array

    pout << "a = ", pout(v.begin(), v.end()), '\n',
            "b = ", pout(b,b+N), '\n',
            "c = ", pout(c,c+M), '\n';
    
    auto cb = [](ostream &os) { os << "foo bar baz"; };
    pout << "a = ", pout(cb), endl;

    try {
        // ... any subroutines
    } catch (const ErrLogic &e) {
        pout << "ERROR: ", pout.info_of(e), endl;
        throw;
    }
    ```
    */
    template<typename It>
    stream_op_t::it_pair_t<It> operator()(It b, It e) const;

    template<typename CB> auto operator()(CB &&cb) const;

    template<typename T>
    auto info_of(T &&x, int fmt_cntl = 0, int level = 0) const;

    /* Return a reference to the internal std::ostream object. */
    ostream & get_stream() const noexcept;
protected:
    stream_op_t _op;
};

extern PStream pout;
extern PStream perr;

namespace _hippcntl_stream_pretty_helper {

#define _HIPP_TEMPCLS StreamOperand
#define _HIPP_TEMPRET inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET StreamOperand(ostream &os) noexcept : _os(os) {}

_HIPP_TEMPNORET 
StreamOperand(const StreamOperand &op) noexcept : _os(op._os) {}

_HIPP_TEMPNORET 
StreamOperand(StreamOperand &&op) noexcept : _os(op._os) {}

_HIPP_TEMPNORET 
~StreamOperand() noexcept {}

_HIPP_TEMPRET operator, (ostream& (*pf)(ostream&)) -> StreamOperand &
{
    _os << pf; return *this; 
}

_HIPP_TEMPRET operator, (std::ios& (*pf)(std::ios&))  -> StreamOperand &
{
    _os << pf; return *this; 
}

_HIPP_TEMPRET operator, (std::ios_base& (*pf)(std::ios_base&)) 
    -> StreamOperand &
{ 
    _os << pf; return *this; 
}

template<typename T, std::size_t N>
_HIPP_TEMPRET operator,(const std::array<T, N> &v) -> StreamOperand &
{
    *this, "<array> ";
    return _prt_range(std::begin(v), std::end(v));
}

template<typename T, typename A>
_HIPP_TEMPRET operator,(const std::deque<T, A> &v) -> StreamOperand &
{
    *this, "<deque> ";
    return _prt_range(std::begin(v), std::end(v));
}

template<typename T, typename A>
_HIPP_TEMPRET operator,(const std::forward_list<T, A> &v) -> StreamOperand & 
{
    *this, "<forward_list> ";
    return _prt_range(std::begin(v), std::end(v));
}

template<typename T, typename A>
_HIPP_TEMPRET operator,(const std::list<T, A> &v) -> StreamOperand &
{
    *this, "<list> ";
    return _prt_range(std::begin(v), std::end(v));
}

template<typename K, typename T, typename Comp, typename A>
_HIPP_TEMPRET operator,(const std::map<K,T,Comp,A> &v) -> StreamOperand &
{
    *this, "<map> ";
    return _prt_pair_range(std::begin(v), std::end(v));
}

template<typename K, typename T, typename Comp, typename A>
_HIPP_TEMPRET operator,(const std::multimap<K,T,Comp,A> &v) -> StreamOperand & 
{
    *this, "<multiple> ";
    return _prt_pair_range(std::begin(v), std::end(v));
}

template<typename T, typename Comp, typename A>
_HIPP_TEMPRET operator,(const std::set<T,Comp,A> &v) -> StreamOperand &
{
    *this, "<set> ";
    return _prt_range(std::begin(v), std::end(v));
}
template<typename T, typename Comp, typename A>
_HIPP_TEMPRET operator,(const std::multiset<T,Comp,A> &v) -> StreamOperand &
{
    *this, "<multiset> ";
    return _prt_range(std::begin(v), std::end(v));
}

template<typename K, typename T, typename Hash, typename Comp, typename A>
_HIPP_TEMPRET operator,(const std::unordered_map<K,T,Hash,Comp,A> &v) 
-> StreamOperand & 
{
    *this, "<unordered_map> ";
    return _prt_pair_range(std::begin(v), std::end(v));
}

template<typename K, typename T, typename Hash, typename Comp, typename A>
_HIPP_TEMPRET operator,(const std::unordered_multimap<K,T,Hash,Comp,A> &v) 
-> StreamOperand & 
{
    *this, "<unordered_multimap> ";
    return _prt_pair_range(std::begin(v), std::end(v));
}

template<typename T, typename Hash, typename Comp, typename A>
_HIPP_TEMPRET operator,(const std::unordered_set<T,Hash,Comp,A> &v) 
-> StreamOperand & 
{
    *this, "<unordered_set> ";
    return _prt_range(std::begin(v), std::end(v));
}

template<typename T, typename Hash, typename Comp, typename A>
_HIPP_TEMPRET operator,(const std::unordered_multiset<T,Hash,Comp,A> &v) 
-> StreamOperand & 
{
    *this, "<unordered_multiset> ";
    return _prt_range(std::begin(v), std::end(v));
}

template<typename T, typename A>
_HIPP_TEMPRET operator,(const vector<T, A> &v) -> StreamOperand &
{
    *this, "<vector> ";
    return _prt_range(std::begin(v), std::end(v));
}

template<typename T1, typename T2>
_HIPP_TEMPRET operator,(const std::pair<T1, T2> &pr) -> StreamOperand &
{
    return *this, pr.first, ':' , pr.second;
}

template<typename ...Ts>
_HIPP_TEMPRET operator,(const std::tuple<Ts...> &tpl) -> StreamOperand &
{
    if constexpr( std::tuple_size_v<std::tuple<Ts...> > == 0 ){
        return *this, "()";
    }else{
        *this, '(', std::get<0>(tpl);
        _prt_tpl<std::tuple<Ts...>, 1>(tpl);
        return *this, ')';
    }
}

template<typename It>
_HIPP_TEMPRET operator,(const it_pair_t<It> &it_pair) -> StreamOperand &
{
    return _prt_range(it_pair.b, it_pair.e);
}

template<typename CB>
_HIPP_TEMPRET operator,(const call_back_t<CB> &cb) -> StreamOperand &
{
    cb.cb(_os);
    return *this;
}

template<typename T>
_HIPP_TEMPRET operator,(const T &x) -> StreamOperand &
{ 
    return _prt_any(x);
}

_HIPP_TEMPRET get_stream() const noexcept -> ostream & { 
    return _os; 
}

template<typename ForwardIt>
_HIPP_TEMPRET _prt_range(ForwardIt b, ForwardIt e) -> StreamOperand &
{
    *this, '{';
    if( b != e )
        *this, *b++;
    while( b != e )
        *this, ", ", *b++;
    *this, '}';
    return *this;
}

template<typename ForwardIt>
_HIPP_TEMPRET _prt_pair_range(ForwardIt b, ForwardIt e) -> StreamOperand &
{
    *this, '{';
    if( b != e )
        *this, *b++;
    while( b != e )
        *this, ", ", *b++;
    *this, '}';
    return *this;
}

template<typename Tpl, std::size_t I>
_HIPP_TEMPRET _prt_tpl(const Tpl &tpl) -> StreamOperand &
{
    if constexpr (I == std::tuple_size_v<Tpl>){
        return *this;
    }else{
        *this, ", ", std::get<I>(tpl);
        return _prt_tpl<Tpl, I+1>(tpl);
    }
}

template<typename T, typename RetT>
_HIPP_TEMPRET _prt_any(const T &x) -> StreamOperand &
{
    _os << x;
    return *this;
}

template<typename T, typename ...Args>
_HIPP_TEMPRET _prt_any(const T &x, const Args & ...args) -> StreamOperand &
{
    _os << "<" << typeid(x).name() << "> {" 
        << (void *)&x << ", size=" << sizeof(T) << "}";
    return *this;
}

#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET
#undef _HIPP_TEMPCLS

} // namespace _hippcntl_stream_pretty_helper

#define _HIPP_TEMPCLS PStream
#define _HIPP_TEMPRET inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET PStream(ostream &os) noexcept
: _op(os) {}

_HIPP_TEMPNORET PStream(const PStream &that) noexcept
: PStream(that._op.get_stream()){}

_HIPP_TEMPNORET PStream(PStream &&that) noexcept
: PStream(that._op.get_stream()) {}


_HIPP_TEMPNORET ~PStream() noexcept {}

_HIPP_TEMPRET operator<< (ostream& (*pf)(ostream&)) -> stream_op_t &
{ 
    return _op, pf; 
}

_HIPP_TEMPRET operator<< (std::ios& (*pf)(std::ios&)) -> stream_op_t &
{ 
    return _op, pf; 
}

_HIPP_TEMPRET operator<< (std::ios_base& (*pf)(std::ios_base&))-> stream_op_t &
{ 
    return _op, pf; 
}

template<typename T>
_HIPP_TEMPRET operator<<(T &&x) -> stream_op_t &
{ 
    return _op, std::forward<T>(x);
}

template<typename It>
auto _HIPP_TEMPCLS::operator()(It b, It e) const -> stream_op_t::it_pair_t<It>
{ 
    return {b, e}; 
}

template<typename CB>
auto _HIPP_TEMPCLS::operator()(CB &&cb) const
{
    using _CB = std::remove_cv_t<std::remove_reference_t<CB> >;
    return stream_op_t::call_back_t<_CB>(std::forward<CB>(cb)); 
}

template<typename T>
auto _HIPP_TEMPCLS::info_of(T &&x, int fmt_cntl, int level) const
{
    auto cb = [&x, fmt_cntl, level](ostream &os) {
        std::forward<T>(x).info(os, fmt_cntl, level);
    };
    return (*this)(cb);
}

_HIPP_TEMPRET get_stream() const noexcept -> ostream &
{ 
    return _op.get_stream(); 
}

#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET
#undef _HIPP_TEMPCLS

} // namespace HIPP
#endif	//_HIPPCNTL_STREAM_PRETTY_H_