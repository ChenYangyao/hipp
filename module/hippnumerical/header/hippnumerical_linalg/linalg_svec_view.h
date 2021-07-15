/**
create: Yangyao CHEN, 2021/07/14
    [write   ] 
    SVecView, SVecConstView - view type for SVec.
*/

#ifndef _HIPPNUMERICAL_LINALG_SVEC_VIEW_H_
#define _HIPPNUMERICAL_LINALG_SVEC_VIEW_H_
#include "linalg_sfilter.h"
namespace HIPP::NUMERICAL {

template<typename ValueT, size_t N, typename FilterT>
class SVecView {
public:
    typedef SVec<ValueT, N> vec_t;
    typedef FilterT filter_t;
    typedef typename vec_t::value_t value_t;
    inline static constexpr size_t SIZE = vec_t::SIZE;

    /**
    Constructor.
    Filter the `vec` by the `filter`.
    */
    SVecView(vec_t &vec, const filter_t &filter) noexcept;

    SVecView(const SVecView &) noexcept = delete;
    SVecView(SVecView &&) noexcept = delete;
    ~SVecView() noexcept;

    vec_t & vec() noexcept;
    const vec_t & vec() const noexcept;
    filter_t & filter() noexcept;
    const filter_t & filter() const noexcept;

    /**
    If rhs is `vec_t`, it gets same filter.
    */
    SVecView & operator=(const value_t &rhs) noexcept;
    SVecView & operator=(const vec_t &rhs) noexcept;

    SVecView & operator+=(const value_t &rhs) noexcept;
    SVecView & operator-=(const value_t &rhs) noexcept;
    SVecView & operator*=(const value_t &rhs) noexcept;
    SVecView & operator/=(const value_t &rhs) noexcept;
    SVecView & operator%=(const value_t &rhs) noexcept;
    SVecView & operator&=(const value_t &rhs) noexcept;
    SVecView & operator|=(const value_t &rhs) noexcept;
    SVecView & operator^=(const value_t &rhs) noexcept;

    SVecView & operator+=(const vec_t &rhs) noexcept;
    SVecView & operator-=(const vec_t &rhs) noexcept;
    SVecView & operator*=(const vec_t &rhs) noexcept;
    SVecView & operator/=(const vec_t &rhs) noexcept;
    SVecView & operator%=(const vec_t &rhs) noexcept;
    SVecView & operator&=(const vec_t &rhs) noexcept;
    SVecView & operator|=(const vec_t &rhs) noexcept;
    SVecView & operator^=(const vec_t &rhs) noexcept;

