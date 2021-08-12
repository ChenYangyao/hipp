#ifndef _HIPPNUMERICAL_LINALG_SARRAYND_H_
#define _HIPPNUMERICAL_LINALG_SARRAYND_H_

#include "linalg_base.h"

#define _HIPP_TEMPHD template<typename ValueT, size_t ...Ds>
#define _HIPP_TEMPARG <ValueT, Ds...>
#define _HIPP_TEMPCLS \
    SArray _HIPP_TEMPARG
#define _HIPP_TEMPCLS_B \
    SArray <bool, Ds...>
#define _HIPP_TEMPRET \
    _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET \
    _HIPP_TEMPHD inline _HIPP_TEMPCLS::

namespace HIPP {
namespace NUMERICAL {

/** Forward declarations. */
_HIPP_TEMPHD class SArray;

template<size_t ...Ds> class SBoolFilter;
template<size_t ...Ds> class SStrideFilter;
template<typename FilterT, typename ValueT, size_t ...Ds> class SArrayView;
template<typename FilterT, typename ValueT, size_t ...Ds> class SArrayConstView;

_HIPP_TEMPHD ostream & operator<<(ostream &os, const _HIPP_TEMPCLS &);
_HIPP_TEMPHD void swap(_HIPP_TEMPCLS &lhs, _HIPP_TEMPCLS &rhs) noexcept;

/**
Aliases for convenience. 
Format: SVecPq, where P is in {X, 1, 2, 3, 4}, and q is in {d, f, i, b}.
@P: dimension of the vector.
@q: type of its element. d, f, i, and b are for double, float, integer, 
    and bool, respectively.
*/
template<typename ValueT, size_t N>
using SVec = SArray<ValueT, N>;

template<size_t N>
using SVecXd = SArray<double, N>;
using SVec1d = SVecXd<1>;
using SVec2d = SVecXd<2>;
using SVec3d = SVecXd<3>;
using SVec4d = SVecXd<4>;

template<size_t N>
using SVecXf = SArray<float, N>;
using SVec1f = SVecXf<1>;
using SVec2f = SVecXf<2>;
using SVec3f = SVecXf<3>;
using SVec4f = SVecXf<4>;

template<size_t N>
using SVecXi = SArray<int, N>;
using SVec1i = SVecXi<1>;
using SVec2i = SVecXi<2>;
using SVec3i = SVecXi<3>;
using SVec4i = SVecXi<4>;

template<size_t N>
using SVecXb = SArray<bool, N>;
using SVec1b = SVecXb<1>;
using SVec2b = SVecXb<2>;
using SVec3b = SVecXb<3>;
using SVec4b = SVecXb<4>;

} // namespace NUMERICAL

/** SArray follows the raw-array concept. */
template<typename ValueT, size_t ...Ds> 
class RawArrayTraits< HIPP::NUMERICAL::SArray<ValueT, Ds...> > 
: public RawArrayTraits< RawArrayHelper::extents_to_array_t<ValueT, Ds...> > {};

} // namespace HIPP


namespace std {

/** The tuple-like API. */
template<typename ValueT, size_t ...Ds>
struct tuple_size<HIPP::NUMERICAL::_HIPP_TEMPCLS > {

    static_assert(sizeof...(Ds) >= 2, 
        "Tuple size of SArray: must have dimensions >= 2");
    
    template<size_t D1, size_t ...Dother>
    struct _dim_decompose {
        inline static constexpr size_t d1 = D1;
        typedef HIPP::NUMERICAL::SArray<ValueT, Dother...> sub_array_t;
    };
    typedef _dim_decompose<Ds...> _dim_decompose_t;

    typedef typename _dim_decompose_t::sub_array_t _sub_array_t;
    inline static constexpr size_t 
        _d1_v = _dim_decompose_t::d1,     // no. of element in the first dim
        _stride_v = sizeof(_sub_array_t); // stride in byte

    /* The number of tuple elements, namely the size of the first dimension. */
    inline static constexpr size_t value = _d1_v;
};

template<size_t I, typename ValueT, size_t ...Ds>
struct tuple_element<I, HIPP::NUMERICAL::_HIPP_TEMPCLS > {
    typedef tuple_size< HIPP::NUMERICAL::_HIPP_TEMPCLS > _tpl_size_t;

    /* The type of each tuple element, namely a sub-array. */
    using type = typename _tpl_size_t::_sub_array_t;
};

} // namespace std

