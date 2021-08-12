/**
create: Yangyao CHEN, 2021/07/14
    [write   ] 
    SArrayView, SArrayConstView - view type for SArray.
*/

#ifndef _HIPPNUMERICAL_LINALG_SARRAY_VIEW_H_
#define _HIPPNUMERICAL_LINALG_SARRAY_VIEW_H_
#include "linalg_sfilter.h"
namespace HIPP::NUMERICAL {

/**
A view of a SArray.
The view refers to a SArray, with a filter selecting a part of its elements.
Operations, like arithmetics, reductions, map, visit can be applied to the 
selected elements.
*/
template<typename FilterT, typename ValueT, size_t ...Ds>
class SArrayView {
public:
    static_assert(std::is_base_of_v<SFilter, FilterT>, 
        "FilterT is not a subclass of SFilter");
    
    typedef SArray<ValueT, Ds...> array_t;
    typedef FilterT filter_t;
    typedef typename array_t::value_t value_t;
    inline static constexpr size_t SIZE = array_t::SIZE;

    /**
    Constructor.
    Filter the SArray ``a`` by the ``filter``.
    */
    SArrayView(array_t &a, const filter_t &filter) noexcept;

    SArrayView(const SArrayView &) noexcept = delete;
    SArrayView(SArrayView &&) noexcept = delete;
    ~SArrayView() noexcept;

    /* Get the array that the view refers to. */
    array_t & array() noexcept;

    /* Get the filter used for element selection. */
    const array_t & array() const noexcept;
    filter_t & filter() noexcept;
    const filter_t & filter() const noexcept;

    /**
    Set the selected elements of the referred array to ``rhs``.
    If ``rhs`` is a SArray ``array_t``, it gets the same filter and then is 
    assigned to the view element-wisely.
    */
    SArrayView & operator=(const value_t &rhs) noexcept;
    SArrayView & operator=(const array_t &rhs) noexcept;

    /**
    Element-wise arithmetic and logic operations with ``rhs``.
    If ``rhs`` is a SArray ``array_t``, it gets the same filter and then is 
    operated with the view element-wisely.
    */
    SArrayView & operator+=(const value_t &rhs) noexcept;
    SArrayView & operator-=(const value_t &rhs) noexcept;
    SArrayView & operator*=(const value_t &rhs) noexcept;
    SArrayView & operator/=(const value_t &rhs) noexcept;
    SArrayView & operator%=(const value_t &rhs) noexcept;
    SArrayView & operator&=(const value_t &rhs) noexcept;
    SArrayView & operator|=(const value_t &rhs) noexcept;
    SArrayView & operator^=(const value_t &rhs) noexcept;

    SArrayView & operator+=(const array_t &rhs) noexcept;
    SArrayView & operator-=(const array_t &rhs) noexcept;
    SArrayView & operator*=(const array_t &rhs) noexcept;
    SArrayView & operator/=(const array_t &rhs) noexcept;
    SArrayView & operator%=(const array_t &rhs) noexcept;
    SArrayView & operator&=(const array_t &rhs) noexcept;
    SArrayView & operator|=(const array_t &rhs) noexcept;
    SArrayView & operator^=(const array_t &rhs) noexcept;

