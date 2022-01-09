/**
create: Yangyao CHEN, 2021/07/30
    [write   ] DArray - dynamic N-dimensional array.
*/

#ifndef _HIPPNUMERICAL_LINALG_DARRAYND_H_
#define _HIPPNUMERICAL_LINALG_DARRAYND_H_
#include "linalg_sarray.h"

#define _HIPP_TEMPHD template<typename ValueT, size_t Rank, typename Alloc>
#define _HIPP_TEMPARG <ValueT, Rank, Alloc>
#define _HIPP_TEMPCLS DArray _HIPP_TEMPARG

#define _HIPP_TEMPHD_B \
    template<typename ValueT, size_t Rank, typename Alloc, \
        typename AllocB=std::allocator<bool> >
#define _HIPP_TEMPCLS_B DArray<bool, Rank, AllocB>

#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

namespace HIPP::NUMERICAL {

/** Forward declaration. */
_HIPP_TEMPHD class DArray;

_HIPP_TEMPHD ostream & operator<<(ostream &os, const _HIPP_TEMPCLS &);
_HIPP_TEMPHD void swap(_HIPP_TEMPCLS &lhs, _HIPP_TEMPCLS &rhs) noexcept;

/** Base class for all DArray. */
struct DArrayBase {
    /**
    size_hint_t - hint the DArray implementation about its dynamical size.
    This type is defined for type matching.
    */
    struct size_hint_t {
        size_t size;
        explicit constexpr size_hint_t(size_t _size) : size(_size) {}
    };
};
} // namespace HIPP::NUMERICAL

namespace HIPP {

/** The DynamicArray protocol */
_HIPP_TEMPHD
class DynamicArrayTraits< NUMERICAL::_HIPP_TEMPCLS > {
public:
    inline static constexpr bool is_array = true;
    inline static constexpr size_t rank = Rank;

    typedef NUMERICAL::_HIPP_TEMPCLS array_t;
    typedef ValueT value_t;

    DynamicArrayTraits(array_t &a) : array(a) {}

    value_t * buff() const noexcept { return array.data(); }
    size_t size() const noexcept { return array.size(); }
    std::array<size_t, rank> extents() const noexcept { 
        std::array<size_t, rank> out;
        std::copy_n(array.shape().data(), rank, out.data());
        return out;
    }
    std::array<size_t, rank> strides() const noexcept {
        auto ext = extents();
        std::array<size_t, rank> out;
        out[rank-1] = 1;
        for(size_t i=rank-1; i>0; --i)
            out[i-1] = out[i] * ext[i];
        return out;
    }

    array_t &array;
};

_HIPP_TEMPHD
class DynamicArrayTraits< const NUMERICAL::_HIPP_TEMPCLS > {
public:
    inline static constexpr bool is_array = true;
    inline static constexpr size_t rank = Rank;

    typedef const NUMERICAL::_HIPP_TEMPCLS array_t;
    typedef std::add_const_t<ValueT> value_t;

    DynamicArrayTraits(array_t &a) : array(a) {}

    value_t * buff() const noexcept { return array.data(); }
    size_t size() const noexcept { return array.size(); }
    std::array<size_t, rank> extents() const noexcept { 
        std::array<size_t, rank> out;
        std::copy_n(array.shape().data(), rank, out.data());
        return out;
    }
    std::array<size_t, rank> strides() const noexcept {
        auto ext = extents();
        std::array<size_t, rank> out;
        out[rank-1] = 1;
        for(size_t i=rank-1; i>0; --i)
            out[i-1] = out[i] * ext[i];
        return out;
    }
    
    array_t &array;
};

_HIPP_TEMPHD
DynamicArrayTraits( NUMERICAL::_HIPP_TEMPCLS & ) 
-> DynamicArrayTraits< NUMERICAL::_HIPP_TEMPCLS >;

_HIPP_TEMPHD
DynamicArrayTraits( const NUMERICAL::_HIPP_TEMPCLS & ) 
-> DynamicArrayTraits< const NUMERICAL::_HIPP_TEMPCLS >;

} // namespace HIPP

namespace HIPP::NUMERICAL {

/**
Dynamic Array with fixed compile-time rank (i.e., the number of dimensions).
The extents of dimensions (i.e., the shape) is specified at run-time.

Template parameters
--------------------
@ValueT: element type. Usually arithmetic scalar.
@Rank: number of dimensions.
@Alloc: allocator for memory.
*/
template<typename ValueT, size_t Rank, typename Alloc=std::allocator<ValueT> >
class DArray : public DArrayBase {
public:
    /**
    Basic aliases and properties.
    value_t: type of the array element.
    RANK: number of dimensions.
    shape_t: extents of dimensions.
    alloc_t: type of the allocator.
    */
    typedef ValueT value_t;
    inline static constexpr size_t RANK = Rank;
    typedef SVec<size_t, RANK> shape_t;
    typedef Alloc alloc_t;

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
    (1) a valid-state darray, behaving like a moved object. 
        (called an empty state)
    (2) a darray with specified ``shape``, elements are default-initialized. 
        Note that for fundamental type, the values are uninitialized.
    