namespace HIPP::NUMERICAL {

template<size_t I, typename ValueT, size_t ...Ds>
decltype(auto) get(const _HIPP_TEMPCLS &v) noexcept 
{ 
    typedef _HIPP_TEMPCLS arr_t;
    typedef typename std::tuple_element<I, arr_t>::type sub_arr_t;
    constexpr size_t stride_v = std::tuple_size<arr_t>::_stride_v,
        offset = stride_v * I;
    auto *p = reinterpret_cast<const char *>(v.data()) + offset;
    return *reinterpret_cast<const sub_arr_t *>(p);
}

template<size_t I, typename ValueT, size_t ...Ds>
decltype(auto) get(_HIPP_TEMPCLS &v) noexcept {
    typedef _HIPP_TEMPCLS arr_t;
    typedef typename std::tuple_element<I, arr_t>::type sub_arr_t;
    constexpr size_t stride_v = std::tuple_size<arr_t>::_stride_v,
        offset = stride_v * I;
    auto *p = reinterpret_cast<char *>(v.data()) + offset;
    return *reinterpret_cast<sub_arr_t *>(p);
}

template<size_t I, typename ValueT, size_t ...Ds>
decltype(auto) get(_HIPP_TEMPCLS &&v) noexcept {
    typedef _HIPP_TEMPCLS arr_t;
    typedef typename std::tuple_element<I, arr_t>::type sub_arr_t;
    constexpr size_t stride_v = std::tuple_size<arr_t>::_stride_v,
        offset = stride_v * I;
    auto *p = reinterpret_cast<char *>(v.data()) + offset;
    return std::move(*reinterpret_cast<sub_arr_t *>(p));
}

/**
The base class for all SArray.
*/
struct SArrayBase {
    /** Slice/Stride descriptor types and generators. */
    struct s_all_t {};
    struct s_none_t {};
    struct s_one_t { 
        size_t id;  
        explicit constexpr s_one_t(size_t _id): id(_id) {}
    };
    struct s_range_t {
        size_t b, e, step;
        explicit constexpr s_range_t(size_t _b, size_t _e, size_t _step=1)
        : b(_b), e(_e), step(_step) {}
    };
    struct s_head_t {
        size_t n;
        explicit constexpr s_head_t(size_t _n) : n(_n) {}
    };
    struct s_tail_t {
        size_t n;
        explicit constexpr s_tail_t(size_t _n) : n(_n) {}
    };

    struct s_stride_t {};

    /**
    Selectors - select a slice along one dimension. The generator is obtained
    by the methods or directly from the attributes.
    All these selectors can be converted to a slice form, b, e, step, as 
    generated by s_range().

    s_all：     select all element.
    s_none：    select no element.
    s_one()：   select a single element indexed `id`.
    s_range()： select all elements in the index range [b, e), with a 
        step size `step`.
    s_head()：  first ``n`` elements.
    s_tail()：  last ``n`` elements.
    */
    inline static constexpr s_all_t s_all {};
    inline static constexpr s_none_t s_none {};
    static constexpr s_one_t s_one(size_t id) { return s_one_t(id); }
    static constexpr s_range_t s_range(size_t b, size_t e, size_t step=1) 
        { return s_range_t(b, e, step); }
    static constexpr s_head_t s_head(size_t n) { return s_head_t(n); }
    static constexpr s_tail_t s_tail(size_t n) { return s_tail_t(n); }

    /**
    A positional argument for template matching.
    */
    inline static constexpr s_stride_t s_stride {};
};


/**
SArray - static/small N-D array.
For the 1-D case, see the specialization SArray<ValueT, N>.

SArray has only continuous and row-major layout data members typed ``ValueT``
with dimensions specified by ``Ds...``.

SArray is movable and copyable and these operations produce deep copy. 
The copy, move and destructor are all noexcept.

Template arguments
-------------------
@ValueT: element type, usually a numerical scalar (integer or floating-point 
         number).
@Ds: length of each dimensions.
*/
template<typename ValueT, size_t ...Ds>
class SArray : public SArrayBase {
public:
    static_assert(sizeof...(Ds) >= 2, "SArray: must have dimensions >= 2");

    /**
    Basic aliases and properties.
    value_t: type of the array element.
    raw_array_t: type of the internal storage of the array, i.e., a raw array.
    traits_t: type traits for SArray.
    SIZE: total number of elements.
    */
    typedef ValueT value_t;
    typedef RawArrayHelper::extents_to_array_t<value_t, Ds...> raw_array_t;
    typedef RawArrayTraits<SArray> traits_t;
    inline static constexpr size_t SIZE = traits_t::size;

    /** 
    Aliases for member access.
    ret_t and cref_t: reference type to the vector element, and its const 
        counterpart.
    iter_t and citer_t: iterator type to the vector element, and its const
        counterpart.
    */
    typedef value_t & ref_t;
    typedef const value_t & cref_t;
    typedef value_t *iter_t;
    typedef const value_t *citer_t;

    /** Aliases for Boolean filter. */
    typedef SArray<bool, Ds...> bool_mask_t;
    typedef SBoolFilter<Ds...> bool_filter_t;
    typedef SArrayView<bool_filter_t, ValueT, Ds...> bool_view_t;
    typedef SArrayConstView<bool_filter_t, ValueT, Ds...> cbool_view_t; 