    /**
    The return vector is defined by, first copying the un-filtered vector in the 
    view, then operating with another operand with the same filter 
    (binary operation) or apply the operation with the filter (unary operation).
    */
    friend array_t operator+(const SArrayView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] += rhs; } ); return ret; }
    friend array_t operator-(const SArrayView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] -= rhs; } ); return ret; }
    friend array_t operator*(const SArrayView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] *= rhs; } ); return ret; }
    friend array_t operator/(const SArrayView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] /= rhs; } ); return ret; }
    friend array_t operator%(const SArrayView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] %= rhs; } ); return ret; }
    friend array_t operator&(const SArrayView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] &= rhs; } ); return ret; }
    friend array_t operator|(const SArrayView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] |= rhs; } ); return ret; }
    friend array_t operator^(const SArrayView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] ^= rhs; } ); return ret; }

    friend array_t operator+(const value_t &lhs, const SArrayView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs+ret[i]; } ); return ret; }
    friend array_t operator-(const value_t &lhs, const SArrayView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs-ret[i]; } ); return ret; }
    friend array_t operator*(const value_t &lhs, const SArrayView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs*ret[i]; } ); return ret; }
    friend array_t operator/(const value_t &lhs, const SArrayView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs/ret[i]; } ); return ret; }
    friend array_t operator%(const value_t &lhs, const SArrayView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs%ret[i]; } ); return ret; }
    friend array_t operator&(const value_t &lhs, const SArrayView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs&ret[i]; } ); return ret; }
    friend array_t operator|(const value_t &lhs, const SArrayView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs|ret[i]; } ); return ret; }
    friend array_t operator^(const value_t &lhs, const SArrayView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs^ret[i]; } ); return ret; }

    friend array_t operator+(const SArrayView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] += rhs[i]; } ); return ret; }
    friend array_t operator-(const SArrayView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] -= rhs[i]; } ); return ret; }
    friend array_t operator*(const SArrayView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] *= rhs[i]; } ); return ret; }
    friend array_t operator/(const SArrayView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] /= rhs[i]; } ); return ret; }
    friend array_t operator%(const SArrayView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] %= rhs[i]; } ); return ret; }
    friend array_t operator&(const SArrayView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] &= rhs[i]; } ); return ret; }
    friend array_t operator|(const SArrayView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] |= rhs[i]; } ); return ret; }
    friend array_t operator^(const SArrayView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] ^= rhs[i]; } ); return ret; }

    friend array_t operator+(const array_t &lhs, const SArrayView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]+ret[i]; } ); return ret; }
    friend array_t operator-(const array_t &lhs, const SArrayView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]-ret[i]; } ); return ret; }
    friend array_t operator*(const array_t &lhs, const SArrayView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]*ret[i]; } ); return ret; }
    friend array_t operator/(const array_t &lhs, const SArrayView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]/ret[i]; } ); return ret; }
    friend array_t operator%(const array_t &lhs, const SArrayView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]%ret[i]; } ); return ret; }
    friend array_t operator&(const array_t &lhs, const SArrayView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]&ret[i]; } ); return ret; }
    friend array_t operator|(const array_t &lhs, const SArrayView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]|ret[i]; } ); return ret; }
    friend array_t operator^(const array_t &lhs, const SArrayView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]^ret[i]; } ); return ret; }

    array_t operator+() const noexcept;
    array_t operator-() const noexcept;
    array_t operator~() const noexcept;

    /**
    Reduction operations.
    sum(), prod(), mean() - the summation, product, and mean of all elements 
    selected by the view.

    all(), any() - all true or any true of all elements selected by the view.
    */
    template<typename ResT = value_t>
    ResT sum() const noexcept;
    template<typename ResT = value_t>
    ResT prod() const noexcept;
    template<typename ResT = value_t>
    ResT mean() const noexcept;

    bool all() const noexcept;
    bool any() const noexcept;

    /**
    Map and visit operations.
    map() - for each ``size_t i`` of selected elements, call 
    ``self[i] = op(self[i])``.
    mapped() - returns a mapped copy, i.e., copy the entire array 
    (including unselected elements), then map the selected elements, and 
    return the new array.
    visit() - for each ``size_t i`` of selected elements, call 
    ``op(i, self[i])``.
    */
    template<typename UnaryOp>
    SArrayView & map(UnaryOp op);

    template<typename UnaryOp, 
        typename ResT = std::invoke_result_t<UnaryOp, value_t> >
    SArray<ResT, Ds...> mapped(UnaryOp op) const;

    template<typename BinaryOp>
    void visit(BinaryOp op) const;
    
    template<typename BinaryOp>
    void visit(BinaryOp op);
protected:
    array_t &_array;
    filter_t _filter;
};

#define _HIPP_TEMPHD \
    template<typename FilterT, typename ValueT, size_t ...Ds>