    (3)-(5) the same as (2) but initialize the elements either by a single 
        ``value`` (to all elements), filled row-major with a range [b, e), 
        or a initializer list. The number of values provided by the range or 
        initializer list must be <= the desired number of elements in the 
        darray. In the case of <, the remaining elements are uninitialized.

    (6) copy another darray whose value type may be different. The values 
        get statically cast.

    (7)-(10) 
        the same as (2)-(5) but use a size equal to ``size_hint.size``. The 
        value of size hint must be equal to shape.prod() or otherwise the 
        program is ill-defined.
        These constructors are provided to avoid the overhead in computing 
        the size from the shape.
    */
    DArray() noexcept;
    
    explicit DArray(const shape_t &shape);
    
    DArray(const shape_t &shape, const value_t&value);
    
    template<typename It>
    DArray(const shape_t &shape, It b, It e);
    
    template<typename InputValue>
    DArray(const shape_t &shape, std::initializer_list<InputValue> il);
    
    template<typename InputValue>
    explicit DArray(const DArray<InputValue, Rank> &a);

    DArray(const shape_t &shape, const size_hint_t &size_hint);
    DArray(const shape_t &shape, const size_hint_t &size_hint, 
        const value_t &value);
    template<typename It>
    DArray(const shape_t &shape, const size_hint_t &size_hint, It b, It e);
    template<typename InputValue>
    DArray(const shape_t &shape, const size_hint_t &size_hint, 
        std::initializer_list<InputValue> il);
    
    /**
    Copy is deep. 
    After move, the moved darray (i.e., source object) is left an empty state,
    which can be destroyed or reconstructed using copy/move-assignment.

    Copy with a single ``value`` results in a constant-value DArray.
    */
    DArray(const DArray &);
    DArray(DArray &&)  noexcept;
    DArray & operator=(const DArray &);
    DArray & operator=(DArray &&)  noexcept;
    ~DArray() noexcept;

    DArray & operator=(const value_t &value) noexcept;

    /**
    Deep, all elements are swapped. ``swap`` can be made to DArrays with 
    different sizes or shapes.
    */
    friend void swap <ValueT, Rank> (DArray &lhs, DArray &rhs) noexcept;

    /**
    release() returns the pointer to the internal data buffer and sets the 
        instance to empty. User is responsible for the life-time management of 
        the buffer. The buffer should be freed with delete [] operator.
    reset() destructs the instance and take over an external buffer. The buffer
        is expected to be allocated with new [] and is managed by the instance.
        reset() with no parameter gives an empty darray.
        With a size hint, ``size_hint.size`` must be equal to ``shape.prod()``.
    */
    value_t * release() noexcept;
    void reset() noexcept;
    void reset(value_t *ptr, const shape_t &shape) noexcept;
    void reset(value_t *ptr, const shape_t &shape, 
        const size_hint_t &size_hint) noexcept;

    /** 
    operator<<() prints inline information of the instance.
    info() prints the instance with more controls.
    @fmt_cntl: 0 for an inline short message. 1 for a long block message.
    */
    friend ostream & operator<< <ValueT, Rank> (ostream &os, const DArray &);
    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    
    /**
    STL-conforming definitions - treating the array as a row-major linear array.
    data() - return a pointer to the internal storage.
    size() - always returns ``SIZE``.
    shape() - returns the shape, i.e., extents at all dimensions.
    empty() - returns true only if ``SIZE == 0``.

    operator[] and at() - element access. 
        - at() throws on the out-of-range.
    begin(), end() and their const counterparts - iterators.

    For an empty object
    - data() returns NULL, size() returns 0, shape returns a all-zero vector, 
      empty() returns true.
    - operator[] is forbidden, at() raises exception.
    - begin() and end(), and their const counterparts, return a valid iterable 
      range (of length 0).
    */
    const value_t *data() const noexcept;
    value_t *data() noexcept;

    size_t size() const noexcept;
    const shape_t & shape() const noexcept;
    bool empty() const noexcept;
    
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

    /** Visit an element with indices ``ids`` at each dimension. */
    template<typename ...SizeTs>
    ref_t operator()(SizeTs &&...ids) noexcept;
    template<typename ...SizeTs>
    cref_t operator()(SizeTs &&...ids) const noexcept;

    /**
    reshape(): inplace change the shape to ``new_shape``. The number of elements
        (i.e., ``size()``) and the rank cannot be changed.
    reshaped(): return a new reshaped darray ranked ``NewRank``. The number of 
        elements cannot be changed.
    The result of reshape to an empty darray is an empty one.
    */
    void reshape(const shape_t &new_shape);
    template<size_t NewRank, typename NewAlloc=alloc_t>
    DArray<ValueT, NewRank, NewAlloc> reshaped(
        const SVec<size_t, NewRank> &new_shape) const;