    /** Aliases for stride filter. */
    typedef SStrideFilter<Ds...> stride_filter_t;
    typedef SArrayView<stride_filter_t, ValueT, Ds...> stride_view_t;
    typedef SArrayConstView<stride_filter_t, ValueT, Ds...> cstride_view_t; 

    /**
    IS_INT: tells whether the value type is integer-like (i.e., integer or 
        pointer). 
    int_value_t: defined as ``int`` for non-integer-like, and defined as 
        itself for integer-like.
    */
    inline static constexpr bool IS_INT = 
        std::is_integral_v<ValueT> || std::is_pointer_v<ValueT>;
    typedef std::conditional_t<IS_INT, ValueT, int> int_value_t;

    /**
    (1) Default initialization of all elements. Caution for numeric types.
    (2) All members are initialized with ``value``.
    (3) Row-major order copying from a range starting from ``b``, with ``n`` 
        elements.
    (4) Row-major order copying From an initializer list.
    (5) Cast from another SArray.
    In (3) and (4), ``n`` and ``il.size()`` may be less than ``SIZE``, leaving 
    the tail un-initialized.
    */
    SArray() noexcept {}

    explicit SArray(const value_t &value) noexcept;

    template<typename InputValue>
    explicit SArray(const InputValue *b, size_t n = SIZE) noexcept;

    template<typename InputValue>
    SArray(std::initializer_list<InputValue> il) noexcept;

    template<typename InputValue>
    explicit SArray(const SArray<InputValue, Ds...> &a) noexcept;

    SArray(const SArray &) noexcept = default;
    SArray(SArray &&)  noexcept = default;
    SArray & operator=(const SArray &) noexcept = default;
    SArray & operator=(SArray &&)  noexcept = default;
    ~SArray() noexcept {}
    
    /* Set all elements to a single value. */
    SArray & operator=(const value_t &value) noexcept;
    
    /**
    Deep, all elements are swapped.
    */
    friend void swap <ValueT, Ds...> (SArray &lhs, SArray &rhs) noexcept;

    /** 
    operator<<() prints inline information of the instance.
    info() prints the instance with more controls.
    @fmt_cntl: 0 for an inline short message. 1 for a long block message.
    */
    friend ostream & operator<< <ValueT, Ds...> (ostream &os, const SArray &);
    ostream & info(ostream &os=cout, int fmt_cntl=1) const;

    /**
    STL-conforming definitions - treating the array as a row-major linear array.
    data() - return a pointer to the internal storage.
    size() - always returns ``SIZE``.
    empty() - returns true only if ``SIZE == 0``.

    operator[] and at() - element access. 
        - at() throws on the out-of-range.
        - operator [] can accept a Boolean mask, return a view.
        - operator [] can accept a stride filter, return a view.
    begin(), end() and their const counterparts - iterators.
    */
    value_t * data() noexcept;
    const value_t * data() const noexcept;
    raw_array_t & raw() noexcept;
    const raw_array_t & raw() const noexcept;
    static constexpr size_t size() noexcept;
    static constexpr bool empty() noexcept;

    ref_t operator[](size_t pos) noexcept;
    cref_t operator[](size_t pos) const noexcept;

    bool_view_t operator[](const bool_mask_t &mask) noexcept;
    cbool_view_t operator[](const bool_mask_t &mask) const noexcept;
    
    stride_view_t operator[](const stride_filter_t &s) noexcept;
    cstride_view_t operator[](const stride_filter_t &s) const noexcept;

    ref_t at(size_t pos);
    cref_t at(size_t pos) const;

    iter_t begin() noexcept;
    citer_t begin() const noexcept;
    citer_t cbegin() const noexcept;
    iter_t end() noexcept;
    citer_t end() const noexcept;
    citer_t cend() const noexcept;

    /** Visit an element with indices `ids` at each dimension. */
    template<typename ...SizeTs>
    ref_t operator()(SizeTs &&...ids) noexcept;
    template<typename ...SizeTs>
    cref_t operator()(SizeTs &&...ids) const noexcept;

    /**
    Linear algebra operations. All are element-wise operations.
    1. Unary RMW operations:
        (1) arr op= scalar;
        (2) arr op= arr;
    Where op is 
        +, -, *, /, %           - arithmetic -> SArray &
        &, |, ^                 - bit-wise for each element -> SArray &
    2. Binary operations:
        (1) arr op scalar;
        (2) scalar op arr;
        (3) arr op arr;
    Where op is 
        +, -, *, /, %           - arithmetic -> SArray
        &, |, ^                 - bit-wise for each element -> SArray
        <, <=, >, >=, ==, !=    - comparison/logic -> SArray<bool, Ds...>
    3. Unary operations: 
        op arr;
    Where op is
        ~, +, -                 - bit-wise NOT, arithmetic positate, negate. 
    Caution: interger-lift is used intermediately for small integers, like 
    `bool`, `char`, etc., so that ~true != false. But &, |, ^ work just as
    expected.
    */
    SArray & operator+=(const value_t &rhs) noexcept;
    SArray & operator-=(const value_t &rhs) noexcept;
    SArray & operator*=(const value_t &rhs) noexcept;
    SArray & operator/=(const value_t &rhs) noexcept;
    SArray & operator%=(const value_t &rhs) noexcept;
    SArray & operator&=(const value_t &rhs) noexcept;
    SArray & operator|=(const value_t &rhs) noexcept;
    SArray & operator^=(const value_t &rhs) noexcept;