#define _HIPP_TEMPARG \
    <FilterT, ValueT, Ds...>
#define _HIPP_TEMPRET \
    _HIPP_TEMPHD \
    inline auto SArrayView _HIPP_TEMPARG::
#define _HIPP_TEMPCLS \
    SArrayView _HIPP_TEMPARG

_HIPP_TEMPHD
_HIPP_TEMPCLS::SArrayView(array_t &vec, const filter_t &filter) noexcept
: _array(vec), _filter(filter) { }

_HIPP_TEMPHD
_HIPP_TEMPCLS::~SArrayView() noexcept {}

_HIPP_TEMPRET array() noexcept -> array_t & { 
    return _array;
}

_HIPP_TEMPRET array() const noexcept -> const array_t & { 
    return _array;
}

_HIPP_TEMPRET filter() noexcept -> filter_t & { 
    return _filter;
}

_HIPP_TEMPRET filter() const noexcept -> const filter_t & { 
    return _filter;
}

#define _HIPP_UNARY_OP_DEF(op) \
    _HIPP_TEMPRET operator op(const value_t &rhs) noexcept -> SArrayView & { \
        _filter.visit(  \
            [&](size_t i){ _array[i] op rhs; } \
        ); \
        return *this; \
    } \
    _HIPP_TEMPRET operator op(const array_t &rhs) noexcept -> SArrayView & { \
        _filter.visit(  \
            [&](size_t i){ _array[i] op rhs[i]; } \
        ); \
        return *this; \
    }
_HIPP_UNARY_OP_DEF(=)
_HIPP_UNARY_OP_DEF(+=)
_HIPP_UNARY_OP_DEF(-=)
_HIPP_UNARY_OP_DEF(*=)
_HIPP_UNARY_OP_DEF(/=)
_HIPP_UNARY_OP_DEF(%=)
_HIPP_UNARY_OP_DEF(&=)
_HIPP_UNARY_OP_DEF(|=)
_HIPP_UNARY_OP_DEF(^=)

#undef _HIPP_UNARY_OP_DEF

#define _HIPP_UNARY_OP_DEF(op) \
    _HIPP_TEMPRET operator op() const noexcept -> array_t {  \
        array_t ret(_array);  \
        _filter.visit( \
            [&](size_t i){ ret[i] =  op ret[i]; } \
        );  \
        return ret;  \
    }

_HIPP_UNARY_OP_DEF(+)
_HIPP_UNARY_OP_DEF(-)
_HIPP_UNARY_OP_DEF(~)

#undef _HIPP_UNARY_OP_DEF

_HIPP_TEMPHD 
template<typename ResT>
auto _HIPP_TEMPCLS::sum() const noexcept -> ResT { 
    ResT ret {0}; 
    _filter.visit( 
        [&](size_t i){ret += _array[i];}
    ); 
    return ret; 
}

_HIPP_TEMPHD 
template<typename ResT>
auto _HIPP_TEMPCLS::prod() const noexcept -> ResT { 
    ResT ret {1}; 
    _filter.visit( 
        [&](size_t i){ret *= _array[i];}
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
        [&](size_t i){ ret = ret && _array[i]; }
    ); 
    return ret; 
}

_HIPP_TEMPRET any() const noexcept -> bool { 
    bool ret = false; 
    _filter.visit(
        [&](size_t i){ ret = ret || _array[i]; }
    ); 
    return ret; 
}

_HIPP_TEMPHD 
template<typename UnaryOp>
auto _HIPP_TEMPCLS::map(UnaryOp op) -> SArrayView & { 
    _filter.visit(
        [&](size_t i){ _array[i] = op(_array[i]); }
    );
    return *this;
}

_HIPP_TEMPHD 
template<typename UnaryOp, typename ResT>
auto _HIPP_TEMPCLS::mapped(UnaryOp op) const -> SArray<ResT, Ds...> { 
    SArray<ResT, SIZE> ret(_array);
    _filter.visit(
        [&](size_t i){ ret[i] = op(ret[i]); }
    );
    return ret;
}