    /**
    resize(): change the size, i.e., the number of elements.
    If a ``new_size`` is passed, the first dimension if extended so that the 
    final size is equal to ``new_size``. ``new_size`` can be 0.
    - For empty DArray, the dimensions except the first is set to 1.
    - If impossible to resize to ``new_size`` due to inconsistency of 
      dimensions, an ErrLogic is thrown.
    If a ``new_shape`` is passed, the resulted DArray has this shape.
    The original elements are contiguously copied to the new buffer. If the 
    new size is larger than the original, ``value`` is used to fill the tail
    of the new buffer.
    If new size is smaller, original data at the tail is truncated.
    */
    void resize(size_t new_size, const value_t &value = value_t {});
    void resize(const shape_t &new_shape, const value_t &value = value_t {});

    /* Return a vector row-majorly filled with the DArray elements. */
    template<typename T = value_t, typename NewAlloc=std::allocator<T> >
    vector<T, NewAlloc> to_vector() const;

    /**
    Linear algebra operations. All are element-wise operations.
    1. Unary RMW operations:
        (1) arr op= scalar;
        (2) arr op= arr;
    Where op is 
        +, -, *, /, %           - arithmetic -> DArray &
        &, |, ^                 - bit-wise for each element -> DArray &
    2. Binary operations:
        (1) arr op scalar;
        (2) scalar op arr;
        (3) arr op arr;
    Where op is 
        +, -, *, /, %           - arithmetic -> DArray
        &, |, ^                 - bit-wise for each element -> DArray
        <, <=, >, >=, ==, !=    - comparison/logic -> DArray<bool, Rank>
    For empty Darray, the result is empty.
    3. Unary operations: 
        op arr;
    Where op is
        ~, +, -                 - bit-wise NOT, arithmetic positate, negate. 
    Caution: interger-lift is used intermediately for small integers, like 
    `bool`, `char`, etc., so that ~true != false. But &, |, ^ work just as
    expected.
    */
    DArray & operator+=(const value_t &rhs) noexcept;
    DArray & operator-=(const value_t &rhs) noexcept;
    DArray & operator*=(const value_t &rhs) noexcept;
    DArray & operator/=(const value_t &rhs) noexcept;
    DArray & operator%=(const value_t &rhs) noexcept;
    DArray & operator&=(const value_t &rhs) noexcept;
    DArray & operator|=(const value_t &rhs) noexcept;
    DArray & operator^=(const value_t &rhs) noexcept;

    DArray & operator+=(const DArray &rhs);
    DArray & operator-=(const DArray &rhs);
    DArray & operator*=(const DArray &rhs);
    DArray & operator/=(const DArray &rhs);
    DArray & operator%=(const DArray &rhs);
    DArray & operator&=(const DArray &rhs);
    DArray & operator|=(const DArray &rhs);
    DArray & operator^=(const DArray &rhs);

    DArray operator+() const;
    DArray operator-() const;
    DArray operator~() const;

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
    
    DArray & normalize() noexcept;
    DArray & normalize(int p) noexcept;
    DArray normalized() const;
    DArray normalized(int p) const;

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
    DArray & map(UnaryOp op);

    template<typename UnaryOp, 
        typename ResT = std::invoke_result_t<UnaryOp, value_t>,
        typename NewAlloc = std::allocator<ResT> >
    DArray<ResT, Rank, NewAlloc> mapped(UnaryOp op) const;

    template<typename BinaryOp>
    void visit(BinaryOp op) const;
    
    template<typename BinaryOp>
    void visit(BinaryOp op);

    /** 
    Round to floor, ceil, trunc toward zero, and absolute value.
    ``ResT`` can be floating-point or integral type.

    By default, if ``value_t`` is integer or pointer, ``ResT`` is 
    ``value_t`` itself, no conversion bappens. Otherwise ``ResT`` is ``int``, 
    and the conversion is made by std::floor, ceil, trunc and then cast.
    */
    template<typename ResT = int_value_t, 
        typename NewAlloc = std::allocator<ResT> >
    DArray<ResT, Rank, NewAlloc> floor() const;

    template<typename ResT = int_value_t, 
        typename NewAlloc = std::allocator<ResT> >
    DArray<ResT, Rank, NewAlloc> ceil() const;

    template<typename ResT = int_value_t, 
        typename NewAlloc = std::allocator<ResT> >
    DArray<ResT, Rank, NewAlloc> trunc() const;

    DArray abs() const;
protected:
    shape_t _shape;
    size_t _size;
    value_t *_data;

    /**
    _nullify(): set attributes to an "empty" state.
    _alloc(): _data = buffer allocated for n value_t objects.
    _dealloc(): deallocate buffer pointned by _data. _data itself is untouched.
    _not_null(), _is_null() identify whether the instance is not/is in the 
        "empty" state.
    */
    void _nullify() noexcept;
    void _alloc(size_t n);
    void _dealloc() noexcept;
    bool _not_null() const noexcept { return _data; }
    bool _is_null() const noexcept { return ! _not_null(); }

    template<typename ...SizeTs>
    size_t _indices_to_pos(size_t prev, size_t id0, 
        SizeTs ...ids) const noexcept;
    