    SArray & operator+=(const SArray &rhs) noexcept;
    SArray & operator-=(const SArray &rhs) noexcept;
    SArray & operator*=(const SArray &rhs) noexcept;
    SArray & operator/=(const SArray &rhs) noexcept;
    SArray & operator%=(const SArray &rhs) noexcept;
    SArray & operator&=(const SArray &rhs) noexcept;
    SArray & operator|=(const SArray &rhs) noexcept;
    SArray & operator^=(const SArray &rhs) noexcept;

    SArray operator+() const noexcept;
    SArray operator-() const noexcept;
    SArray operator~() const noexcept;

    /**
    norm() - 2-norm (i.e., Frobenius norm).
    norm(p) - p-norm (treating array as a vector).
    squared_norm() - square of 2-norm.
    normalize() - normalize itself, according to 2-norm.
    normalize(int p) - normalize itself, according to p-norm.
    normalized() - returns a normalized (according to 2-norm) copy.
    normalized(int p) - returns a normalized (according to p-norm) copy.

    ``normalize()`` or ``normalized()`` for an integer vector are ill-defined. 
    ``norm()`` with ``ResT`` != floting-point type may have truncation.
    With caution to use.
    */
    template<typename ResT = double> 
    ResT norm() const noexcept;
    template<typename ResT = double> 
    ResT norm(int p) const noexcept;
    template<typename ResT = double> 
    ResT squared_norm() const noexcept;
    
    SArray & normalize() noexcept;
    SArray & normalize(int p) noexcept;
    SArray normalized() const noexcept;
    SArray normalized(int p) const noexcept;

    /**
    Reduction operations.
    sum(), prod(), mean() - the summation, product, and mean of all elements.
    
    min(), max(), minmax() - as you expect. 
    The indexed-version returns the element index if the corresponding result.

    all(), any() - all true or any true.
    */
    template<typename ResT = value_t>
    ResT sum() const noexcept;
    template<typename ResT = value_t>
    ResT prod() const noexcept; 
    template<typename ResT = value_t>
    ResT mean() const noexcept;

    value_t min() const noexcept;
    value_t max() const noexcept;
    std::pair<value_t, value_t> minmax() const noexcept;
    
    size_t min_index() const noexcept;
    size_t max_index() const noexcept;
    std::pair<size_t, size_t> minmax_index() const noexcept;

    bool all() const noexcept;
    bool any() const noexcept;


    /**
    Map and visit operations.
    map() - for each i, self[i] = op(self[i]).
    mapped() - returns a mapped copy.
    visit() - for each size_t(i), call op(i, self[i]).
    */
    template<typename UnaryOp>
    SArray & map(UnaryOp op);

    template<typename UnaryOp, 
        typename ResT = std::invoke_result_t<UnaryOp, value_t> >
    SArray<ResT, Ds...> mapped(UnaryOp op) const;

    template<typename BinaryOp>
    void visit(BinaryOp op) const;
    
    template<typename BinaryOp>
    void visit(BinaryOp op);

    /** 
    Round to floor, ceil, trunc toward zero, and absolute value.
    ``ResT`` can be floating-point or integral type.

    By default, if ``value_t`` is integer or pointer, ``ResT`` is ``value_t`` 
    itself, no conversion bappens. Otherwise ``ResT`` is ``int``, and the 
    conversion is made by std::floor, ceil, trunc and then cast.
    */
    template<typename ResT = int_value_t>
    SArray<ResT, Ds...> floor() const noexcept;

    template<typename ResT = int_value_t>
    SArray<ResT, Ds...> ceil() const noexcept;

    template<typename ResT = int_value_t>
    SArray<ResT, Ds...> trunc() const noexcept;

    SArray abs() const noexcept;

    /**
    Views - get a "view" object of the instance.
    The view object holds a reference to the SArray instance that generates it.
    Any modifications to the view is reflected to the SArray.
    A constant view cannot be used to modify the SArray.

    view() - get a view object. 
    cview() - get constant view object.
    
    If the `s_stride_t` function is matched, `args` are forwarded to construct a 
    stride filter and then it is used for the view.
    
    @mask: generate a boolean view according to the mask for each element. E.g.,
        if mask[i, ...] is true, then the view contains (*this)[i, ...].
    @stride_filter: generate a stride view according to the stride filter.
    */
    bool_view_t view(const bool_mask_t &mask) noexcept;
    cbool_view_t view(const bool_mask_t &mask) const noexcept;
    cbool_view_t cview(const bool_mask_t &mask) const noexcept;