_HIPP_TEMPHD 
template<typename BinaryOp>
auto _HIPP_TEMPCLS::visit(BinaryOp op) const -> void { 
    _filter.visit([&](size_t i){ op(i, _array[i]); });
}

_HIPP_TEMPHD 
template<typename BinaryOp>
auto _HIPP_TEMPCLS::visit(BinaryOp op) -> void { 
    _filter.visit(
        [&](size_t i){ op(i, _array[i]); }
    );
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPCLS

template<typename FilterT, typename ValueT, size_t ...Ds>
class SArrayConstView {
public:
    typedef SArray<ValueT, Ds...> array_t;
    typedef FilterT filter_t;
    typedef typename array_t::value_t value_t;
    inline static constexpr size_t SIZE = array_t::SIZE;

    SArrayConstView(const array_t &a, const filter_t &filter) noexcept;

    SArrayConstView(const SArrayConstView &) noexcept = delete;
    SArrayConstView(SArrayConstView &&) noexcept = delete;
    ~SArrayConstView() noexcept;

    const array_t & array() const noexcept;
    filter_t & filter() noexcept;
    const filter_t & filter() const noexcept;

    friend array_t operator+(const SArrayConstView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] += rhs; } ); return ret; }
    friend array_t operator-(const SArrayConstView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] -= rhs; } ); return ret; }
    friend array_t operator*(const SArrayConstView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] *= rhs; } ); return ret; }
    friend array_t operator/(const SArrayConstView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] /= rhs; } ); return ret; }
    friend array_t operator%(const SArrayConstView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] %= rhs; } ); return ret; }
    friend array_t operator&(const SArrayConstView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] &= rhs; } ); return ret; }
    friend array_t operator|(const SArrayConstView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] |= rhs; } ); return ret; }
    friend array_t operator^(const SArrayConstView &lhs, const value_t &rhs) noexcept    { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] ^= rhs; } ); return ret; }

    friend array_t operator+(const value_t &lhs, const SArrayConstView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs+ret[i]; } ); return ret; }
    friend array_t operator-(const value_t &lhs, const SArrayConstView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs-ret[i]; } ); return ret; }
    friend array_t operator*(const value_t &lhs, const SArrayConstView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs*ret[i]; } ); return ret; }
    friend array_t operator/(const value_t &lhs, const SArrayConstView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs/ret[i]; } ); return ret; }
    friend array_t operator%(const value_t &lhs, const SArrayConstView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs%ret[i]; } ); return ret; }
    friend array_t operator&(const value_t &lhs, const SArrayConstView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs&ret[i]; } ); return ret; }
    friend array_t operator|(const value_t &lhs, const SArrayConstView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs|ret[i]; } ); return ret; }
    friend array_t operator^(const value_t &lhs, const SArrayConstView &rhs) noexcept    { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs^ret[i]; } ); return ret; }

    friend array_t operator+(const SArrayConstView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] += rhs[i]; } ); return ret; }
    friend array_t operator-(const SArrayConstView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] -= rhs[i]; } ); return ret; }
    friend array_t operator*(const SArrayConstView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] *= rhs[i]; } ); return ret; }
    friend array_t operator/(const SArrayConstView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] /= rhs[i]; } ); return ret; }
    friend array_t operator%(const SArrayConstView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] %= rhs[i]; } ); return ret; }
    friend array_t operator&(const SArrayConstView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] &= rhs[i]; } ); return ret; }
    friend array_t operator|(const SArrayConstView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] |= rhs[i]; } ); return ret; }
    friend array_t operator^(const SArrayConstView &lhs, const array_t &rhs) noexcept      { array_t ret(lhs._array); lhs._filter.visit( [&](size_t i){ ret[i] ^= rhs[i]; } ); return ret; }

    friend array_t operator+(const array_t &lhs, const SArrayConstView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]+ret[i]; } ); return ret; }
    friend array_t operator-(const array_t &lhs, const SArrayConstView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]-ret[i]; } ); return ret; }
    friend array_t operator*(const array_t &lhs, const SArrayConstView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]*ret[i]; } ); return ret; }
    friend array_t operator/(const array_t &lhs, const SArrayConstView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]/ret[i]; } ); return ret; }
    friend array_t operator%(const array_t &lhs, const SArrayConstView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]%ret[i]; } ); return ret; }
    friend array_t operator&(const array_t &lhs, const SArrayConstView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]&ret[i]; } ); return ret; }
    friend array_t operator|(const array_t &lhs, const SArrayConstView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]|ret[i]; } ); return ret; }
    friend array_t operator^(const array_t &lhs, const SArrayConstView &rhs) noexcept      { array_t ret(rhs._array); rhs._filter.visit( [&](size_t i){ ret[i] = lhs[i]^ret[i]; } ); return ret; }

    array_t operator+() const noexcept;
    array_t operator-() const noexcept;
    array_t operator~() const noexcept;

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
    SArray<ResT, Ds...> mapped(UnaryOp op) const;

    template<typename BinaryOp>
    void visit(BinaryOp op) const;
