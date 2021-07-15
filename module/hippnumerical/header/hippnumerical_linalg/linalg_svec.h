/**
create: Yangyao CHEN, 2021/05/28
    [write   ] SVec - static/small vector with fixed length.
*/

#ifndef _HIPPNUMERICAL_LINALG_SVEC_H_
#define _HIPPNUMERICAL_LINALG_SVEC_H_
#include "linalg_base.h"

#define _HIPP_TEMPHD template<typename ValueT, size_t N>
#define _HIPP_TEMPARG <ValueT, N>
#define _HIPP_TEMPCLS SVec _HIPP_TEMPARG
#define _HIPP_TEMPCLS_B SVec <bool, N>
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

namespace HIPP::NUMERICAL {

/** Forward declarations. */
_HIPP_TEMPHD class SVec;
template<size_t N> class SBoolFilter;
template<typename ValueT, size_t N, typename FilterT> class SVecView;
template<typename ValueT, size_t N, typename FilterT> class SVecConstView;

_HIPP_TEMPHD ostream & operator<<(ostream &os, const _HIPP_TEMPCLS &);
_HIPP_TEMPHD void swap(_HIPP_TEMPCLS &lhs, _HIPP_TEMPCLS &rhs) noexcept;

/**
SVec - static/small vector with fixed length.

Template arguments
------------------
@ValueT: element type, usually a numerical scalar (integer or floating-point 
         number).
@N: length of the vector.

Description
------------
SVec is like std::array, but more convenient methods are provided for basic 
linear algebra, e.g., arithmetic operations, reductions (min, max, sum, prod, 
norm, ...).

Tuple-like API is defined for SVec, so that structural binding can be used.
e.g., 
SVec3d v {1.,2.,3.};
auto [x,y,z] = v; 

A particle in a N-body simulation may use SVec to find its grid index, simply 
by:
SVec3d v {-3.1, 0.1, 10.1};
double grid_sz = 0.5;
auto ids = (v / grid_sz).floor();   // => SVec<int, 3> {-7, 0, 20}.

Filter can be applied to SVec to give a "view" of it. The "view" is defined 
with a subset of SVec operations which applied to the selected elements 
of the viewed SVec.
e.g.,
SVec3i x {-3,1,2};
auto view = x[x>-6];                // => bool_view_t {true, true, true}.
x.view(x>-6);                       // the same as x[x>6].
x.cview(x>-6);                      // => the const version of the view
                                    // which cannot used to change x.

x[x>-6].sum();                      // => int {0}.
x[x>0].sum();                       // => int {3}.

x[x>0] + 1;                         // => a new SVec3i {-3, 2, 3}.
x[x>0] += 1;                        // x itself is changed to {-3, 2, 3}.

Note that a view cannot be further viewed. Its filter object and bool array mask 
can be obtained by, e.g.,
auto view = x[x>0];
view.vec();                         // => a ref to x.
view.filter();                      // => a ref to bool_filter_t(x>0).
view.filter().mask();               // => a ref to bool SVec x>0.

SVec has a RawArrayTraits support, which enables compile-time feature 
detection.
*/
template<typename ValueT, size_t N>
class SVec {
public:
    /**
    Basic aliases - The type and number of elements.
    */
    typedef ValueT value_t;
    inline static constexpr size_t SIZE = N;

    /** 
    Aliases for member access.
    ret_t and cref_t - reference type to the vector element, and its const 
        counterpart.
    iter_t and citer_t - iterator type to the vector element, and its const
        counterpart.
    */
    typedef value_t & ref_t;
    typedef const value_t & cref_t;
    typedef value_t *iter_t;
    typedef const value_t *citer_t;

    /** Aliases for Boolean filter. */
    typedef SVec<bool, SIZE> bool_mask_t;
    typedef SBoolFilter<SIZE> bool_filter_t;
    typedef SVecView<value_t, SIZE, bool_filter_t> bool_view_t;
    typedef SVecConstView<value_t, SIZE, bool_filter_t> cbool_view_t; 

    inline static constexpr bool IS_INT = 
        std::is_integral_v<value_t> || std::is_pointer_v<value_t>;
    typedef std::conditional_t<IS_INT, value_t, int> int_value_t;