    stride_view_t view(const stride_filter_t &s) noexcept;
    cstride_view_t view(const stride_filter_t &s) const noexcept;
    cstride_view_t cview(const stride_filter_t &s) const noexcept;
    template<typename ...Args>
    stride_view_t view(s_stride_t, Args &&...args) noexcept;
    template<typename ...Args>
    cstride_view_t view(s_stride_t, Args &&...args) const noexcept;
    template<typename ...Args>
    cstride_view_t cview(s_stride_t, Args &&...args) const noexcept;
protected:
    raw_array_t _data;
};

_HIPP_TEMPNORET
SArray(const value_t &value) noexcept {
    std::fill_n(data(), SIZE, value);
}

_HIPP_TEMPHD
template<typename InputValue>
_HIPP_TEMPCLS::SArray(const InputValue *b, size_t n) noexcept {
    std::copy_n(b, n, data());
}

_HIPP_TEMPHD
template<typename InputValue>
_HIPP_TEMPCLS::SArray(std::initializer_list<InputValue> il) noexcept {
    std::copy(il.begin(), il.end(), data());
}

_HIPP_TEMPHD
template<typename InputValue>
_HIPP_TEMPCLS::SArray(const SArray<InputValue, Ds...> &a) noexcept {
    std::copy_n(a.data(), SIZE, data());
}

_HIPP_TEMPRET
operator=(const value_t &value) noexcept -> SArray & {
    std::fill_n(data(), SIZE, value);
    return *this;
}

_HIPP_TEMPHD
void swap(_HIPP_TEMPCLS &lhs, _HIPP_TEMPCLS &rhs) noexcept {
    std::swap_ranges(lhs.begin(), lhs.end(), rhs.begin());
}

_HIPP_TEMPHD
ostream & operator<<(ostream &os, const _HIPP_TEMPCLS &a) {
    PStream ps(os);
    ps << "SArray{", ps(a.begin(), a.end()), "}";
    return os;
}

_HIPP_TEMPRET info(ostream &os, int fmt_cntl) const -> ostream & {
    PStream ps(os);
    if(fmt_cntl == 0) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(SArray), 
            "{", ps(begin(), end()), "}";
    }else{
        ps << HIPPCNTL_CLASS_INFO(SArray), 
            "  |- size = ", SIZE, ", extents = {", traits_t::extents, "}\n",
            "  |- values = {", ps(begin(), end()), "}\n"; 
    }
    return os;
}

_HIPP_TEMPRET data() noexcept -> value_t * {
    return reinterpret_cast<value_t *>(_data);
}

_HIPP_TEMPRET data() const noexcept -> const value_t * {
    return reinterpret_cast<const value_t *>(_data);
}

_HIPP_TEMPRET raw() noexcept -> raw_array_t & {
    return _data;
}

_HIPP_TEMPRET raw() const noexcept -> const raw_array_t & {
    return _data;
}

_HIPP_TEMPHD
constexpr size_t _HIPP_TEMPCLS::size() noexcept { return SIZE; }

_HIPP_TEMPHD
constexpr bool _HIPP_TEMPCLS::empty() noexcept { return SIZE == 0; }

_HIPP_TEMPRET operator[](size_t pos) noexcept -> ref_t {
    return data()[pos];
}

_HIPP_TEMPRET operator[](size_t pos) const noexcept -> cref_t {
    return data()[pos];
}

_HIPP_TEMPRET operator[](const bool_mask_t &mask) noexcept 
-> bool_view_t 
{
    return view(mask);
}

_HIPP_TEMPRET operator[](const bool_mask_t &mask) const noexcept 
-> cbool_view_t 
{
    return view(mask);
}

_HIPP_TEMPRET operator[](const stride_filter_t &s) noexcept
-> stride_view_t {
    return view(s);
}

_HIPP_TEMPRET operator[](const stride_filter_t &s) const noexcept
-> cstride_view_t {
    return view(s);
}

_HIPP_TEMPRET at(size_t pos) -> ref_t {
    if( pos >= SIZE )
        ErrLogic::throw_(ErrLogic::eOUTOFRANGE, emFLPFB, "   ... pos ", 
        pos, " >= size ", SIZE, '\n');    
    return (*this)[pos];
}

_HIPP_TEMPRET at(size_t pos) const -> cref_t {
    if( pos >= SIZE )
        ErrLogic::throw_(ErrLogic::eOUTOFRANGE, emFLPFB, "   ... pos ", 
        pos, " >= size ", SIZE, '\n');    
    return (*this)[pos];
}

_HIPP_TEMPRET begin() noexcept -> iter_t {
    return data();
}

