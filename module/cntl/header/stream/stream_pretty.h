/**
create: Yangyao CHEN, 2021/06/01
    [write   ] PStream - the pretty stream.
*/

#ifndef _HIPPCNTL_STREAM_PRETTY_H_
#define _HIPPCNTL_STREAM_PRETTY_H_
#include "stream_base.h"
#include "stream_fmt_io.h"
namespace HIPP {

namespace _hippcntl_stream_pretty_helper {
class StreamOperand {
public:
    template<typename It>
    struct it_pair_t {
        It b, e;
        it_pair_t(It _b, It _e): b(_b), e(_e) {} 
    };

    explicit StreamOperand(ostream &os) noexcept : _os(os) {}

    StreamOperand & operator, (ostream& (*pf)(ostream&))
        { _os << pf; return *this; }
    StreamOperand & operator, (std::ios& (*pf)(std::ios&)) 
        { _os << pf; return *this; }
    StreamOperand & operator, (std::ios_base& (*pf)(std::ios_base&))
        { _os << pf; return *this; }


    template<typename T, std::size_t N>
    StreamOperand & operator,(const std::array<T, N> &v) {
        return _prt_range(std::begin(v), std::end(v));
    }
    template<typename T, typename A>
    StreamOperand & operator,(const std::deque<T, A> &v) {
        return _prt_range(std::begin(v), std::end(v));
    }
    template<typename T, typename A>
    StreamOperand & operator,(const std::forward_list<T, A> &v) {
        return _prt_range(std::begin(v), std::end(v));
    }
    template<typename T, typename A>
    StreamOperand & operator,(const std::list<T, A> &v) {
        return _prt_range(std::begin(v), std::end(v));
    }
    template<typename K, typename T, typename Comp, typename A>
    StreamOperand & operator,(const std::map<K,T,Comp,A> &v) {
        return _prt_pair_range(std::begin(v), std::end(v));
    }
    template<typename K, typename T, typename Comp, typename A>
    StreamOperand & operator,(const std::multimap<K,T,Comp,A> &v) {
        return _prt_pair_range(std::begin(v), std::end(v));
    }
    template<typename T, typename Comp, typename A>
    StreamOperand & operator,(const std::set<T,Comp,A> &v) {
        return _prt_range(std::begin(v), std::end(v));
    }
    template<typename T, typename Comp, typename A>
    StreamOperand & operator,(const std::multiset<T,Comp,A> &v) {
        return _prt_range(std::begin(v), std::end(v));
    }
    template<typename K, typename T, typename Hash, typename Comp, typename A>
    StreamOperand & operator,(const std::unordered_map<K,T,Hash,Comp,A> &v) {
        return _prt_pair_range(std::begin(v), std::end(v));
    }
    template<typename K, typename T, typename Hash, typename Comp, typename A>
    StreamOperand & operator,(const std::unordered_multimap<K,T,Hash,Comp,A> &v) 
    {
        return _prt_pair_range(std::begin(v), std::end(v));
    }
    template<typename T, typename Hash, typename Comp, typename A>
    StreamOperand & operator,(const std::unordered_set<T,Hash,Comp,A> &v) {
        return _prt_range(std::begin(v), std::end(v));
    }
    template<typename T, typename Hash, typename Comp, typename A>
    StreamOperand & operator,(const std::unordered_multiset<T,Hash,Comp,A> &v) {
        return _prt_range(std::begin(v), std::end(v));
    }
    template<typename T, typename A>
    StreamOperand & operator,(const vector<T, A> &v) {
        return _prt_range(std::begin(v), std::end(v));
    }

    template<typename T1, typename T2>
    StreamOperand & operator,(const std::pair<T1, T2> &pr) {
        return *this, pr.first, ':' , pr.second;
    }

    template<typename ...Ts>
    StreamOperand & operator,(const std::tuple<Ts...> &tpl) {
        if constexpr( std::tuple_size_v<std::tuple<Ts...> > == 0 ){
            return *this;
        }else{
            *this, std::get<0>(tpl);
            return _prt_tpl<std::tuple<Ts...>, 1>(tpl);
        }
    }

    template<typename It>
    StreamOperand & operator,(const it_pair_t<It> &it_pair) {
        return _prt_range(it_pair.b, it_pair.e);
    }

    template<typename T>
    StreamOperand & operator,(const T &x) { 
        _os << x; 
        return *this; 
    }

    ostream & get_stream() const noexcept { return _os; }
private:
    ostream &_os;

    template<typename ForwardIt>
    StreamOperand & _prt_range(ForwardIt b, ForwardIt e){
        if( b != e )
            *this, *b++;
        while( b != e )
            *this, ",", *b++;
        return *this;
    }
    template<typename ForwardIt>
    StreamOperand & _prt_pair_range(ForwardIt b, ForwardIt e){
        if( b != e )
            *this, *b++;
        while( b != e )
            *this, ",", *b++;
        return *this;
    }

    template<typename Tpl, std::size_t I>
    StreamOperand & _prt_tpl(const Tpl &tpl){
        if constexpr (I == std::tuple_size_v<Tpl>){
            return *this;
        }else{
            *this, ':', std::get<I>(tpl);
            return _prt_tpl<Tpl, I+1>(tpl);
        }
    }
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

    PStream(ostream &os) noexcept: _op(os) {}
    PStream(const PStream &that) noexcept: PStream(that._op.get_stream()){}
    PStream(PStream &&that) noexcept: PStream(that._op.get_stream()) {}
    PStream & operator=(const PStream &that) =delete;
    PStream & operator=(PStream &&that) =delete;
    ~PStream() noexcept {}

    stream_op_t & operator<< (ostream& (*pf)(ostream&)) { return _op, pf; }
    stream_op_t & operator<< (std::ios& (*pf)(std::ios&)) { return _op, pf; }
    stream_op_t & operator<< (std::ios_base& (*pf)(std::ios_base&))
        { return _op, pf; }

    template<typename T>
    stream_op_t & operator<<(T &&x) { 
        return _op, std::forward<T>(x);
    }

    /**
    Objects in an iterable range defined by iterators [b, e) can be printed 
    by, e..g, 
    pout << "{", pout(b, e), "}", endl;

    This is particularly useful for the raw array, like 
    int a[N];
    int *a = new int [N];
    */
    template<typename It>
    stream_op_t::it_pair_t<It> operator()(It b, It e){ return {b, e}; }
    
    /* Return a reference to the internal std::ostream object. */
    ostream & get_stream() const noexcept { return _op.get_stream(); }
protected:
    stream_op_t _op;
};

extern PStream pout;
extern PStream perr;


} // namespace HIPP
#endif	//_HIPPCNTL_STREAM_PRETTY_H_