    /**
    (1) Default initialization of all elements. Caution for numeric types.
    (2) All members are initialized with `value`.
    (3) Copy from a range starting from `b`, with `n` elements.
    (4) From an initializer list.
    (5) Cast from another svec.
    In (3) and (4), `n` and `il.size()` may be less than `SIZE`, leaving 
    the tail un-initialized.
    */
    SVec() noexcept {}

    explicit SVec(const value_t &value) noexcept;
    
    template<typename InputValue>
    explicit SVec(const InputValue *b, size_t n = SIZE) noexcept;
    
    template<typename InputValue>
    SVec(std::initializer_list<InputValue> il) noexcept;

    template<typename InputValue>
    explicit SVec(const SVec<InputValue, SIZE> &v) noexcept;

    SVec(const SVec &) noexcept = default;
    SVec(SVec &&)  noexcept = default;
    SVec & operator=(const SVec &) noexcept = default;
    SVec & operator=(SVec &&)  noexcept = default;
    ~SVec() noexcept {}
    /**
    Deep, all elements are swapped.
    */
    friend void swap <ValueT, N> (SVec &lhs, SVec &rhs) noexcept;

    friend ostream & operator<< <ValueT, N> (ostream &os, const SVec &);
    ostream & info(ostream &os=cout, int fmt_cntl=1) const;

    /**
    STL-conforming definitions - semantics are like std::vector.
    data() - return a pointer to the internal storage.
    size() - always returns `SIZE`.
    empty() - returns true only if `SIZE == 0`.

    operator[] and at() - element access. 
        - at() throws on the out-of-range.
        - operator [] can accept a Boolean mask, return a view.
    begin(), end and their const counterparts - iterators.
    */
    value_t * data() noexcept;
    const value_t * data() const noexcept;
    static constexpr size_t size() noexcept;
    static constexpr bool empty() noexcept;

    ref_t operator[](size_t pos) noexcept;
    cref_t operator[](size_t pos) const noexcept;
    
    ref_t at(size_t pos);
    cref_t at(size_t pos) const;

    bool_view_t operator[](const bool_mask_t &mask) noexcept;
    cbool_view_t operator[](const bool_mask_t &mask) const noexcept;

    iter_t begin() noexcept;
    citer_t begin() const noexcept;
    citer_t cbegin() const noexcept;
    iter_t end() noexcept;
    citer_t end() const noexcept;
    citer_t cend() const noexcept;

    /**
    Linear algebra operations. All are element-wise operations.
    1. Unary RMW operations:
        (1) vec op= scalar;
        (2) vec op= vec;
    Where op is 
        +, -, *, /, %           - arithmetic -> SVec &
        &, |, ^                 - bit-wise for each element -> SVec &
    2. Binary operations:
        (1) vec op scalar;
        (2) scalar op vec;
        (3) vec op vec;
    Where op is 
        +, -, *, /, %           - arithmetic -> SVec
        &, |, ^                 - bit-wise for each element -> SVec
        <, <=, >, >=, ==, !=    - comparison/logic -> SVec<bool, SIZE>
    3. Unary operations: 
        op vec;
    Where op is
        ~, +, -                 - bit-wise NOT, arithmetic positate, negate. 
    Caution: interger-lift is used intermediately for small integers, like 
    `int`, `char`, etc., so that ~true != false. But &, |, ^ work just as
    expected.
    */
    SVec & operator+=(const value_t &rhs) noexcept;
    SVec & operator-=(const value_t &rhs) noexcept;
    SVec & operator*=(const value_t &rhs) noexcept;
    SVec & operator/=(const value_t &rhs) noexcept;
    SVec & operator%=(const value_t &rhs) noexcept;
    SVec & operator&=(const value_t &rhs) noexcept;
    SVec & operator|=(const value_t &rhs) noexcept;
    SVec & operator^=(const value_t &rhs) noexcept;

    SVec & operator+=(const SVec &rhs) noexcept;
    SVec & operator-=(const SVec &rhs) noexcept;
    SVec & operator*=(const SVec &rhs) noexcept;
    SVec & operator/=(const SVec &rhs) noexcept;
    SVec & operator%=(const SVec &rhs) noexcept;
    SVec & operator&=(const SVec &rhs) noexcept;
    SVec & operator|=(const SVec &rhs) noexcept;
    SVec & operator^=(const SVec &rhs) noexcept;