_HIPP_TEMPRET begin() const noexcept -> citer_t {
    return data();
}

_HIPP_TEMPRET cbegin() const noexcept -> citer_t {
    return data();
}

_HIPP_TEMPRET end() noexcept -> iter_t {
    return data() + SIZE;
}

_HIPP_TEMPRET end() const noexcept -> citer_t {
    return data() + SIZE;
}

_HIPP_TEMPRET cend() const noexcept -> citer_t {
    return data() + SIZE;
}

_HIPP_TEMPHD
template<typename ...SizeTs>
auto _HIPP_TEMPCLS::operator()(SizeTs &&...ids) noexcept -> ref_t {
    static_assert(sizeof...(SizeTs) == traits_t::rank, 
        "SArray::operator(): number of indices must be equal to the rank");
    return RawArrayHelper::get_elem(_data, std::forward<SizeTs>(ids)...);
}

_HIPP_TEMPHD
template<typename ...SizeTs>
auto _HIPP_TEMPCLS::operator()(SizeTs &&...ids) const noexcept -> cref_t {
    static_assert(sizeof...(SizeTs) == traits_t::rank,
        "SArray::operator(): number of indices must be equal to the rank");
    return RawArrayHelper::get_elem(_data, std::forward<SizeTs>(ids)...);
}


#define _HIPP_UNARY_OP_DEF(op) \
    _HIPP_TEMPRET \
    operator op(const value_t &rhs) noexcept -> SArray & { \
        value_t *a = data(); \
        for(size_t i=0; i<SIZE; ++i) a[i] op rhs; \
        return *this; \
    } \
    _HIPP_TEMPRET \
    operator op(const SArray &rhs) noexcept -> SArray & { \
        value_t *a = data();  \
        const value_t *b = rhs.data(); \
        for(size_t i=0; i<SIZE; ++i) a[i] op b[i]; \
        return *this; \
    }

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
    _HIPP_TEMPRET \
    operator op() const noexcept -> SArray { \
        SArray ret; \
        const value_t *a = data(); \
        for(size_t i=0; i<SIZE; ++i) ret[i] = op a[i]; \
        return ret; \
    }

_HIPP_UNARY_OP_DEF(+)
_HIPP_UNARY_OP_DEF(-)
_HIPP_UNARY_OP_DEF(~)

#undef _HIPP_UNARY_OP_DEF

#define _HIPP_BIN_OP_DEF(op) \
    _HIPP_TEMPHD \
    _HIPP_TEMPCLS operator op( \
        const typename _HIPP_TEMPCLS::value_t &lhs,  \
        const _HIPP_TEMPCLS &rhs) noexcept  \
    { \
        _HIPP_TEMPCLS ret; \
        for(size_t i=0; i<_HIPP_TEMPCLS::SIZE; ++i) ret[i] = lhs op rhs[i]; \
        return ret; \
    } \
    _HIPP_TEMPHD \
    _HIPP_TEMPCLS operator op( \
        const _HIPP_TEMPCLS &lhs, \
        const typename _HIPP_TEMPCLS::value_t &rhs) noexcept  \
    { \
        _HIPP_TEMPCLS ret; \
        for(size_t i=0; i<_HIPP_TEMPCLS::SIZE; ++i) ret[i] = lhs[i] op rhs; \
        return ret; \
    } \
    _HIPP_TEMPHD \
    _HIPP_TEMPCLS operator op( \
        const _HIPP_TEMPCLS &lhs, const _HIPP_TEMPCLS &rhs) noexcept  \
    { \
        _HIPP_TEMPCLS ret; \
        for(size_t i=0; i<_HIPP_TEMPCLS::SIZE; ++i) ret[i] = lhs[i] op rhs[i]; \
        return ret; \
    }

_HIPP_BIN_OP_DEF(+)
_HIPP_BIN_OP_DEF(-)
_HIPP_BIN_OP_DEF(*)
_HIPP_BIN_OP_DEF(/)
_HIPP_BIN_OP_DEF(%)
_HIPP_BIN_OP_DEF(&)
_HIPP_BIN_OP_DEF(|)
_HIPP_BIN_OP_DEF(^)

#undef _HIPP_BIN_OP_DEF

