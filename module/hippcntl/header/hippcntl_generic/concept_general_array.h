/**
create: Yangyao CHEN, 2021/10/11
    [write   ] GeneralArrayTraits - compile-time and run-time feature detection
        for general array types.
*/
#ifndef _HIPPCNTL_CONCEPT_GENERAL_ARRAY_H_
#define _HIPPCNTL_CONCEPT_GENERAL_ARRAY_H_
#include "generic_base.h"
#include "concept_raw_array.h"
#include "concept_dynamic_array.h"
namespace HIPP {

/**
GeneralArray Protocol.

Given type ``T``, if it is not GeneralArray-compliant, 
- Compile-time attribute ``bool is_array = false``.
- Member type ``value_t`` is aliased to ``T``.

Otherwise, it is a GeneralArray type, satisfying
- ``GeneralArrayTraits<T>`` can be constructed by passing a reference to the
  ``T`` instance.
- Compile-time attribute ``bool is_array = true``.
- Compile-time attribute ``bool is_const`` tells whether or not the element 
  is a const-qualified type.
- Member type ``array_t`` is aliased to a binary-compatible type as ``T``, i.e.,
  for ``RawArray``, it is the corrpesponding raw array type, and for 
  ``DynamicArray``, it is ``T`` itself.
- Member type ``value_t`` is aliased to the type of array element.
- Method ``buff()`` returns the pointer to the first array element.
- Method ``rank()``, ``size()``, ``extents()``, and ``strides`` returns the 
  details of the array - its number of dimensions, number of elements in total,
  number of elements at each dimension, and the stride to the next element at 
  each dimension. 

The traits object is copyable and movable and the results refer to the same 
array.

A special case is ``T = ValueT *`` or ``T = const ValueT *`` for any ``ValueT``,
in this case the traits refers to a raw buffer.

Predefined GeneralArray types include any RawArray type and any DynamicArray
type.

Examples
--------
// Numerical scalars are not considered as GeneralArray.
// printed: int: is_array = 0
int scalar {};
pout << "int: is_array = ", 
    GeneralArrayTraits<decltype(scalar)>::is_array, "\n";

// std::vector is DynamicArray, and therefore a General Array.
// Detailed features are accessible from the traits instance.
// printed:
// vector<int>(4):
//   is_array = 1, is_const = 0
//   buff = 0x557a068552c0, size = 4
//   extents = 4, strides = 1
vector<int> a1(4);
GeneralArrayTraits tr1{a1};
pout << "vector<int>(4):\n",
    "  is_array = ",  tr1.is_array,   ", is_const = ",    tr1.is_const, "\n",
    "  buff = ",      tr1.buff(),     ", size = ",        tr1.size(), "\n",
    "  extents = ",   tr1.extents(),  ", strides = ",    tr1.strides(), "\n";

// Raw array and std::array are RawArray types - also GeneralArray.
// printed: 
// array<int,3>: buff = 0x7fffddb798bc, size = 3
// float[2]: buff = 0x7fffddb798b4, size = 2
array<int, 3> a2 {};
float a3[2];
GeneralArrayTraits tr2{a2};
GeneralArrayTraits tr3{a3};
pout << "array<int,3>: buff = ", tr2.buff(), ", size = ", tr2.size(), "\n";
pout << "float[2]: buff = ", tr3.buff(), ", size = ", tr3.size(), "\n";

// Multi-dimensional arrays are also GeneralArray.
// printed:
// double[2][3]: extents = 2,3, strides = 3,1
// const array<array<int,3>,2>: is_const = 1, extents = 2,3
double a4[2][3] {};
const array<array<int, 3>, 2> a5{};
GeneralArrayTraits tr4{a4};
GeneralArrayTraits tr5{a5};
pout << "double[2][3]: extents = ", tr4.extents(), 
    ", strides = ", tr4.strides(), "\n";
pout << "const array<array<int,3>,2>: is_const = ", tr5.is_const, 
    ", extents = ", tr5.extents(), "\n";
*/
template<typename T, typename V=void>
class GeneralArrayTraits {
public:
    inline static constexpr bool is_array = false;
    typedef T value_t;
};

/**
Specializations for pointer and const pointer.
*/
template<typename ValueT>
class GeneralArrayTraits< const ValueT * > {
public:
    inline static constexpr bool is_array = true,
        is_const = true;

    typedef const ValueT *array_t;
    typedef const ValueT value_t;
    typedef value_t cvalue_t;