    /* Copy b, e to _data, without any checking. */
    template<typename It>
    void _write_data(It b, It e) noexcept;

    void _resize_impl(const shape_t &new_shape, size_t new_size, 
        const value_t &value);
    
    /**
    Consistency check.
    _chk_non_empty(): if empty(), throw ErrLogic(eLENGTH).
    _chk_size_match(): if s1 != s2, throw ErrLogic(eLENGTH).
    */
    template<typename ...Args>
    void _chk_non_empty(Args &&...args) const;
    template<typename ...Args>
    static void _chk_size_match(size_t s1, size_t s2, Args &&...args);
};

_HIPP_TEMPNORET
DArray() noexcept 
: _shape(0), _size(0), _data(nullptr)
{ }
    
_HIPP_TEMPNORET
DArray(const shape_t &shape) 
: DArray(shape, size_hint_t(shape.prod()))
{ }

_HIPP_TEMPNORET
DArray(const shape_t &shape, const value_t&value) 
: DArray(shape, size_hint_t(shape.prod()), value)
{ }

_HIPP_TEMPHD
template<typename It>
_HIPP_TEMPCLS::DArray(const shape_t &shape, It b, It e) 
: DArray(shape, size_hint_t(shape.prod()), b, e)
{ }

_HIPP_TEMPHD
template<typename InputValue>
_HIPP_TEMPCLS::DArray(const shape_t &shape, 
    std::initializer_list<InputValue> il) 
: DArray(shape, il.begin(), il.end())
{ }

_HIPP_TEMPHD
template<typename InputValue>
_HIPP_TEMPCLS::DArray(const DArray<InputValue, Rank> &a) 
: _shape(a._shape), _size(a._size), _data(nullptr)
{
    if( a._data ) {
        _alloc(_size);
        _write_data(a.cbegin(), a.cend());
    }
}

_HIPP_TEMPNORET
DArray(const shape_t &shape, const size_hint_t &size_hint) 
: _shape(shape), _size(size_hint.size)
{
    _alloc(_size);
}

_HIPP_TEMPNORET
DArray(const shape_t &shape, const size_hint_t &size_hint, 
    const value_t &value)
: DArray(shape, size_hint)
{
    this->operator=(value);
}

_HIPP_TEMPHD
template<typename It>
_HIPP_TEMPCLS::DArray(const shape_t &shape, const size_hint_t &size_hint, 
    It b, It e) 
: DArray(shape, size_hint)
{
    _write_data(b, e);
}

_HIPP_TEMPHD
template<typename InputValue>
_HIPP_TEMPCLS::DArray(const shape_t &shape, const size_hint_t &size_hint,
    std::initializer_list<InputValue> il) 
: DArray(shape, size_hint, il.begin(), il.end())    
{ }

_HIPP_TEMPNORET
DArray(const DArray &a)
: _shape(a._shape), _size(a._size), _data(nullptr)
{
    if( a._data ) {
        _alloc(_size);
        _write_data(a.cbegin(), a.cend());
    }
}

_HIPP_TEMPNORET
DArray(DArray &&a)  noexcept 
: _shape(a._shape), _size(a._size), _data(a._data)
{
    a._nullify();
}

_HIPP_TEMPRET
operator=(const DArray &a) -> DArray & {
    DArray _temp_a(a);
    return this->operator=( std::move(_temp_a) );
}

_HIPP_TEMPRET
operator=(DArray &&a)  noexcept -> DArray & {
    reset(a._data, a._shape, size_hint_t(a._size));
    a._nullify();
    return *this;
}

_HIPP_TEMPNORET
~DArray() noexcept {
    _dealloc();
}

_HIPP_TEMPRET
operator=(const value_t &value) noexcept -> DArray & {
    std::fill_n(_data, _size, value);
    return *this;
}

_HIPP_TEMPHD
void swap(_HIPP_TEMPCLS &lhs, _HIPP_TEMPCLS &rhs) noexcept {
    using std::swap;
    swap(lhs._shape, rhs._shape);
    swap(lhs._size, rhs._size);
    swap(lhs._data, rhs._data);
}

_HIPP_TEMPRET release() noexcept -> value_t * {
    auto ptr = _data;
    _nullify();
    return ptr;
}

_HIPP_TEMPRET reset() noexcept -> void {
    _dealloc();
    _nullify();
}

_HIPP_TEMPRET reset(value_t *ptr, const shape_t &shape) noexcept -> void {
    reset(ptr, shape, size_hint_t(shape.prod()));
}

_HIPP_TEMPRET reset(value_t *ptr, const shape_t &shape, 
    const size_hint_t &size_hint) noexcept -> void 
{
    _dealloc();
    _shape = shape;
    _size = size_hint.size;
    _data = ptr;
}

_HIPP_TEMPHD
ostream & operator<< (ostream &os, const _HIPP_TEMPCLS &a) {
    PStream ps(os);
    ps << "DArray{", ps(a.begin(), a.end()), "}";
    return os;
}

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl) const -> ostream &  {
    PStream ps(os);
    if(fmt_cntl == 0) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(DArray), 
            "{", ps(begin(), end()), "}";
    }else{
        ps << HIPPCNTL_CLASS_INFO(DArray), 
            "  |- size = ", _size, ", shape = ", _shape, "\n",
            "  |- values = {", ps(begin(), end()), "}\n"; 
    }
    return os;
}