#define _HIPP_BIN_OP_DEF(op) \
    _HIPP_TEMPHD \
    _HIPP_TEMPCLS_B operator op( \
        const typename _HIPP_TEMPCLS::value_t &lhs,  \
        const _HIPP_TEMPCLS &rhs) noexcept  \
    { \
        _HIPP_TEMPCLS_B ret; \
        for(size_t i=0; i<_HIPP_TEMPCLS::SIZE; ++i) ret[i] = lhs op rhs[i]; \
        return ret; \
    } \
    _HIPP_TEMPHD \
    _HIPP_TEMPCLS_B operator op( \
        const _HIPP_TEMPCLS &lhs, \
        const typename _HIPP_TEMPCLS::value_t &rhs) noexcept  \
    { \
        _HIPP_TEMPCLS_B ret; \
        for(size_t i=0; i<_HIPP_TEMPCLS::SIZE; ++i) ret[i] = lhs[i] op rhs; \
        return ret; \
    } \
    _HIPP_TEMPHD \
    _HIPP_TEMPCLS_B operator op( \
        const _HIPP_TEMPCLS &lhs, const _HIPP_TEMPCLS &rhs) noexcept  \
    { \
        _HIPP_TEMPCLS_B ret; \
        for(size_t i=0; i<_HIPP_TEMPCLS::SIZE; ++i) ret[i] = lhs[i] op rhs[i]; \
        return ret; \
    }

_HIPP_BIN_OP_DEF(<)
_HIPP_BIN_OP_DEF(<=)
_HIPP_BIN_OP_DEF(>)
_HIPP_BIN_OP_DEF(>=)
_HIPP_BIN_OP_DEF(==)
_HIPP_BIN_OP_DEF(!=)

#undef _HIPP_BIN_OP_DEF

_HIPP_TEMPHD
template<typename ResT> 
ResT _HIPP_TEMPCLS::norm() const noexcept {
    return static_cast<ResT>(std::sqrt( squared_norm<ResT>() ));
}

_HIPP_TEMPHD
template<typename ResT> 
ResT _HIPP_TEMPCLS::norm(int p) const noexcept {
    ResT ret {0};
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i){
        ret += std::pow(a[i], p);
    }
    return std::pow(ret, 1.0/static_cast<double>(p));
}

_HIPP_TEMPHD
template<typename ResT> 
ResT _HIPP_TEMPCLS::squared_norm() const noexcept {
    ResT ret {0};
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i){
        auto x = static_cast<ResT>(a[i]);
        ret += x*x;
    }
    return ret;
}

_HIPP_TEMPRET normalize() noexcept -> SArray & {
    auto scale = 1.0 / norm();
    return this->operator*=( static_cast<value_t>(scale) );
}

_HIPP_TEMPRET normalize(int p) noexcept -> SArray & {
    auto scale = 1.0 / norm(p);
    return this->operator*=( static_cast<value_t>(scale) );
}

_HIPP_TEMPRET normalized() const noexcept -> SArray {
    auto scale = 1.0 / norm();
    return (*this) * static_cast<value_t>(scale);
}

_HIPP_TEMPRET normalized(int p) const noexcept -> SArray {
    auto scale = 1.0 / norm(p);
    return (*this) * static_cast<value_t>(scale);
}

_HIPP_TEMPHD
template<typename ResT>
ResT _HIPP_TEMPCLS::sum() const noexcept {
    ResT ret {0};
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i) ret += a[i];
    return ret;
}

_HIPP_TEMPHD
template<typename ResT>
ResT _HIPP_TEMPCLS::prod() const noexcept {
    ResT ret {1};
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i) ret *= a[i];
    return ret;
}

_HIPP_TEMPHD
template<typename ResT>
ResT _HIPP_TEMPCLS::mean() const noexcept {
    static_assert(SIZE != 0);
    return sum<ResT>() / static_cast<ResT>(SIZE);
}

_HIPP_TEMPRET min() const noexcept -> value_t {
    static_assert(SIZE != 0);
    value_t ret { std::numeric_limits<value_t>::max() };
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i){
        if( a[i] < ret ) ret = a[i];
    }
    return ret;
}

_HIPP_TEMPRET max() const noexcept -> value_t {
    static_assert(SIZE != 0);
    value_t ret { std::numeric_limits<value_t>::lowest() };
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i){
        if( a[i] > ret ) ret = a[i];
    }
    return ret;
}

_HIPP_TEMPRET minmax() const noexcept -> std::pair<value_t, value_t> {
    static_assert(SIZE != 0);
    value_t ret_min = { std::numeric_limits<value_t>::max() },
        ret_max = { std::numeric_limits<value_t>::lowest() };
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i){
        value_t e = a[i];
        if( e < ret_min ) ret_min = e;
        if( e > ret_max ) ret_max = e;
    }
    return {ret_min, ret_max};
}

_HIPP_TEMPRET min_index() const noexcept -> size_t {
    static_assert(SIZE != 0);
    const value_t & a = data();
    auto it = std::min_element(a, a+SIZE);
    return static_cast<size_t>(it - a);
}

_HIPP_TEMPRET max_index() const noexcept -> size_t {
    static_assert(SIZE != 0);
    const value_t & a = data();
    auto it = std::max_element(a, a+SIZE);
    return static_cast<size_t>(it - a);
}

_HIPP_TEMPRET minmax_index() const noexcept -> std::pair<size_t, size_t> {
    static_assert(SIZE != 0);
    const value_t & a = data();
    auto [it_min, it_max] = std::minmax_element(a, a+SIZE);
    return { static_cast<size_t>(it_min-a), 
        static_cast<size_t>(it_max-a) };
}

