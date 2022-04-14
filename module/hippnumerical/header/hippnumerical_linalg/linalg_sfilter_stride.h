/**
create: Yangyao CHEN, 2021/07/21
    [write   ] SStrideFilter - filter using stride at each dimension.
*/

#ifndef _HIPPNUMERICAL_LINALG_SFILTER_STRIDE_H_
#define _HIPPNUMERICAL_LINALG_SFILTER_STRIDE_H_

#include "linalg_sfilter_base.h"

#define _HIPP_TEMPHD template<size_t ...Ds>
#define _HIPP_TEMPARG <Ds...>
#define _HIPP_TEMPCLS SStrideFilter _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

namespace HIPP::NUMERICAL {

template<size_t ...Ds>
class SStrideFilter: public SFilter {
public:
    inline static constexpr size_t RANK = sizeof...(Ds);
    typedef SVec<size_t, 3> stride_1d_t;
    typedef SArray<size_t, RANK, 3> strides_t;

    static_assert(RANK>=1, "RANK must be >= 1");

    /**
    Constructors.
    
    (1) select all.
    
    (2) select from 1-D stride at each dimension. 
    @s: each of `s` is used to construct a stride_1d_t, which marks the 
        begin, the end, and the step size.
        
        Each `s` could be one of the following types:
        - s_all_t, s_one_t, s_range_t, s_head_t, s_tail_t or s_none_t in 
            SArrayBase.
        - stride_1d_t.

    (3) the same as (2), but specify all the strides using an array, each row
        of which marks the begin, the end, and the step size.
    */
    SStrideFilter() noexcept;
    template<typename ...Stride1Ds>
    explicit SStrideFilter(Stride1Ds &&...s) noexcept;
    explicit SStrideFilter(const strides_t &s) noexcept;

    SStrideFilter(const SStrideFilter &) noexcept = default;
    SStrideFilter(SStrideFilter &&) noexcept = default;
    SStrideFilter & operator=(const SStrideFilter &) noexcept = default;
    SStrideFilter & operator=(SStrideFilter &&) noexcept = default;
    ~SStrideFilter() noexcept;
    friend void swap(SStrideFilter &lhs, SStrideFilter &rhs) noexcept           { swap(lhs._strides, rhs._strides); }

    /**
    Visit all selected elements, i.e., for each index `size_t i` of selected 
    element, call 
        op(i);
    The second version call binary 
        op(i, b[i]);
    */
    template<typename UnaryOp>
    void visit(UnaryOp op) const;
    template<typename BinaryOp, typename RandomAccessIt>
    void visit(BinaryOp op, RandomAccessIt b) const;
protected:
    typedef typename SArray<bool, Ds...>::traits_t _raw_traits_t;
    inline static constexpr std::array<size_t, RANK> 
        _raw_strides = _raw_traits_t::strides,
        _raw_extents = _raw_traits_t::extents;

    strides_t _strides;

    template<typename UnaryOp, size_t D>
    void _visit_at_dim(size_t cum_id, UnaryOp &op) const;

    template<typename Stride1D, typename ...Stride1Ds> 
    void _init_strides(size_t D, Stride1D &&s1, Stride1Ds &&...s);

    template<typename T>
    static stride_1d_t _select_to_stride_1d(size_t D, 
        T &&x) noexcept;
};

_HIPP_TEMPNORET
SStrideFilter() noexcept {
    auto &s = _strides.raw();
    for(size_t i=0; i<RANK; ++i){
        s[i][0] = 0;
        s[i][1] = _raw_extents[i];
        s[i][2] = 1;
    }
}

_HIPP_TEMPHD
template<typename ...Stride1Ds>
_HIPP_TEMPCLS::SStrideFilter(Stride1Ds &&...s) noexcept 
{
    static_assert(sizeof...(Stride1Ds) == RANK, 
        "Number of 1-D strides is not equal to RANK");
    _init_strides(0, std::forward<Stride1Ds>(s)...);
}

_HIPP_TEMPNORET SStrideFilter(const strides_t &s) noexcept 
: _strides(s)
{}

_HIPP_TEMPNORET ~SStrideFilter() noexcept {}

_HIPP_TEMPHD
template<typename UnaryOp>
void _HIPP_TEMPCLS::visit(UnaryOp op) const {
    _visit_at_dim<UnaryOp, 0>(0, op);
}

_HIPP_TEMPHD
template<typename BinaryOp, typename RandomAccessIt>
void _HIPP_TEMPCLS::visit(BinaryOp op, RandomAccessIt b) const {
    auto combined_op = [&op, &b](size_t i)->void { op(i, b[i]); };
    visit(combined_op);
}

_HIPP_TEMPHD
template<typename UnaryOp, size_t D>
void _HIPP_TEMPCLS::_visit_at_dim(size_t cum_id, UnaryOp &op) const {
    const auto [b, e, step] = _strides.raw()[D];
    if constexpr( D == RANK-1 ) {
        for(size_t i=b; i<e; i+=step) op(cum_id+i);
    }else{
        constexpr size_t raw_s = _raw_strides[D];
        for(size_t i=b; i<e; i+=step)
            _visit_at_dim<UnaryOp, D+1>(cum_id+i*raw_s, op);
    }
}

_HIPP_TEMPHD
template<typename Stride1D, typename ...Stride1Ds> 
void _HIPP_TEMPCLS::_init_strides(size_t D, Stride1D &&s1, Stride1Ds &&...s) {
    auto &s_dest = _strides.raw()[D];
    auto s_src = _select_to_stride_1d(D, std::forward<Stride1D>(s1));
    std::copy(s_src.begin(), s_src.end(), s_dest);
    if constexpr( sizeof...(Stride1Ds) > 0 ) {
        _init_strides(D+1, std::forward<Stride1Ds>(s)...);
    }
}

_HIPP_TEMPHD
template<typename T>
auto _HIPP_TEMPCLS::_select_to_stride_1d(size_t D, T &&x) noexcept 
-> stride_1d_t 
{
    typedef std::remove_cv_t<std::remove_reference_t<T>> _T;
    typedef SArrayBase base_t;
    constexpr size_t one_v = 1, zero_v = 0;
    size_t EXT = _raw_extents[D];
    if constexpr ( std::is_same_v<_T, base_t::s_all_t> ) {
        return stride_1d_t{zero_v, EXT, one_v};
    }else if constexpr( std::is_same_v<_T, base_t::s_none_t> ) {
        return stride_1d_t{zero_v, zero_v, one_v};
    }else if constexpr( std::is_same_v<_T, base_t::s_one_t> ) {
        size_t id = std::forward<T>(x).id;
        return stride_1d_t{id, id+one_v, one_v} ;
    }else if constexpr( std::is_same_v<_T, base_t::s_range_t> ) {
        size_t b = std::forward<T>(x).b, 
            e = std::forward<T>(x).e,
            step = std::forward<T>(x).step;
        return stride_1d_t{b, e, step};
    }else if constexpr ( std::is_same_v<_T, base_t::s_head_t> ) {
        size_t e = std::forward<T>(x).n;
        return stride_1d_t{zero_v, e, one_v};
    }else if constexpr ( std::is_same_v<_T, base_t::s_tail_t> ) {
        size_t b = EXT - std::forward<T>(x).n;
        return stride_1d_t{b, EXT, one_v};
    }else {
        return stride_1d_t(std::forward<T>(x));
    }
}

} // namespace HIPP::NUMERICAL

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#endif	//_HIPPNUMERICAL_LINALG_SFILTER_STRIDE_H_