_HIPP_TEMPRET data() const noexcept -> const value_t * {
    return _data;
}

_HIPP_TEMPRET data() noexcept -> value_t * {
    return _data;
}

_HIPP_TEMPRET size() const noexcept -> size_t  {
    return _size;
}

_HIPP_TEMPRET shape() const noexcept -> const shape_t &  {
    return _shape;
}

_HIPP_TEMPRET empty() const noexcept -> bool  {
    return _size == 0;
}

_HIPP_TEMPRET operator[](size_t pos) noexcept -> ref_t  {
    return _data[pos];
}

_HIPP_TEMPRET operator[](size_t pos) const noexcept -> cref_t  {
    return _data[pos];
}

_HIPP_TEMPRET at(size_t pos) -> ref_t  {
    if( pos >= _size )
        ErrLogic::throw_(ErrLogic::eOUTOFRANGE, emFLPFB, "   ... pos ", 
        pos, " >= size ", _size, '\n');    
    return (*this)[pos];
}

_HIPP_TEMPRET at(size_t pos) const -> cref_t  {
    if( pos >= _size )
        ErrLogic::throw_(ErrLogic::eOUTOFRANGE, emFLPFB, "   ... pos ", 
        pos, " >= size ", _size, '\n');    
    return (*this)[pos];
}

_HIPP_TEMPRET begin() noexcept -> iter_t  {
    return _data;
}

_HIPP_TEMPRET begin() const noexcept -> citer_t  {
    return _data;
}

_HIPP_TEMPRET cbegin() const noexcept -> citer_t  {
    return _data;
}

_HIPP_TEMPRET end() noexcept -> iter_t  {
    return _data + _size;
}

_HIPP_TEMPRET end() const noexcept -> citer_t  {
    return _data + _size;
}

_HIPP_TEMPRET cend() const noexcept -> citer_t  {
    return _data + _size;
}

_HIPP_TEMPHD
template<typename ...SizeTs>
auto _HIPP_TEMPCLS::operator()(SizeTs &&...ids) noexcept -> ref_t {
    static_assert( sizeof...(ids) == RANK, "Number of indices invalid" );
    auto pos = _indices_to_pos(0, std::forward<SizeTs>(ids)... );
    return (*this)[pos];
}

_HIPP_TEMPHD
template<typename ...SizeTs>
auto _HIPP_TEMPCLS:: operator()(SizeTs &&...ids) const noexcept -> cref_t {
    static_assert( sizeof...(ids) == RANK, "Number of indices invalid" );
    auto pos = _indices_to_pos(0, std::forward<SizeTs>(ids)... );
    return (*this)[pos];
}

_HIPP_TEMPRET reshape(const shape_t &new_shape) -> void {
    size_t new_size = new_shape.prod();
    _chk_size_match(_size, new_size, emFLPFB);
    _shape = new_shape;
    _size = new_size;
}

_HIPP_TEMPHD
template<size_t NewRank, typename NewAlloc>
DArray<ValueT, NewRank, NewAlloc> _HIPP_TEMPCLS::reshaped(
    const SVec<size_t, NewRank> &new_shape) const 
{
    size_t new_size = new_shape.prod();
    _chk_size_match(_size, new_size, emFLPFB);

    if( _data ) {
        return DArray<ValueT, NewRank, NewAlloc>(
            new_shape, size_hint_t(new_size), cbegin(), cend());
    }else{
        return DArray<ValueT, NewRank, NewAlloc>();
    }
}

_HIPP_TEMPRET resize(size_t new_size, const value_t &value) 
-> void {
    if( _is_null() ) {
        shape_t sp( size_t(1) );
        sp[0] = new_size;
        _resize_impl(sp, new_size, value); return;
    }

    if( new_size == size() ) return;
    
    shape_t sp = shape();
    if( new_size == 0 ) {
        sp[0] = 0;
        _resize_impl(sp, 0, value); return;
    }

    size_t rem_sz = sp.cview(sp.s_stride, sp.s_tail(RANK-1)).prod();
    if( rem_sz == 0)
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... shape ", sp, 
        " cannot be resized to ", new_size, '\n');
    size_t ext0 = new_size / rem_sz;
    _chk_size_match(ext0*rem_sz, new_size, emFLPFB);
    sp[0] = ext0;
    _resize_impl(sp, new_size, value);
}
    
_HIPP_TEMPRET resize(const shape_t &new_shape, const value_t &value) 
-> void {
    _resize_impl( new_shape, new_shape.prod(), value );
}