    /**
    The return vector is defined by, first copying the un-filtered vector in the 
    view, then operating with another operand with the filter (binary operation)
    or apply the operation with the filter (unary operation).
    */
    friend vec_t operator+(const SVecView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] += rhs; } ); return ret; }
    friend vec_t operator-(const SVecView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] -= rhs; } ); return ret; }
    friend vec_t operator*(const SVecView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] *= rhs; } ); return ret; }
    friend vec_t operator/(const SVecView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] /= rhs; } ); return ret; }
    friend vec_t operator%(const SVecView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] %= rhs; } ); return ret; }
    friend vec_t operator&(const SVecView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] &= rhs; } ); return ret; }
    friend vec_t operator|(const SVecView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] |= rhs; } ); return ret; }
    friend vec_t operator^(const SVecView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] ^= rhs; } ); return ret; }

    friend vec_t operator+(const value_t &lhs, const SVecView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs+ret[i]; } ); return ret; }
    friend vec_t operator-(const value_t &lhs, const SVecView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs-ret[i]; } ); return ret; }
    friend vec_t operator*(const value_t &lhs, const SVecView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs*ret[i]; } ); return ret; }
    friend vec_t operator/(const value_t &lhs, const SVecView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs/ret[i]; } ); return ret; }
    friend vec_t operator%(const value_t &lhs, const SVecView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs%ret[i]; } ); return ret; }
    friend vec_t operator&(const value_t &lhs, const SVecView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs&ret[i]; } ); return ret; }
    friend vec_t operator|(const value_t &lhs, const SVecView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs|ret[i]; } ); return ret; }
    friend vec_t operator^(const value_t &lhs, const SVecView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs^ret[i]; } ); return ret; }

    friend vec_t operator+(const SVecView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] += rhs[i]; } ); return ret; }
    friend vec_t operator-(const SVecView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] -= rhs[i]; } ); return ret; }
    friend vec_t operator*(const SVecView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] *= rhs[i]; } ); return ret; }
    friend vec_t operator/(const SVecView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] /= rhs[i]; } ); return ret; }
    friend vec_t operator%(const SVecView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] %= rhs[i]; } ); return ret; }
    friend vec_t operator&(const SVecView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] &= rhs[i]; } ); return ret; }
    friend vec_t operator|(const SVecView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] |= rhs[i]; } ); return ret; }
    friend vec_t operator^(const SVecView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] ^= rhs[i]; } ); return ret; }

    friend vec_t operator+(const vec_t &lhs, const SVecView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]+ret[i]; } ); return ret; }
    friend vec_t operator-(const vec_t &lhs, const SVecView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]-ret[i]; } ); return ret; }
    friend vec_t operator*(const vec_t &lhs, const SVecView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]*ret[i]; } ); return ret; }
    friend vec_t operator/(const vec_t &lhs, const SVecView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]/ret[i]; } ); return ret; }
    friend vec_t operator%(const vec_t &lhs, const SVecView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]%ret[i]; } ); return ret; }
    friend vec_t operator&(const vec_t &lhs, const SVecView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]&ret[i]; } ); return ret; }
    friend vec_t operator|(const vec_t &lhs, const SVecView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]|ret[i]; } ); return ret; }
    friend vec_t operator^(const vec_t &lhs, const SVecView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]^ret[i]; } ); return ret; }

    vec_t operator+() const noexcept;
    vec_t operator-() const noexcept;
    vec_t operator~() const noexcept;

    template<typename ResT = value_t>
    ResT sum() const noexcept;
    template<typename ResT = value_t>
    ResT prod() const noexcept;
    template<typename ResT = value_t>
    ResT mean() const noexcept;

    bool all() const noexcept;
    bool any() const noexcept;

    template<typename UnaryOp>
    SVecView & map(UnaryOp op);

    template<typename UnaryOp, 
        typename ResT = std::invoke_result_t<UnaryOp, value_t> >
    SVec<ResT, SIZE> mapped(UnaryOp op) const;

    template<typename BinaryOp>
    void visit(BinaryOp op) const;
    
    template<typename BinaryOp>
    void visit(BinaryOp op);
protected:
    vec_t &_vec;
    filter_t _filter;
};

#define _HIPP_TEMPHD \
    template<typename ValueT, size_t N, typename FilterT>
#define _HIPP_TEMPARG \
    <ValueT, N, FilterT>
#define _HIPP_TEMPRET \
    _HIPP_TEMPHD \
    inline auto SVecView _HIPP_TEMPARG::
#define _HIPP_TEMPCLS \
    SVecView _HIPP_TEMPARG

_HIPP_TEMPHD
_HIPP_TEMPCLS::SVecView(vec_t &vec, const filter_t &filter) noexcept
: _vec(vec), _filter(filter) { }

_HIPP_TEMPHD
_HIPP_TEMPCLS::~SVecView() noexcept {}

_HIPP_TEMPRET vec() noexcept -> vec_t & { 
    return _vec;
}

_HIPP_TEMPRET vec() const noexcept -> const vec_t & { 
    return _vec;
}

_HIPP_TEMPRET filter() noexcept -> filter_t & { 
    return _filter;
}

_HIPP_TEMPRET filter() const noexcept -> const filter_t & { 
    return _filter;
}

_HIPP_TEMPRET operator=(const value_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] = rhs; }
    );
    return *this;
}

_HIPP_TEMPRET operator=(const vec_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] = rhs[i]; }
    );
    return *this;
}

_HIPP_TEMPRET operator+=(const value_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] += rhs; } 
    );
    return *this;
}
_HIPP_TEMPRET operator-=(const value_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] -= rhs; } 
    );
    return *this;
}

_HIPP_TEMPRET operator*=(const value_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] *= rhs; } 
    );
    return *this;
}

_HIPP_TEMPRET operator/=(const value_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] /= rhs; } 
    );
    return *this;
}
_HIPP_TEMPRET operator%=(const value_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] %= rhs; } 
    );
    return *this;
}