    SVec operator+() const noexcept;
    SVec operator-() const noexcept;
    SVec operator~() const noexcept;

    /**
    norm() - 2-norm.
    norm(p) - p-norm.
    squared_norm() - square of 2-norm.
    normalize() - normalize itself, according to 2-norm.
    normalize(int p) - normalize itself, according to p-norm.
    normalized() - returns a normalized (according to 2-norm) copy.
    normalized(int p) - returns a normalized (according to p-norm) copy.

    `normalize()` or `normalized()` for an integer vector are ill-defined. 
    `norm()` with `ResT` != floting-point type may have truncation.
    With caution to use.
    */
    template<typename ResT = double> 
    ResT norm() const noexcept;
    template<typename ResT = double> 
    ResT norm(int p) const noexcept;
    template<typename ResT = double> 
    ResT squared_norm() const noexcept;
    
    SVec & normalize() noexcept;
    SVec & normalize(int p) noexcept;
    SVec normalized() const noexcept;
    SVec normalized(int p) const noexcept;

    /**
    Reduction operations.
    sum(), prod(), mean() - the summation, product, and mean of all elements.
    dot(), cross() - dot product and cross product.
    
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

    template<typename ResT = value_t>
    ResT dot(const SVec &rhs) const noexcept;
    template<typename ResT = value_t>
    SVec<ResT, SIZE> cross(const SVec &rhs) const noexcept;

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
    SVec & map(UnaryOp op);

    template<typename UnaryOp, 
        typename ResT = std::invoke_result_t<UnaryOp, value_t> >
    SVec<ResT, SIZE> mapped(UnaryOp op) const;

    template<typename BinaryOp>
    void visit(BinaryOp op) const;
    
    template<typename BinaryOp>
    void visit(BinaryOp op);

    /** 
    Round to floor, ceil, trunc toward zero, and absolute value.
    `ResT` can be floating-point or integral type.

    By default, if `value_t` is integer or pointer, `ResT` is `value_t` itself,
    no conversion bappens. Otherwise `ResT` is `int`, and the conversion is made
    by std::floor, ceil, trunc and then cast.
    */
    template<typename ResT = int_value_t>
    SVec<ResT, SIZE> floor() const noexcept;

    template<typename ResT = int_value_t>
    SVec<ResT, SIZE> ceil() const noexcept;

    template<typename ResT = int_value_t>
    SVec<ResT, SIZE> trunc() const noexcept;

    SVec abs() const noexcept;

    bool_view_t view(const bool_mask_t &mask) noexcept;
    cbool_view_t view(const bool_mask_t &mask) const noexcept;
    cbool_view_t cview(const bool_mask_t &mask) const noexcept;
protected:
    value_t _data[SIZE];
};

/**
Aliases for convenience. 

Format: SVecPq, where P is X, 1, 2, 3 or 4, and q is d, f or i.
@P: dimension of the vector.
@q: type of its element. d, f and i are for double, float and integer, 
    respectively.
*/
template<size_t N>
using SVecXd = SVec<double, N>;
using SVec1d = SVecXd<1>;
using SVec2d = SVecXd<2>;
using SVec3d = SVecXd<3>;
using SVec4d = SVecXd<4>;

template<size_t N>
using SVecXf = SVec<float, N>;
using SVec1f = SVecXf<1>;
using SVec2f = SVecXf<2>;
using SVec3f = SVecXf<3>;
using SVec4f = SVecXf<4>;

template<size_t N>
using SVecXi = SVec<int, N>;
using SVec1i = SVecXi<1>;
using SVec2i = SVecXi<2>;
using SVec3i = SVecXi<3>;
using SVec4i = SVecXi<4>;

/* Implementation. */
_HIPP_TEMPNORET
SVec(const value_t &value) noexcept { std::fill_n(_data, SIZE, value); }

_HIPP_TEMPHD
template<typename InputValue>
_HIPP_TEMPCLS::SVec(const InputValue *b, size_t n) noexcept {
    std::copy_n(b, n, _data);
}

_HIPP_TEMPHD
template<typename InputValue>
_HIPP_TEMPCLS::SVec(std::initializer_list<InputValue> il) noexcept {
    auto b = il.begin(), e = il.end();
    size_t i = 0;
    while( b != e )
        _data[i++] = *b++;
}

_HIPP_TEMPHD
template<typename InputValue>
_HIPP_TEMPCLS::SVec(const SVec<InputValue, SIZE> &v) noexcept {
    std::copy_n(v.data(), SIZE, _data);
}

_HIPP_TEMPHD
void swap(_HIPP_TEMPCLS &lhs, _HIPP_TEMPCLS &rhs) noexcept {
    std::swap_ranges(lhs.begin(), lhs.end(), rhs.begin());
}

_HIPP_TEMPHD
ostream & operator<< (ostream &os, const SVec _HIPP_TEMPARG & v) {
    PStream ps(os);
    ps << "SVec{", ps(v.begin(), v.end()), "}";
    return os;
}

_HIPP_TEMPRET info(ostream &os, int fmt_cntl) const -> ostream & {
    PStream ps(os);
    if(fmt_cntl == 0) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(SVec), 
            "{", ps(cbegin(), cend()), "}";
    }else {
        ps << HIPPCNTL_CLASS_INFO(SVec), 
            "  |- size = ", SIZE, "\n"
            "  |- values = {", ps(cbegin(), cend()), "}\n";
    }
    return os;
}