_HIPP_TEMPHD
template<typename T, typename NewAlloc>
vector<T, NewAlloc> _HIPP_TEMPCLS::to_vector() const {
    static_assert( Rank == 1, "Rank must be 1 to be converted to a vector" );
    return vector<T, NewAlloc>(cbegin(), cend());
}

#define _HIPP_UNARY_OP_DEF(op) \
    _HIPP_TEMPRET \
    operator op(const value_t &rhs) noexcept -> DArray & { \
        for (size_t i = 0; i < _size; i++) _data[i] op rhs; \
        return *this; \
    } \
    _HIPP_TEMPRET \
    operator op(const DArray &rhs) -> DArray & { \
        _chk_size_match(_size, rhs._size, emFLPFB); \
        for(size_t i=0; i<_size; ++i) _data[i] op rhs._data[i]; \
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
    operator op() const -> DArray { \
        DArray ret(_shape); \
        for(size_t i=0; i<_size; ++i) ret[i] = op _data[i]; \
        return _HIPP_TEMPCLS{std::move(ret)}; \
    }

_HIPP_UNARY_OP_DEF(+)
_HIPP_UNARY_OP_DEF(-)
_HIPP_UNARY_OP_DEF(~)

#undef _HIPP_UNARY_OP_DEF

#define _HIPP_BIN_OP_DEF(op) \
    _HIPP_TEMPHD \
    _HIPP_TEMPCLS operator op(const typename _HIPP_TEMPCLS::value_t &lhs,  \
        const _HIPP_TEMPCLS &rhs) \
    { \
        _HIPP_TEMPCLS ret(rhs.shape(), \
            typename _HIPP_TEMPCLS::size_hint_t(rhs.size())); \
        const size_t n = rhs.size(); \
        for(size_t i=0; i<n; ++i) ret[i] = lhs op rhs[i]; \
        return _HIPP_TEMPCLS{std::move(ret)}; \
    } \
    _HIPP_TEMPHD \
    _HIPP_TEMPCLS operator op(const _HIPP_TEMPCLS &lhs,  \
        const typename _HIPP_TEMPCLS::value_t &rhs) \
    { \
        _HIPP_TEMPCLS ret(lhs.shape(), \
            typename _HIPP_TEMPCLS::size_hint_t(lhs.size())); \
        const size_t n = lhs.size(); \
        for(size_t i=0; i<n; ++i) ret[i] = lhs[i] op rhs; \
        return _HIPP_TEMPCLS{std::move(ret)}; \
    } \
    _HIPP_TEMPHD \
    _HIPP_TEMPCLS operator op(const _HIPP_TEMPCLS &lhs,  \
        const _HIPP_TEMPCLS &rhs) \
    { \
        if( lhs.size() != rhs.size() ) \
            ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, \
                "  ... Sizes do not match (got ", lhs.size(), \
                " and ", rhs.size(), ")\n"); \
        _HIPP_TEMPCLS ret(lhs.shape(), \
            typename _HIPP_TEMPCLS::size_hint_t(lhs.size())); \
        const size_t n = lhs.size(); \
        for(size_t i=0; i<n; ++i) ret[i] = lhs[i] op rhs[i]; \
        return _HIPP_TEMPCLS{std::move(ret)}; \
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
    _HIPP_TEMPHD_B \
    _HIPP_TEMPCLS_B operator op(const typename _HIPP_TEMPCLS::value_t &lhs,  \
        const _HIPP_TEMPCLS &rhs) \
    { \
        _HIPP_TEMPCLS_B ret(rhs.shape(), \
            typename _HIPP_TEMPCLS::size_hint_t(rhs.size())); \
        const size_t n = rhs.size(); \
        for(size_t i=0; i<n; ++i) ret[i] = lhs op rhs[i]; \
        return _HIPP_TEMPCLS_B{std::move(ret)}; \
    } \
    _HIPP_TEMPHD_B \
    _HIPP_TEMPCLS_B operator op(const _HIPP_TEMPCLS &lhs,  \
        const typename _HIPP_TEMPCLS::value_t &rhs) \
    { \
        _HIPP_TEMPCLS_B ret(lhs.shape(), \
            typename _HIPP_TEMPCLS::size_hint_t(lhs.size())); \
        const size_t n = lhs.size(); \
        for(size_t i=0; i<n; ++i) ret[i] = lhs[i] op rhs; \
        return _HIPP_TEMPCLS_B{std::move(ret)}; \
    } \
    _HIPP_TEMPHD_B \
    _HIPP_TEMPCLS_B operator op(const _HIPP_TEMPCLS &lhs,  \
        const _HIPP_TEMPCLS &rhs) \
    { \
        if( lhs.size() != rhs.size() ) \
            ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, \
                "  ... Sizes do not match (got ", lhs.size(), \
                " and ", rhs.size(), ")\n"); \
        _HIPP_TEMPCLS_B ret(lhs.shape(), \
            typename _HIPP_TEMPCLS::size_hint_t(lhs.size())); \
        const size_t n = lhs.size(); \
        for(size_t i=0; i<n; ++i) ret[i] = lhs[i] op rhs[i]; \
        return _HIPP_TEMPCLS_B{std::move(ret)}; \
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
    for(size_t i=0; i<_size; ++i){
        ret += std::pow(_data[i], p);
    }
    return std::pow(ret, 1.0/static_cast<double>(p));
}