_HIPP_TEMPRET operator&=(const value_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] &= rhs; } 
    );
    return *this;
}

_HIPP_TEMPRET operator|=(const value_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] |= rhs; } 
    );
    return *this;
}
_HIPP_TEMPRET operator^=(const value_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] ^= rhs; } 
    );
    return *this;
}

_HIPP_TEMPRET operator+=(const vec_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] += rhs[i]; } 
    );
    return *this;
}

_HIPP_TEMPRET operator-=(const vec_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] -= rhs[i]; } 
    );
    return *this;
}

_HIPP_TEMPRET operator*=(const vec_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] *= rhs[i]; } 
    );
    return *this;
}

_HIPP_TEMPRET operator/=(const vec_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] /= rhs[i]; } 
    );
    return *this;
}
_HIPP_TEMPRET operator%=(const vec_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] %= rhs[i]; } 
    );
    return *this;
}

_HIPP_TEMPRET operator&=(const vec_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] &= rhs[i]; } 
    );
    return *this;
}

_HIPP_TEMPRET operator|=(const vec_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] |= rhs[i]; } 
    );
    return *this;
}

_HIPP_TEMPRET operator^=(const vec_t &rhs) noexcept -> SVecView & {
     _filter.visit( 
         [&](size_t i){ _vec[i] ^= rhs[i]; } 
    );
    return *this;
}

_HIPP_TEMPRET operator+() const noexcept -> vec_t { 
    vec_t ret(_vec); 
    _filter.visit(
        [&](size_t i){ ret[i] = +ret[i]; }
    ); 
    return ret; 
}
_HIPP_TEMPRET operator-() const noexcept -> vec_t { 
    vec_t ret(_vec); 
    _filter.visit(
        [&](size_t i){ ret[i] = -ret[i]; }
    ); 
    return ret; 
}

_HIPP_TEMPRET operator~() const noexcept -> vec_t { 
    vec_t ret(_vec); 
    _filter.visit(
        [&](size_t i){ ret[i] = ~ret[i]; }
    ); 
    return ret; 
}

_HIPP_TEMPHD 
template<typename ResT>
auto _HIPP_TEMPCLS::sum() const noexcept -> ResT { 
    ResT ret {0}; 
    _filter.visit( 
        [&](size_t i){ret += _vec[i];}
    ); 
    return ret; 
}

_HIPP_TEMPHD 
template<typename ResT>
auto _HIPP_TEMPCLS::prod() const noexcept -> ResT { 
    ResT ret {1}; 
    _filter.visit( 
        [&](size_t i){ret *= _vec[i];}
    );
    return ret; 
}

_HIPP_TEMPHD 
template<typename ResT>
auto _HIPP_TEMPCLS::mean() const noexcept -> ResT { 
    static_assert(SIZE != 0); return sum<ResT>() / static_cast<ResT>(SIZE); 
}

_HIPP_TEMPRET all() const noexcept -> bool { 
    bool ret = true;  
    _filter.visit(
        [&](size_t i){ ret = ret && _vec[i]; }
    ); 
    return ret; 
}

_HIPP_TEMPRET any() const noexcept -> bool { 
    bool ret = false; 
    _filter.visit(
        [&](size_t i){ ret = ret || _vec[i]; }
    ); 
    return ret; 
}

_HIPP_TEMPHD 
template<typename UnaryOp>
auto _HIPP_TEMPCLS::map(UnaryOp op) -> SVecView & { 
    _filter.visit(
        [&](size_t i){ _vec[i] = op(_vec[i]); }
    );
    return *this;
}

_HIPP_TEMPHD 
template<typename UnaryOp, typename ResT>
auto _HIPP_TEMPCLS::mapped(UnaryOp op) const -> SVec<ResT, SIZE> { 
    SVec<ResT, SIZE> ret(_vec);
    _filter.visit(
        [&](size_t i){ ret[i] = op(ret[i]); }
    );
    return ret;
}

_HIPP_TEMPHD 
template<typename BinaryOp>
auto _HIPP_TEMPCLS::visit(BinaryOp op) const -> void { 
    _filter.visit([&](size_t i){ op(i, _vec[i]); });
}