_HIPP_TEMPRET data() noexcept -> value_t * {
    return _data;
}

_HIPP_TEMPRET data() const noexcept -> const value_t * {
    return _data;
}

_HIPP_TEMPHD 
constexpr size_t _HIPP_TEMPCLS::size() noexcept {
    return SIZE;
}

_HIPP_TEMPHD
constexpr bool _HIPP_TEMPCLS::empty() noexcept {
    return SIZE == 0;
}

_HIPP_TEMPRET operator[](size_t pos) noexcept -> ref_t {
    return _data[pos];
}

_HIPP_TEMPRET operator[](size_t pos) const noexcept -> cref_t {
    return _data[pos];
}

_HIPP_TEMPRET operator[](const bool_mask_t &mask) noexcept -> bool_view_t { 
    return view(mask); 
}

_HIPP_TEMPRET operator[](const bool_mask_t &mask) const noexcept -> cbool_view_t { 
    return view(mask); 
}

_HIPP_TEMPRET at(size_t pos) -> ref_t {
    if( pos >= SIZE )
        ErrLogic::throw_(ErrLogic::eOUTOFRANGE, emFLPFB, "   ... pos ", 
        pos, " >= size ", SIZE, '\n');    
    return _data[pos];
}

_HIPP_TEMPRET at(size_t pos) const -> cref_t {
    if( pos >= SIZE )
        ErrLogic::throw_(ErrLogic::eOUTOFRANGE, emFLPFB, "   ... pos ", 
        pos, " >= size ", SIZE, '\n');    
    return _data[pos];
}

_HIPP_TEMPRET begin() noexcept -> iter_t {
    return _data;
}

_HIPP_TEMPRET begin() const noexcept -> citer_t {
    return _data;
}

_HIPP_TEMPRET cbegin() const noexcept -> citer_t {
    return _data;
}

_HIPP_TEMPRET end() noexcept -> iter_t {
    return _data+SIZE;
}

_HIPP_TEMPRET end() const noexcept -> citer_t {
    return _data+SIZE;
}

_HIPP_TEMPRET cend() const noexcept -> citer_t {
    return _data+SIZE;
}