_HIPP_TEMPHD
template<typename ResT> 
ResT _HIPP_TEMPCLS::squared_norm() const noexcept {
    ResT ret {0};
    for(size_t i=0; i<_size; ++i){
        auto x = static_cast<ResT>(_data[i]);
        ret += x*x;
    }
    return ret;
}

_HIPP_TEMPRET normalize() noexcept -> DArray & {
    auto scale = 1.0 / norm();
    return this->operator*=( static_cast<value_t>(scale) );
}

_HIPP_TEMPRET normalize(int p) noexcept -> DArray & {
    auto scale = 1.0 / norm(p);
    return this->operator*=( static_cast<value_t>(scale) );
}

_HIPP_TEMPRET normalized() const -> DArray {
    auto scale = 1.0 / norm();
    return (*this) * static_cast<value_t>(scale);
}

_HIPP_TEMPRET normalized(int p) const -> DArray {
    auto scale = 1.0 / norm(p);
    return (*this) * static_cast<value_t>(scale);
}


_HIPP_TEMPHD
template<typename ResT>
ResT _HIPP_TEMPCLS::sum() const noexcept {
    ResT ret {0};
    for(size_t i=0; i<_size; ++i) ret += _data[i];
    return ret;
}

_HIPP_TEMPHD
template<typename ResT>
ResT _HIPP_TEMPCLS::prod() const noexcept {
    ResT ret {1};
    for(size_t i=0; i<_size; ++i) ret *= _data[i];
    return ret;
}

_HIPP_TEMPHD
template<typename ResT>
ResT _HIPP_TEMPCLS::mean() const noexcept {
    _chk_non_empty(emFLPFB);
    return sum<ResT>() / static_cast<ResT>(_size);
}

_HIPP_TEMPRET min() const noexcept -> value_t {
    _chk_non_empty(emFLPFB);
    value_t ret { std::numeric_limits<value_t>::max() };
    for(size_t i=0; i<_size; ++i){
        if( _data[i] < ret ) ret = _data[i];
    }
    return ret;
}

_HIPP_TEMPRET max() const noexcept -> value_t {
    _chk_non_empty(emFLPFB);
    value_t ret { std::numeric_limits<value_t>::lowest() };
    for(size_t i=0; i<_size; ++i){
        if( _data[i] > ret ) ret = _data[i];
    }
    return ret;
}

_HIPP_TEMPRET minmax() const noexcept -> std::pair<value_t, value_t> {
    _chk_non_empty(emFLPFB);
    value_t ret_min = { std::numeric_limits<value_t>::max() },
        ret_max = { std::numeric_limits<value_t>::lowest() };
    for(size_t i=0; i<_size; ++i){
        value_t e = _data[i];
        if( e < ret_min ) ret_min = e;
        if( e > ret_max ) ret_max = e;
    }
    return {ret_min, ret_max};
}

_HIPP_TEMPRET min_index() const noexcept -> size_t {
    _chk_non_empty(emFLPFB);
    auto it = std::min_element(_data, _data+_size);
    return static_cast<size_t>(it - _data);
}

_HIPP_TEMPRET max_index() const noexcept -> size_t {
    _chk_non_empty(emFLPFB);
    auto it = std::max_element(_data, _data+_size);
    return static_cast<size_t>(it - _data);
}

_HIPP_TEMPRET minmax_index() const noexcept -> std::pair<size_t, size_t> {
    _chk_non_empty(emFLPFB);
    auto [it_min, it_max] = std::minmax_element(_data, _data+_size);
    return { static_cast<size_t>(it_min-_data), 
        static_cast<size_t>(it_max-_data) };
}

_HIPP_TEMPRET all() const noexcept -> bool {
    for(size_t i=0; i<_size; ++i){
        if( !_data[i] ) return false;
    }
    return true;
}

_HIPP_TEMPRET any() const noexcept -> bool {
    for(size_t i=0; i<_size; ++i){
        if( _data[i] ) return true;
    }
    return false;
}

_HIPP_TEMPHD
template<typename UnaryOp>
auto _HIPP_TEMPCLS::map(UnaryOp op) -> DArray & {
    for(size_t i=0; i<_size; ++i){
        _data[i] = op(_data[i]);
    }
    return *this;
}

_HIPP_TEMPHD
template<typename UnaryOp, typename ResT, typename NewAlloc>
auto _HIPP_TEMPCLS::mapped(UnaryOp op) const -> DArray<ResT, Rank, NewAlloc> {
    DArray<ResT, Rank, NewAlloc> res(_shape, size_hint_t(_size));
    for(size_t i=0; i<_size; ++i) res[i] = op(_data[i]);
    return {std::move(res)};
}