_HIPP_TEMPRET all() const noexcept -> bool {
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i){
        if( !a[i] ) return false;
    }
    return true;
}

_HIPP_TEMPRET any() const noexcept -> bool {
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i){
        if( a[i] ) return true;
    }
    return false;
}

_HIPP_TEMPHD
template<typename UnaryOp>
auto _HIPP_TEMPCLS::map(UnaryOp op) -> SArray & {
    value_t *a = data();
    for(size_t i=0; i<SIZE; ++i) a[i] = op(a[i]);
    return *this;
}

_HIPP_TEMPHD
template<typename UnaryOp, typename ResT>
auto _HIPP_TEMPCLS::mapped(UnaryOp op) const -> SArray<ResT, Ds...> {
    SArray<ResT, Ds...> ret;
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i){
        ret[i] = op(a[i]);
    }
    return ret;
}

_HIPP_TEMPHD
template<typename BinaryOp>
void _HIPP_TEMPCLS::visit(BinaryOp op) const {
    for(size_t i=0; i<SIZE; ++i) op(i, (*this)[i]);
}

_HIPP_TEMPHD
template<typename BinaryOp>
void _HIPP_TEMPCLS::visit(BinaryOp op) {
    for(size_t i=0; i<SIZE; ++i) op(i, (*this)[i]);
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::floor() const noexcept -> SArray<ResT, Ds...> {
    SArray<ResT, Ds...> res;
    const value_t *a = data();
    if constexpr(IS_INT) {
        for(size_t i=0; i<SIZE; ++i) res[i] = static_cast<ResT>(a[i]);
    }else {
        for(size_t i=0; i<SIZE; ++i) 
            res[i] = static_cast<ResT>(std::floor(a[i]));
    }
    return res;
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::ceil() const noexcept -> SArray<ResT, Ds...> {
    SArray<ResT, Ds...> res;
    const value_t *a = data();
    if constexpr(IS_INT) {
        for(size_t i=0; i<SIZE; ++i) res[i] = static_cast<ResT>(a[i]);
    }else {
        for(size_t i=0; i<SIZE; ++i) 
            res[i] = static_cast<ResT>(std::ceil(a[i]));
    }
    return res;
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::trunc() const noexcept -> SArray<ResT, Ds...> {
    SArray<ResT, Ds...> res;
    const value_t *a = data();
    if constexpr(IS_INT) {
        for(size_t i=0; i<SIZE; ++i) res[i] = static_cast<ResT>(a[i]);
    }else {
        for(size_t i=0; i<SIZE; ++i) 
            res[i] = static_cast<ResT>(std::trunc(a[i]));
    }
    return res;
}

_HIPP_TEMPRET abs() const noexcept -> SArray {
    SArray res;
    const value_t *a = data();
    for(size_t i=0; i<SIZE; ++i) res[i] = std::abs(a[i]);
    return res;
}

_HIPP_TEMPRET 
view(const bool_mask_t &mask) noexcept -> bool_view_t {
    return bool_view_t(*this, bool_filter_t(mask));
}

_HIPP_TEMPRET 
view(const bool_mask_t &mask) const noexcept -> cbool_view_t {
    return cview(mask);
}

_HIPP_TEMPRET 
cview(const bool_mask_t &mask) const noexcept -> cbool_view_t {
    return cbool_view_t(*this, bool_filter_t(mask));
}

_HIPP_TEMPRET
view(const stride_filter_t &s) noexcept -> stride_view_t {
    return stride_view_t(*this, s);
}

_HIPP_TEMPRET
view(const stride_filter_t &s) const noexcept -> cstride_view_t {
    return cview(s);
}

_HIPP_TEMPRET
cview(const stride_filter_t &s) const noexcept -> cstride_view_t {
    return cstride_view_t(*this, s);
}

_HIPP_TEMPHD
template<typename ...Args>
auto _HIPP_TEMPCLS::view(s_stride_t, Args &&...args) noexcept 
-> stride_view_t 
{
    return view( stride_filter_t(std::forward<Args>(args)...) );
}

_HIPP_TEMPHD
template<typename ...Args>
auto _HIPP_TEMPCLS::view(s_stride_t, Args &&...args) const noexcept 
-> cstride_view_t 
{
    return view( stride_filter_t(std::forward<Args>(args)...) );
}

_HIPP_TEMPHD
template<typename ...Args>
auto _HIPP_TEMPCLS::cview(s_stride_t, Args &&...args) const noexcept 
-> cstride_view_t 
{
    return cview( stride_filter_t(std::forward<Args>(args)...) );
}

} // namespace HIPP::NUMERICAL


#undef _HIPP_TEMPNORET
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPCLS_B
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPHD

#endif	//_HIPPNUMERICAL_LINALG_SARRAYND_H_