    constexpr GeneralArrayTraits(value_t *p, size_t n) noexcept 
        : _buff(p), _size(n) {}
    constexpr GeneralArrayTraits() noexcept 
        : GeneralArrayTraits(nullptr, 0) {}

    constexpr value_t *buff() noexcept { return _buff; }
    constexpr size_t rank() noexcept { return 1; }
    constexpr size_t size() noexcept { return _size; }
    vector<size_t> extents() const { return vector<size_t>{ _size }; }
    vector<size_t> strides() const { return vector<size_t>{ 1 }; }
private:
    value_t *_buff;
    size_t _size;
};

template<typename ValueT>
class GeneralArrayTraits< ValueT * > {
public:
    inline static constexpr bool is_array = true,
        is_const = false;

    typedef ValueT *array_t;
    typedef ValueT value_t;
    typedef std::add_const_t<value_t> cvalue_t;

    constexpr GeneralArrayTraits(value_t *p, size_t n) noexcept 
        : _buff(p), _size(n) {}
    constexpr GeneralArrayTraits() noexcept 
        : GeneralArrayTraits(nullptr, 0) {}

    constexpr value_t *buff() noexcept { return _buff; }
    constexpr size_t rank() noexcept { return 1; }
    constexpr size_t size() noexcept { return _size; }
    vector<size_t> extents() const { return vector<size_t>{ _size }; }
    vector<size_t> strides() const { return vector<size_t>{ 1 }; }
private:
    value_t *_buff;
    size_t _size;
};

template<typename ValueT>
GeneralArrayTraits(ValueT *, size_t n) -> GeneralArrayTraits<ValueT *>;

/**
Specializations for RawArray type.
*/
template<typename T>
class GeneralArrayTraits<T, 
    std::enable_if_t< RawArrayTraits<T>::is_array > > 
{
public:
    typedef RawArrayTraits<T> _traits_t;

    inline static constexpr bool is_array = true;

    typedef typename _traits_t::array_t array_t;
    typedef typename _traits_t::value_t value_t;
    typedef std::add_const_t<value_t> cvalue_t;

    inline static constexpr bool is_const = 
        GeneralArrayTraits<value_t *>::is_const;

    GeneralArrayTraits(T &x) : _x(x) {}
    
    value_t *buff() const noexcept { return reinterpret_cast<value_t *>(&_x); }
    constexpr size_t rank() noexcept { return _traits_t::rank; }
    constexpr size_t size() noexcept { return _traits_t::size; }
    vector<size_t> extents() const {
        return vector<size_t>( _traits_t::extents.begin(), 
            _traits_t::extents.end() );
    }
    vector<size_t> strides() const {
        return vector<size_t>( _traits_t::strides.begin(), 
            _traits_t::strides.end() );
    }
private:
    T &_x;
};

template<typename T, 
    std::enable_if_t< RawArrayTraits<T>::is_array, int> = 0 >
GeneralArrayTraits(T &) -> GeneralArrayTraits<T>;

/**
Specializations for DynamicArray type.
*/
template<typename T>
class GeneralArrayTraits<T, 
    std::enable_if_t< DynamicArrayTraits<T>::is_array > >
{
public:
    typedef DynamicArrayTraits<T> _traits_t;
    
    inline static constexpr bool is_array = true;
    

    typedef typename _traits_t::array_t array_t;
    typedef typename _traits_t::value_t value_t;
    typedef std::add_const_t<value_t> cvalue_t;

    inline static constexpr bool is_const = 
        GeneralArrayTraits<value_t *>::is_const;

    GeneralArrayTraits(T &x) : _x_traits(x) {}
    
    value_t *buff() const noexcept { return _x_traits.buff(); }
    constexpr size_t rank() noexcept { return _traits_t::rank; }
    size_t size() const { return _x_traits.size(); }
    vector<size_t> extents() const {
        auto ext = _x_traits.extents();
        return vector<size_t>( ext.begin(), ext.end() );
    }
    vector<size_t> strides() const {
        auto sd = _x_traits.strides();
        return vector<size_t>( sd.begin(), sd.end() );
    }
private:
    _traits_t _x_traits;
};

template<typename T, 
    std::enable_if_t< DynamicArrayTraits<T>::is_array, int> =0 >
GeneralArrayTraits(T &) -> GeneralArrayTraits<T>;

    
} // namespace HIPP
#endif	//_HIPPCNTL_CONCEPT_GENERAL_ARRAY_H_