_HIPP_TEMPHD
template<typename BinaryOp>
void _HIPP_TEMPCLS::visit(BinaryOp op) const {
    for(size_t i=0; i<_size; ++i){
        op(i, _data[i]);
    }
}

_HIPP_TEMPHD
template<typename BinaryOp>
void _HIPP_TEMPCLS::visit(BinaryOp op) {
    for(size_t i=0; i<_size; ++i){
        op(i, _data[i]);
    }
}

_HIPP_TEMPHD
template<typename ResT, typename NewAlloc>
auto _HIPP_TEMPCLS::floor() const -> DArray<ResT, Rank, NewAlloc> {
    DArray<ResT, Rank, NewAlloc> res(_shape, size_hint_t(_size));
    if constexpr(IS_INT) {
        for(size_t i=0; i<_size; ++i) res[i] = static_cast<ResT>(_data[i]);
    }else {
        for(size_t i=0; i<_size; ++i) 
            res[i] = static_cast<ResT>(std::floor(_data[i]));
    }
    return res;
}

_HIPP_TEMPHD
template<typename ResT, typename NewAlloc>
auto _HIPP_TEMPCLS::ceil() const -> DArray<ResT, Rank, NewAlloc> {
    DArray<ResT, Rank, NewAlloc> res(_shape, size_hint_t(_size));
    if constexpr(IS_INT) {
        for(size_t i=0; i<_size; ++i) res[i] = static_cast<ResT>(_data[i]);
    }else {
        for(size_t i=0; i<_size; ++i) 
            res[i] = static_cast<ResT>(std::ceil(_data[i]));
    }
    return res;
}

_HIPP_TEMPHD
template<typename ResT, typename NewAlloc>
auto _HIPP_TEMPCLS::trunc() const -> DArray<ResT, Rank, NewAlloc> {
    DArray<ResT, Rank, NewAlloc> res(_shape, size_hint_t(_size));
    if constexpr(IS_INT) {
        for(size_t i=0; i<_size; ++i) res[i] = static_cast<ResT>(_data[i]);
    }else {
        for(size_t i=0; i<_size; ++i) 
            res[i] = static_cast<ResT>(std::trunc(_data[i]));
    }
    return res;   
}

_HIPP_TEMPRET
abs() const -> DArray {
    DArray res(_shape, size_hint_t(_size));
    for(size_t i=0; i<_size; ++i) res[i] = std::abs(_data[i]);
    return { std::move(res) };
}

_HIPP_TEMPRET _nullify() noexcept -> void { 
    _shape = size_t(0);
    _size = 0; 
    _data = nullptr; 
}

_HIPP_TEMPRET _alloc(size_t n) -> void {
    _data = alloc_t().allocate(n);
}

_HIPP_TEMPRET _dealloc() noexcept -> void {
    if ( _data ) alloc_t().deallocate(_data, _size);
}

_HIPP_TEMPHD
template<typename ...SizeTs>
size_t _HIPP_TEMPCLS::_indices_to_pos(size_t prev, 
    size_t id0, SizeTs ...ids) const noexcept 
{
    constexpr size_t rem_rank = sizeof...(SizeTs);
    if constexpr ( rem_rank == 0 ) {
        return prev + id0;
    }else {
        constexpr size_t next_dim = RANK - rem_rank;
        return _indices_to_pos( (prev + id0)*_shape[next_dim], ids...); 
    }
}

_HIPP_TEMPHD
template<typename It>
void _HIPP_TEMPCLS::_write_data(It b, It e) noexcept {
    std::copy(b, e, _data);
}

_HIPP_TEMPRET _resize_impl(const shape_t &new_shape, size_t new_size, 
    const value_t &value) -> void
{
    if( _is_null() ) {
        (*this) = DArray(new_shape, size_hint_t(new_size), value);
        return;
    }
    const size_t old_size = size();
    if( new_size == old_size ) {
        _shape = new_shape; return;
    }

    DArray new_a(new_shape, size_hint_t(new_size));
    if( new_size <= old_size ) {
        new_a._write_data( cbegin(), cbegin()+new_size );
    }else{
        new_a._write_data( cbegin(), cend() );
        std::fill_n(new_a.begin() + old_size, new_size-old_size, value);
    }
    (*this) = std::move(new_a);
}

_HIPP_TEMPHD
template<typename ...Args>
void _HIPP_TEMPCLS::_chk_non_empty(Args &&...args) const {
    if( empty() ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, std::forward<Args>(args)..., 
            "  ... Cannot work with an empty array\n");
}

_HIPP_TEMPHD
template<typename ...Args>
void _HIPP_TEMPCLS::_chk_size_match(size_t s1, size_t s2, Args &&...args) {
    if( s1 != s2 ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, std::forward<Args>(args)..., 
            "  ... Sizes do not match (got ", s1, " and ", s2, ")\n");
}


} // namespace HIPP::NUMERICAL

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS

#undef _HIPP_TEMPHD_B
#undef _HIPP_TEMPCLS_B

#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#endif	//_HIPPNUMERICAL_LINALG_DARRAYND_H_