_HIPP_TEMPHD 
template<typename BinaryOp>
auto _HIPP_TEMPCLS::visit(BinaryOp op) -> void { 
    _filter.visit(
        [&](size_t i){ op(i, _vec[i]); }
    );
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPCLS

template<typename ValueT, size_t N, typename FilterT>
class SVecConstView {
public:
    typedef SVec<ValueT, N> vec_t;
    typedef FilterT filter_t;
    typedef typename vec_t::value_t value_t;
    inline static constexpr size_t SIZE = vec_t::SIZE;

    SVecConstView(const vec_t &vec, const filter_t &filter) noexcept;

    SVecConstView(const SVecConstView &) noexcept = delete;
    SVecConstView(SVecConstView &&) noexcept = delete;
    ~SVecConstView() noexcept;

    const vec_t & vec() const noexcept;
    filter_t & filter() noexcept;
    const filter_t & filter() const noexcept;

    friend vec_t operator+(const SVecConstView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] += rhs; } ); return ret; }
    friend vec_t operator-(const SVecConstView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] -= rhs; } ); return ret; }
    friend vec_t operator*(const SVecConstView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] *= rhs; } ); return ret; }
    friend vec_t operator/(const SVecConstView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] /= rhs; } ); return ret; }
    friend vec_t operator%(const SVecConstView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] %= rhs; } ); return ret; }
    friend vec_t operator&(const SVecConstView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] &= rhs; } ); return ret; }
    friend vec_t operator|(const SVecConstView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] |= rhs; } ); return ret; }
    friend vec_t operator^(const SVecConstView &lhs, const value_t &rhs) noexcept    { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] ^= rhs; } ); return ret; }

    friend vec_t operator+(const value_t &lhs, const SVecConstView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs+ret[i]; } ); return ret; }
    friend vec_t operator-(const value_t &lhs, const SVecConstView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs-ret[i]; } ); return ret; }
    friend vec_t operator*(const value_t &lhs, const SVecConstView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs*ret[i]; } ); return ret; }
    friend vec_t operator/(const value_t &lhs, const SVecConstView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs/ret[i]; } ); return ret; }
    friend vec_t operator%(const value_t &lhs, const SVecConstView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs%ret[i]; } ); return ret; }
    friend vec_t operator&(const value_t &lhs, const SVecConstView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs&ret[i]; } ); return ret; }
    friend vec_t operator|(const value_t &lhs, const SVecConstView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs|ret[i]; } ); return ret; }
    friend vec_t operator^(const value_t &lhs, const SVecConstView &rhs) noexcept    { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs^ret[i]; } ); return ret; }

    friend vec_t operator+(const SVecConstView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] += rhs[i]; } ); return ret; }
    friend vec_t operator-(const SVecConstView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] -= rhs[i]; } ); return ret; }
    friend vec_t operator*(const SVecConstView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] *= rhs[i]; } ); return ret; }
    friend vec_t operator/(const SVecConstView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] /= rhs[i]; } ); return ret; }
    friend vec_t operator%(const SVecConstView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] %= rhs[i]; } ); return ret; }
    friend vec_t operator&(const SVecConstView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] &= rhs[i]; } ); return ret; }
    friend vec_t operator|(const SVecConstView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] |= rhs[i]; } ); return ret; }
    friend vec_t operator^(const SVecConstView &lhs, const vec_t &rhs) noexcept      { vec_t ret(lhs._vec); lhs._filter.visit( [&](size_t i){ ret[i] ^= rhs[i]; } ); return ret; }

    friend vec_t operator+(const vec_t &lhs, const SVecConstView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]+ret[i]; } ); return ret; }
    friend vec_t operator-(const vec_t &lhs, const SVecConstView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]-ret[i]; } ); return ret; }
    friend vec_t operator*(const vec_t &lhs, const SVecConstView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]*ret[i]; } ); return ret; }
    friend vec_t operator/(const vec_t &lhs, const SVecConstView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]/ret[i]; } ); return ret; }
    friend vec_t operator%(const vec_t &lhs, const SVecConstView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]%ret[i]; } ); return ret; }
    friend vec_t operator&(const vec_t &lhs, const SVecConstView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]&ret[i]; } ); return ret; }
    friend vec_t operator|(const vec_t &lhs, const SVecConstView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]|ret[i]; } ); return ret; }
    friend vec_t operator^(const vec_t &lhs, const SVecConstView &rhs) noexcept      { vec_t ret(rhs._vec); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]^ret[i]; } ); return ret; }

    vec_t operator+() const noexcept;
    vec_t operator-() const noexcept;
    vec_t operator~() const noexcept;

    template<typename ResT = value_t>
    ResT sum() const noexcept;
    template<typename ResT = value_t>
    ResT prod() const noexcept;
    template<typename ResT = value_t>
    ResT mean() const noexcept;

    bool all() const noexcept;
    bool any() const noexcept;

    template<typename UnaryOp, 
        typename ResT = std::invoke_result_t<UnaryOp, value_t> >
    SVec<ResT, SIZE> mapped(UnaryOp op) const;

    template<typename BinaryOp>
    void visit(BinaryOp op) const;