#define _HIPP_UNARY_OP_DEF(op) \
_HIPP_TEMPRET operator op (const value_t &rhs) noexcept -> SVec & { \
    for(size_t i=0; i<SIZE; ++i) _data[i]  op  rhs; \
    return *this; \
} \
_HIPP_TEMPRET operator op (const SVec &rhs) noexcept -> SVec & { \
    for(size_t i=0; i<SIZE; ++i) _data[i]  op  rhs._data[i]; \
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
_HIPP_TEMPRET operator op () const noexcept -> SVec { \
    SVec ret; \
    for(size_t i=0; i<SIZE; ++i) ret[i] = op _data[i]; \
    return ret; \
} \

_HIPP_UNARY_OP_DEF(+)
_HIPP_UNARY_OP_DEF(-)
_HIPP_UNARY_OP_DEF(~)

#undef _HIPP_UNARY_OP_DEF

#define _HIPP_BIN_OP_DEF(op)  \
_HIPP_TEMPHD \
_HIPP_TEMPCLS operator op (const typename _HIPP_TEMPCLS::value_t &lhs,  \
    const _HIPP_TEMPCLS &rhs) noexcept  \
{ \
    _HIPP_TEMPCLS ret; \
    for(size_t i=0; i<N; ++i) ret[i] = lhs op rhs[i]; \
    return ret; \
} \
_HIPP_TEMPHD \
_HIPP_TEMPCLS operator op (const _HIPP_TEMPCLS &lhs,  \
    const typename _HIPP_TEMPCLS::value_t& rhs) noexcept { \
    _HIPP_TEMPCLS ret; \
    for(size_t i=0; i<N; ++i) ret[i] = lhs[i] op rhs; \
    return ret; \
} \
_HIPP_TEMPHD \
_HIPP_TEMPCLS operator op (const _HIPP_TEMPCLS &lhs,  \
    const _HIPP_TEMPCLS &rhs) noexcept { \
    _HIPP_TEMPCLS ret; \
    for(size_t i=0; i<N; ++i) ret[i] = lhs[i] op rhs[i]; \
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
_HIPP_TEMPCLS_B operator op (const typename _HIPP_TEMPCLS::value_t &lhs,  \
    const _HIPP_TEMPCLS &rhs) noexcept  \
{ \
    _HIPP_TEMPCLS_B ret; \
    for(size_t i=0; i<N; ++i) ret[i] = lhs op rhs[i]; \
    return ret; \
} \
_HIPP_TEMPHD \
_HIPP_TEMPCLS_B operator op (const _HIPP_TEMPCLS &lhs,  \
    const typename _HIPP_TEMPCLS::value_t& rhs) noexcept { \
    _HIPP_TEMPCLS_B ret; \
    for(size_t i=0; i<N; ++i) ret[i] = lhs[i] op rhs; \
    return ret; \
} \
_HIPP_TEMPHD \
_HIPP_TEMPCLS_B operator op (const _HIPP_TEMPCLS &lhs,  \
    const _HIPP_TEMPCLS &rhs) noexcept { \
    _HIPP_TEMPCLS_B ret; \
    for(size_t i=0; i<N; ++i) ret[i] = lhs[i] op rhs[i]; \
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
    for(size_t i=0; i<SIZE; ++i) {
        ret += std::pow(_data[i], p);
    }
    return std::pow(ret, 1.0/static_cast<double>(p));
}

_HIPP_TEMPHD
template<typename ResT>
ResT _HIPP_TEMPCLS::squared_norm() const noexcept {
    ResT ret {0};
    for(size_t i=0; i<SIZE; ++i){
        auto x = static_cast<ResT>(_data[i]);
        ret += x*x;
    }
    return ret;
}

_HIPP_TEMPRET normalize() noexcept -> SVec & {
    auto scale = 1.0 / norm();
    return this->operator*=( static_cast<value_t>(scale) );
}

_HIPP_TEMPRET normalize(int p) noexcept -> SVec & {
    auto scale = 1.0 / norm(p);
    return this->operator*=( static_cast<value_t>(scale) );
}

_HIPP_TEMPRET normalized() const noexcept -> SVec{
    auto scale = 1.0 / norm();
    return (*this) * static_cast<value_t>(scale);
}

_HIPP_TEMPRET normalized(int p) const noexcept -> SVec{
    auto scale = 1.0 / norm(p);
    return (*this) * static_cast<value_t>(scale);
}

_HIPP_TEMPHD 
template<typename ResT> 
auto _HIPP_TEMPCLS::sum() const noexcept -> ResT {
    ResT ret {0};
    for(size_t i=0; i<SIZE; ++i) ret += _data[i];
    return ret;
}

_HIPP_TEMPHD 
template<typename ResT> 
auto _HIPP_TEMPCLS::prod() const noexcept -> ResT {
    ResT ret {1};
    for(size_t i=0; i<SIZE; ++i) ret *= _data[i];
}

_HIPP_TEMPHD 
template<typename ResT> 
auto _HIPP_TEMPCLS::mean() const noexcept -> ResT {
    static_assert(SIZE != 0);
    return sum<ResT>() / static_cast<ResT>(SIZE);
}

_HIPP_TEMPHD 
template<typename ResT> 
auto _HIPP_TEMPCLS::dot(const SVec &rhs) const noexcept -> ResT {
    ResT ret {0};
    for(size_t i=0; i<SIZE; ++i){
        ret += static_cast<ResT>(_data[i])*static_cast<ResT>(rhs._data[i]);  
    } 
    return ret;
}

_HIPP_TEMPHD 
template<typename ResT> 
auto _HIPP_TEMPCLS::cross(const SVec &rhs) const noexcept -> SVec<ResT, SIZE> {
    SVec<ResT, SIZE> ret;
    for(size_t i=0; i<SIZE; ++i){
        size_t j = (i+1)%SIZE, k = (i+2)%SIZE;
        ret[i] = static_cast<ResT>(_data[j])*static_cast<ResT>(rhs._data[k]) 
            - static_cast<ResT>(_data[k])*static_cast<ResT>(rhs._data[j]);
    }
    return ret;
}

_HIPP_TEMPRET min() const noexcept -> value_t {
    static_assert(SIZE != 0);
    value_t ret { std::numeric_limits<value_t>::max() };
    for(size_t i=0; i<SIZE; ++i){
        if( _data[i] < ret ) ret = _data[i];
    }
    return ret;
}

_HIPP_TEMPRET max() const noexcept -> value_t {
    static_assert(SIZE != 0);
    value_t ret { std::numeric_limits<value_t>::lowest() };
    for(size_t i=0; i<SIZE; ++i){
        if( _data[i] > ret ) ret = _data[i];
    }
    return ret;
}

_HIPP_TEMPRET minmax() const noexcept -> std::pair<value_t, value_t> {
    static_assert(SIZE != 0);
    value_t ret_min = { std::numeric_limits<value_t>::max() },
        ret_max = { std::numeric_limits<value_t>::lowest() };
    for(size_t i=0; i<SIZE; ++i){
        value_t e = _data[i];
        if( e < ret_min ) ret_min = e;
        if( e > ret_max ) ret_max = e;
    }
    return {ret_min, ret_max};
}

_HIPP_TEMPRET min_index() const noexcept -> size_t {
    static_assert(SIZE != 0);
    auto it = std::min_element(_data, _data+SIZE);
    return static_cast<size_t>(it - _data);
}

_HIPP_TEMPRET  max_index() const noexcept -> size_t {
    static_assert(SIZE != 0);
    auto it = std::max_element(_data, _data+SIZE);
    return static_cast<size_t>(it - _data);
}

_HIPP_TEMPRET minmax_index() const noexcept -> std::pair<size_t, size_t>{
    static_assert(SIZE != 0);
    auto [it_min, it_max] = std::minmax_element(_data, _data+SIZE);
    return { static_cast<size_t>(it_min-_data), 
        static_cast<size_t>(it_max-_data) };
}

_HIPP_TEMPRET all() const noexcept -> bool {
    for(size_t i=0; i<SIZE; ++i){
        if( !_data[i] ) return false;
    }
    return true;
}

_HIPP_TEMPRET any() const noexcept -> bool {
    for(size_t i=0; i<SIZE; ++i){
        if( _data[i] ) return true;
    }
    return false;
}

_HIPP_TEMPHD
template<typename UnaryOp>
auto _HIPP_TEMPCLS::map(UnaryOp op) -> SVec & {
    for(size_t i=0; i<SIZE; ++i){
        _data[i] = op(_data[i]);
    }
    return *this;
}

_HIPP_TEMPHD
template<typename UnaryOp, typename ResT>
auto _HIPP_TEMPCLS::mapped(UnaryOp op) const -> SVec<ResT, SIZE> {
    SVec<ResT, SIZE> ret;
    for(size_t i=0; i<SIZE; ++i){
        ret[i] = op(_data[i]);
    }
    return ret;
}

_HIPP_TEMPHD
template<typename BinaryOp>
void _HIPP_TEMPCLS::visit(BinaryOp op) const {
    for(size_t i=0; i<SIZE; ++i) op(i, _data[i]);
}

_HIPP_TEMPHD
template<typename BinaryOp>
void _HIPP_TEMPCLS::visit(BinaryOp op) {
    for(size_t i=0; i<SIZE; ++i) op(i, _data[i]);
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::floor() const noexcept -> SVec<ResT, SIZE> 
{
    SVec<ResT, SIZE> res;
    if constexpr ( IS_INT ) {
        for(size_t i=0; i<SIZE; ++i) res[i] = static_cast<ResT>(_data[i]);
    } else {
        for(size_t i=0; i<SIZE; ++i) 
            res[i] = static_cast<ResT>(std::floor(_data[i]));
    }
    return res;
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::ceil() const noexcept -> SVec<ResT, SIZE> 
{
    SVec<ResT, SIZE> res;
    if constexpr ( IS_INT ) {
        for(size_t i=0; i<SIZE; ++i) res[i] = static_cast<ResT>(_data[i]);
    } else {
        for(size_t i=0; i<SIZE; ++i) 
            res[i] = static_cast<ResT>(std::ceil(_data[i]));
    }
    return res;
}

_HIPP_TEMPHD
template<typename ResT>
auto _HIPP_TEMPCLS::trunc() const noexcept -> SVec<ResT, SIZE> 
{
    SVec<ResT, SIZE> res;
    if constexpr ( IS_INT ) {
        for(size_t i=0; i<SIZE; ++i) res[i] = static_cast<ResT>(_data[i]);
    } else {
        for(size_t i=0; i<SIZE; ++i) 
            res[i] = static_cast<ResT>(std::trunc(_data[i]));
    }
    return res;
}

_HIPP_TEMPRET abs() const noexcept -> SVec {
    SVec ret;
    for(size_t i=0; i<SIZE; ++i) ret[i] = std::abs(_data[i]);
    return ret;
}

_HIPP_TEMPRET view(const bool_mask_t &mask) noexcept -> bool_view_t { 
    return bool_view_t(*this, bool_filter_t(mask) ); 
}

_HIPP_TEMPRET view(const bool_mask_t &mask) const noexcept -> cbool_view_t { 
    return cbool_view_t(*this, bool_filter_t(mask) ); 
}

_HIPP_TEMPRET cview(const bool_mask_t &mask) const noexcept -> cbool_view_t { 
    return cbool_view_t(*this, bool_filter_t(mask) ); 
}

template<size_t I, typename ValueT, size_t N>
const ValueT & get(const SVec _HIPP_TEMPARG &v) noexcept { return v[I]; }

template<size_t I, typename ValueT, size_t N>
ValueT & get(SVec _HIPP_TEMPARG &v) noexcept { return v[I]; }

template<size_t I, typename ValueT, size_t N>
ValueT && get(SVec _HIPP_TEMPARG &&v) noexcept { return std::move(v[I]); }

} // namespace HIPP::NUMERICAL


/** The tuple-like API. */
namespace std {

template<typename ValueT, size_t N>
struct tuple_size<HIPP::NUMERICAL::_HIPP_TEMPCLS >;

template<size_t I, typename ValueT, size_t N>
struct tuple_element<I, HIPP::NUMERICAL::_HIPP_TEMPCLS >;

} // namespace std

template<typename ValueT, size_t N>
struct std::tuple_size<HIPP::NUMERICAL::_HIPP_TEMPCLS > {
    inline static constexpr size_t value = N;
};

template<size_t I, typename ValueT, size_t N>
struct std::tuple_element<I, HIPP::NUMERICAL::_HIPP_TEMPCLS > {
    using type = ValueT;
};


/** Specialization of the HIPP RawArrayTraits API */

namespace HIPP {

template<typename ValueT, size_t N> 
class RawArrayTraits< HIPP::NUMERICAL::SVec<ValueT, N> > 
: public RawArrayTraits< ValueT[N] > {};


} // namespace HIPP


#undef _HIPP_TEMPNORET
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPCLS_B
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPHD

#endif	//_HIPPNUMERICAL_LINALG_SVEC_H_