protected:
    const array_t &_array;
    filter_t _filter;
};

#define _HIPP_TEMPHD \
    template<typename FilterT, typename ValueT, size_t ...Ds>
#define _HIPP_TEMPARG \
    <FilterT, ValueT, Ds...>
#define _HIPP_TEMPRET \
    _HIPP_TEMPHD \
    inline auto SArrayConstView _HIPP_TEMPARG::
#define _HIPP_TEMPCLS \
    SArrayConstView _HIPP_TEMPARG

_HIPP_TEMPHD
_HIPP_TEMPCLS::SArrayConstView(
    const array_t &vec, const filter_t &filter) noexcept
: _array(vec), _filter(filter) {}

_HIPP_TEMPHD
_HIPP_TEMPCLS::~SArrayConstView() noexcept {}

_HIPP_TEMPRET array() const noexcept -> const array_t &  {
    return _array;
}

_HIPP_TEMPRET filter() noexcept -> filter_t &  {
    return _filter;
}

_HIPP_TEMPRET filter() const noexcept -> const filter_t &  {
    return _filter;
}

#define _HIPP_UNARY_OP_DEF(op) \
_HIPP_TEMPRET operator op() const noexcept -> array_t {  \
    array_t ret(_array);  \
    _filter.visit( \
        [&](size_t i){ ret[i] = op ret[i]; } \
    ); \
    return ret;  \
}

_HIPP_UNARY_OP_DEF(+)
_HIPP_UNARY_OP_DEF(-)
_HIPP_UNARY_OP_DEF(~)

#undef  _HIPP_UNARY_OP_DEF

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::sum() const noexcept -> ResT { 
    ResT ret {0};
    _filter.visit( [&](size_t i){ret += _array[i];} );
    return ret;
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::prod() const noexcept -> ResT { 
    ResT ret {1};
    _filter.visit( [&](size_t i){ret *= _array[i];} );
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
        [&](size_t i){ ret = ret && _array[i]; }
    );
    return ret; 
}

_HIPP_TEMPRET any() const noexcept -> bool { 
    bool ret = false;
    _filter.visit(
        [&](size_t i){ ret = ret || _array[i]; }
    );
    return ret; 
}

_HIPP_TEMPHD 
template<typename UnaryOp, typename ResT>
auto _HIPP_TEMPCLS::mapped(UnaryOp op) const -> SArray<ResT, Ds...>{ 
    SArray<ResT, SIZE> ret(_array);
    _filter.visit(
        [&](size_t i){ ret[i] = op(ret[i]); }
    );
    return ret; 
}

_HIPP_TEMPHD 
template<typename BinaryOp>
auto _HIPP_TEMPCLS::visit(BinaryOp op) const -> void{ 
    _filter.visit(
        [&](size_t i){ op(i, _array[i]); }
    ); 
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPCLS

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_LINALG_SARRAY_VIEW_H_