protected:
    const vec_t &_vec;
    filter_t _filter;
};

#define _HIPP_TEMPHD \
    template<typename ValueT, size_t N, typename FilterT>
#define _HIPP_TEMPARG \
    <ValueT, N, FilterT>
#define _HIPP_TEMPRET \
    _HIPP_TEMPHD \
    inline auto SVecConstView _HIPP_TEMPARG::
#define _HIPP_TEMPCLS \
    SVecConstView _HIPP_TEMPARG

_HIPP_TEMPHD
_HIPP_TEMPCLS::SVecConstView(const vec_t &vec, const filter_t &filter) noexcept
: _vec(vec), _filter(filter) {}

_HIPP_TEMPHD
_HIPP_TEMPCLS::~SVecConstView() noexcept {}

_HIPP_TEMPRET vec() const noexcept -> const vec_t &  {
    return _vec;
}

_HIPP_TEMPRET filter() noexcept -> filter_t &  {
    return _filter;
}

_HIPP_TEMPRET filter() const noexcept -> const filter_t &  {
    return _filter;
}

_HIPP_TEMPRET operator+() const noexcept -> vec_t { 
    vec_t ret(_vec); 
    _filter.visit(
        [&](size_t i){ ret[i] = +ret[i]; }
    );
    return ret; 
}

_HIPP_TEMPRET operator-() const noexcept -> vec_t { 
    vec_t ret(_vec); 
    _filter.visit(
        [&](size_t i){ ret[i] = -ret[i]; }
    );
    return ret; 
}

_HIPP_TEMPRET operator~() const noexcept -> vec_t { 
    vec_t ret(_vec); 
    _filter.visit(
        [&](size_t i){ ret[i] = ~ret[i]; }
    );
    return ret; 
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::sum() const noexcept -> ResT { 
    ResT ret {0}; _filter.visit( [&](size_t i){ret += _vec[i];} );
    return ret;
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::prod() const noexcept -> ResT { 
    ResT ret {1}; _filter.visit( [&](size_t i){ret *= _vec[i];} );
    return ret;
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::mean() const noexcept -> ResT { 
    static_assert(SIZE != 0);
    return sum<ResT>() / static_cast<ResT>(SIZE);
}

_HIPP_TEMPRET all() const noexcept -> bool { 
    bool ret = true;
    _filter.visit(
        [&](size_t i){ ret = ret && _vec[i]; }
    );
    return ret; 
}

_HIPP_TEMPRET any() const noexcept -> bool { 
    bool ret = false;
    _filter.visit(
        [&](size_t i){ ret = ret || _vec[i]; }
    );
    return ret; 
}

_HIPP_TEMPHD 
template<typename UnaryOp, typename ResT>
auto _HIPP_TEMPCLS::mapped(UnaryOp op) const -> SVec<ResT, SIZE>{ 
    SVec<ResT, SIZE> ret(_vec);
    _filter.visit(
        [&](size_t i){ ret[i] = op(ret[i]); }
    );
    return ret; 
}

_HIPP_TEMPHD 
template<typename BinaryOp>
auto _HIPP_TEMPCLS::visit(BinaryOp op) const -> void{ 
    _filter.visit(
        [&](size_t i){ op(i, _vec[i]); }
    ); 
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPCLS

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_LINALG_SVEC_VIEW_H_