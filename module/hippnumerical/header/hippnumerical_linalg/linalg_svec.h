/**
create: Yangyao CHEN, 2021/05/28
    [write   ] SVec - static/small vector with fixed length.
*/

#ifndef _HIPPNUMERICAL_LINALG_SVEC_H_
#define _HIPPNUMERICAL_LINALG_SVEC_H_
#include "linalg_base.h"
#include <limits>
#include <tuple>

#define _HIPP_TEMPHD template<typename ValueT, size_t N>
#define _HIPP_TEMPARG <ValueT, N>
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto SVec _HIPP_TEMPARG::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline SVec _HIPP_TEMPARG::

namespace HIPP::NUMERICAL {

/* Forward declarations. */
_HIPP_TEMPHD class SVec;

_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator+ (const typename SVec _HIPP_TEMPARG::value_t &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept;
_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator- (const typename SVec _HIPP_TEMPARG::value_t &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept;
_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator* (const typename SVec _HIPP_TEMPARG::value_t &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept;
_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator/ (const typename SVec _HIPP_TEMPARG::value_t &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept;

_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator+ (const SVec _HIPP_TEMPARG &lhs, 
    const typename SVec _HIPP_TEMPARG::value_t &rhs) noexcept;
_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator- (const SVec _HIPP_TEMPARG &lhs, 
    const typename SVec _HIPP_TEMPARG::value_t &rhs) noexcept;
_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator* (const SVec _HIPP_TEMPARG &lhs, 
    const typename SVec _HIPP_TEMPARG::value_t &rhs) noexcept;
_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator/ (const SVec _HIPP_TEMPARG &lhs, 
    const typename SVec _HIPP_TEMPARG::value_t &rhs) noexcept;

_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator+ (const SVec _HIPP_TEMPARG &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept;
_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator- (const SVec _HIPP_TEMPARG &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept;
_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator* (const SVec _HIPP_TEMPARG &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept;
_HIPP_TEMPHD 
SVec _HIPP_TEMPARG operator/ (const SVec _HIPP_TEMPARG &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept;

_HIPP_TEMPHD 
ostream & operator<<(ostream &os, const SVec _HIPP_TEMPARG &);
_HIPP_TEMPHD 
void swap(SVec _HIPP_TEMPARG &lhs, SVec _HIPP_TEMPARG &rhs) noexcept;

/**
SVec - static/small vector with fixed length.

Template arguments
------------------
@ValueT: element type, usually a numerical scalar.
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
*/
template<typename ValueT, size_t N>
class SVec {
public:
    typedef ValueT value_t;                     /* Element type. */
    inline static constexpr size_t SIZE = N;    /* Vector length. */
    typedef value_t & ref_t;               /* Reference to an element. */
    typedef const value_t & cref_t;        /* const ... */
    typedef value_t *iter_t;               /* Iterator pointing to an element */
    typedef const value_t *citer_t;        /* const ... */

    /**
    (1) Default initialization of all elements. Caution for numeric types.
    (2) All members are initialized with `value`.
    (3) Copy from a range starting from `b`, with `n` elements.
    (4) From an initializer list.
    In (3) and (4), `n` and `il.size()` may be less than `SIZE`, leaving 
    the tail un-initialized.
    */
    SVec() noexcept {}
    explicit SVec(const value_t &value) noexcept;
    
    template<typename InputValue>
    explicit SVec(const InputValue *b, size_t n = SIZE) noexcept;
    
    template<typename InputValue>
    SVec(std::initializer_list<InputValue> il) noexcept;

    SVec(const SVec &) noexcept = default;
    SVec(SVec &&)  noexcept = default;
    SVec & operator=(const SVec &) noexcept = default;
    SVec & operator=(SVec &&)  noexcept = default;
    ~SVec() noexcept {}

    friend ostream & operator<< <ValueT, N> (ostream &os, const SVec &);
    ostream & info(ostream &os=cout, int fmt_cntl=1) const;

    /**
    STL-conforming definitions - semantics are like std::vector.
    `swap()` - deep, all elements are swapped.
    `size()` - always returns `SIZE`.
    `empty()` - returns true only if `SIZE == 0`.
    */
    friend void swap <ValueT, N> (SVec &lhs, SVec &rhs) noexcept;
    value_t * data() noexcept;
    const value_t * data() const noexcept;
    static constexpr size_t size() noexcept;
    static constexpr bool empty() noexcept;

    ref_t operator[](size_t pos) noexcept;
    cref_t operator[](size_t pos) const noexcept;
    ref_t at(size_t pos);
    cref_t at(size_t pos) const;

    iter_t begin() noexcept;
    citer_t begin() const noexcept;
    citer_t cbegin() const noexcept;
    iter_t end() noexcept;
    citer_t end() const noexcept;
    citer_t cend() const noexcept;

    /**
    Linear algebra operations. All are element-wise operations.
    Possible combinations for unary RMW are:
    (1) vec Op= scalar;
    (2) vec Op= vec;
    Possible combinations for binary operation are:
    (1) vec Op scalar;
    (2) scalar Op vec;
    (3) vec Op vec;
    */
    SVec & operator+=(const value_t &rhs) noexcept;
    SVec & operator-=(const value_t &rhs) noexcept;
    SVec & operator*=(const value_t &rhs) noexcept;
    SVec & operator/=(const value_t &rhs) noexcept;

    SVec & operator+=(const SVec &rhs) noexcept;
    SVec & operator-=(const SVec &rhs) noexcept;
    SVec & operator*=(const SVec &rhs) noexcept;
    SVec & operator/=(const SVec &rhs) noexcept;

    friend SVec operator+ <ValueT, N> (const value_t &lhs, 
        const SVec &rhs) noexcept;
    friend SVec operator- <ValueT, N> (const value_t &lhs, 
        const SVec &rhs) noexcept;
    friend SVec operator* <ValueT, N> (const value_t &lhs, 
        const SVec &rhs) noexcept;
    friend SVec operator/ <ValueT, N> (const value_t &lhs, 
        const SVec &rhs) noexcept;
    
    friend SVec operator+ <ValueT, N> (const SVec &lhs, 
        const value_t &rhs) noexcept;
    friend SVec operator- <ValueT, N> (const SVec &lhs, 
        const value_t &rhs) noexcept;
    friend SVec operator* <ValueT, N> (const SVec &lhs, 
        const value_t &rhs) noexcept;
    friend SVec operator/ <ValueT, N> (const SVec &lhs, 
        const value_t &rhs) noexcept;

    friend SVec operator+ <ValueT, N> (const SVec &lhs, 
        const SVec &rhs) noexcept;
    friend SVec operator- <ValueT, N> (const SVec &lhs, 
        const SVec &rhs) noexcept;
    friend SVec operator* <ValueT, N> (const SVec &lhs, 
        const SVec &rhs) noexcept;
    friend SVec operator/ <ValueT, N> (const SVec &lhs, 
        const SVec &rhs) noexcept;

    /**
    `normalize()` or `normalized()` for an integer vector are ill-defined. 
    With caution to use.
    norm() - 2-norm.
    norm(p) - p-norm.
    squared_norm() - square of 2-norm.
    normalize() - normalize itself, according to 2-norm.
    normalize(int p) - normalize itself, according to p-norm.
    normalized() - returns a normalized (according to 2-norm) copy.
    normalized(int p) - returns a normalized (according to p-norm) copy.
    */
    double norm() const noexcept;
    double norm(int p) const noexcept;
    double squared_norm() const noexcept;
    SVec & normalize() noexcept;
    SVec & normalize(int p) noexcept;
    SVec normalized() const noexcept;
    SVec normalized(int p) const noexcept;

    /**
    Reduction operations.
    sum(), prod() - the summation or product of all elements.
    mean(), min(), max(), minmax() - as you expect. The indexed-version returns
        the element index if the corresponding result.
    dot(), cross() - dot product and cross product.
    */
    value_t sum() const noexcept;
    value_t prod() const noexcept; 
    value_t mean() const noexcept;
    value_t min() const noexcept;
    value_t max() const noexcept;
    std::pair<value_t, value_t> minmax() const noexcept;
    value_t dot(const SVec &rhs) const noexcept;
    SVec cross(const SVec &rhs) const noexcept;
    
    size_t min_index() const noexcept;
    size_t max_index() const noexcept;
    std::pair<size_t, size_t> minmax_index() const noexcept;

    /**
    Map reduce operations.
    map() - for each i, self[i] = op(self[i]).
    mapped() - returns a mapped copy.
    */
    template<typename UnaryOp>
    SVec & map(UnaryOp op);
    template<typename UnaryOp>
    SVec mapped(UnaryOp op) const;
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
SVec _HIPP_TEMPARG::SVec(const InputValue *b, size_t n) noexcept {
    std::copy_n(b, n, _data);
}

_HIPP_TEMPHD
template<typename InputValue>
SVec _HIPP_TEMPARG::SVec(std::initializer_list<InputValue> il) noexcept {
    auto b = il.begin(), e = il.end();
    size_t i = 0;
    while( b != e )
        _data[i++] = *b++;
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

_HIPP_TEMPHD
void swap(SVec _HIPP_TEMPARG &lhs, SVec _HIPP_TEMPARG &rhs) noexcept {
    std::swap_ranges(lhs.begin(), lhs.end(), rhs.begin());
}

_HIPP_TEMPRET data() noexcept -> value_t * {
    return _data;
}

_HIPP_TEMPRET data() const noexcept -> const value_t * {
    return _data;
}

_HIPP_TEMPHD 
constexpr size_t SVec _HIPP_TEMPARG::size() noexcept {
    return SIZE;
}

_HIPP_TEMPHD
constexpr bool SVec _HIPP_TEMPARG::empty() noexcept {
    return SIZE == 0;
}

_HIPP_TEMPRET operator[](size_t pos) noexcept -> ref_t {
    return _data[pos];
}

_HIPP_TEMPRET operator[](size_t pos) const noexcept -> cref_t {
    return _data[pos];
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

_HIPP_TEMPRET operator+=(const value_t &rhs) noexcept -> SVec & {
    for(size_t i=0; i<SIZE; ++i) _data[i] += rhs;
    return *this;
}

_HIPP_TEMPRET operator-=(const value_t &rhs) noexcept -> SVec & {
    for(size_t i=0; i<SIZE; ++i) _data[i] -= rhs;
    return *this;
}

_HIPP_TEMPRET operator*=(const value_t &rhs) noexcept -> SVec & {
    for(size_t i=0; i<SIZE; ++i) _data[i] *= rhs;
    return *this;
}

_HIPP_TEMPRET operator/=(const value_t &rhs) noexcept -> SVec & {
    for(size_t i=0; i<SIZE; ++i) _data[i] /= rhs;
    return *this;
}

_HIPP_TEMPRET operator+=(const SVec &rhs) noexcept -> SVec & {
    for(size_t i=0; i<SIZE; ++i) _data[i] += rhs._data[i];
    return *this;
}

_HIPP_TEMPRET operator-=(const SVec &rhs) noexcept -> SVec & {
    for(size_t i=0; i<SIZE; ++i) _data[i] -= rhs._data[i];
    return *this;
}

_HIPP_TEMPRET operator*=(const SVec &rhs) noexcept -> SVec & {
    for(size_t i=0; i<SIZE; ++i) _data[i] *= rhs._data[i];
    return *this;
}

_HIPP_TEMPRET operator/=(const SVec &rhs) noexcept -> SVec & {
    for(size_t i=0; i<SIZE; ++i) _data[i] /= rhs._data[i];
    return *this;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator+ (const typename SVec _HIPP_TEMPARG::value_t &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs + rhs._data[i];
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator- (const typename SVec _HIPP_TEMPARG::value_t &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs - rhs._data[i];
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator* (const typename SVec _HIPP_TEMPARG::value_t &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs * rhs._data[i];
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator/ (const typename SVec _HIPP_TEMPARG::value_t &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs / rhs._data[i];
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator+ (const SVec _HIPP_TEMPARG &lhs, 
    const typename SVec _HIPP_TEMPARG::value_t& rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs._data[i] + rhs;
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator- (const SVec _HIPP_TEMPARG &lhs, 
    const typename SVec _HIPP_TEMPARG::value_t& rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs._data[i] - rhs;
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator* (const SVec _HIPP_TEMPARG &lhs, 
    const typename SVec _HIPP_TEMPARG::value_t& rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs._data[i] * rhs;
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator/ (const SVec _HIPP_TEMPARG &lhs, 
    const typename SVec _HIPP_TEMPARG::value_t& rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs._data[i] / rhs;
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator+ (const SVec _HIPP_TEMPARG &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs._data[i] + rhs._data[i];
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator- (const SVec _HIPP_TEMPARG &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs._data[i] - rhs._data[i];
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator* (const SVec _HIPP_TEMPARG &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs._data[i] * rhs._data[i];
    return ret;
}

_HIPP_TEMPHD
SVec _HIPP_TEMPARG operator/ (const SVec _HIPP_TEMPARG &lhs, 
    const SVec _HIPP_TEMPARG &rhs) noexcept {
    SVec _HIPP_TEMPARG ret;
    for(size_t i=0; i<N; ++i) ret._data[i] = lhs._data[i] / rhs._data[i];
    return ret;
}


_HIPP_TEMPRET norm() const noexcept -> double {
    return std::sqrt(squared_norm());
}

_HIPP_TEMPRET norm(int p) const noexcept -> double {
    double ret = 0.;
    for(size_t i=0; i<SIZE; ++i) ret += std::pow(_data[i], p);
    return std::pow(ret, 1.0/static_cast<double>(p) );
}

_HIPP_TEMPRET squared_norm() const noexcept -> double {
    double ret = 0.;
    for(size_t i=0; i<SIZE; ++i) ret += _data[i]*_data[i];
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

_HIPP_TEMPRET sum() const noexcept -> value_t {
    value_t ret {0};
    for(size_t i=0; i<SIZE; ++i) ret += _data[i];
    return ret;
}

_HIPP_TEMPRET prod() const noexcept -> value_t {
    value_t ret {1};
    for(size_t i=0; i<SIZE; ++i) ret *= _data[i];
}

_HIPP_TEMPRET mean() const noexcept -> value_t {
    assert(SIZE != 0);
    return sum() / static_cast<value_t>(SIZE);
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

_HIPP_TEMPRET dot(const SVec &rhs) const noexcept -> value_t {
    value_t ret {0};
    for(size_t i=0; i<SIZE; ++i) ret += _data[i]*rhs._data[i];
    return ret;
}

_HIPP_TEMPRET cross(const SVec &rhs) const noexcept -> SVec {
    SVec ret;
    for(size_t i=0; i<SIZE; ++i){
        size_t j = (i+1)%SIZE, k = (i+2)%SIZE;
        ret[i] = _data[j]*rhs._data[k] - _data[k]*rhs._data[j];
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

_HIPP_TEMPHD
template<typename UnaryOp>
auto SVec _HIPP_TEMPARG::map(UnaryOp op) -> SVec & {
    for(size_t i=0; i<SIZE; ++i){
        _data[i] = op(_data[i]);
    }
    return *this;
}

_HIPP_TEMPHD
template<typename UnaryOp>
auto SVec _HIPP_TEMPARG::mapped(UnaryOp op) const -> SVec {
    SVec ret;
    for(size_t i=0; i<SIZE; ++i){
        ret._data[i] = op(_data[i]);
    }
    return ret;
}

template<size_t I, typename ValueT, size_t N>
const ValueT & get(const SVec _HIPP_TEMPARG &v) noexcept { return v[I]; }

template<size_t I, typename ValueT, size_t N>
ValueT & get(SVec _HIPP_TEMPARG &v) noexcept { return v[I]; }

template<size_t I, typename ValueT, size_t N>
ValueT && get(SVec _HIPP_TEMPARG &&v) noexcept { return std::move(v[I]); }

} // namespace HIPP::NUMERICAL

template<typename ValueT, size_t N>
struct std::tuple_size<HIPP::NUMERICAL::SVec _HIPP_TEMPARG > {
    inline static constexpr size_t value = N;
};

template<size_t I, typename ValueT, size_t N>
struct std::tuple_element<I, HIPP::NUMERICAL::SVec _HIPP_TEMPARG > {
    using type = ValueT;
};


#undef _HIPP_TEMPNORET
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPHD

#endif	//_HIPPNUMERICAL_LINALG_SVEC_H_