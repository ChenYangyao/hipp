/**
create: Yangyao CHEN, 2021/10/08
    [write   ] DynamicArrayTraits - compile-time traits and run-time info 
        detection for dynamic array types.
*/

#ifndef _HIPPCNTL_CONCEPT_DYNAMIC_ARRAY_H_
#define _HIPPCNTL_CONCEPT_DYNAMIC_ARRAY_H_
#include "generic_base.h"
namespace HIPP {

/**
DynamicArray protocol.

For any type ``T`` that does not satisfy this protocol, 
- member compile-time ``bool is_array = false``.
- member type ``value_t`` is aliased as ``T``.

Otherwise, the following conditions are required for the specialization of
``DynamicArrayTraits`` on ``T``:
- member compile-time ``bool is_array = true``.
- member compile-time ``size_t rank`` is set to the rank of the dynamic array.
- member type ``array_t`` is aliased as ``T``.
- member type ``value`` is aliased as the element type of the dynamic array.
- the traits can be constructed by passing ``T &``, with the public member 
  ``array_t &array`` refers to that passed instance.
- the call of methods ``buff()``, ``size()``, ``extents()``, ``strides()`` on 
  the traits instance return the buffer starting address (type ``value_t *``), 
  number of element in the array (``size_t``), extent of the array at any 
  dimension (``std::array<size_t, rank>``), and stride to the next element
  at any dimension (``std::array<size_t, rank>``).

Predefined DynamicArray-compliant type include std::vector and its const 
const-qualified version. Traits for the later always has const-qualified 
``value_t``.
*/

template<typename T, typename V=void>
class DynamicArrayTraits {
public:
    inline static constexpr bool is_array = false;
    typedef T value_t;
};

template<typename ValueT, typename Allocator>
class DynamicArrayTraits< vector<ValueT, Allocator> > {
public:
    inline static constexpr bool is_array = true;
    inline static constexpr size_t rank = 1;

    typedef vector<ValueT, Allocator> array_t;
    typedef ValueT value_t;

    DynamicArrayTraits(array_t &a) : array(a) {}

    value_t * buff() const noexcept { return array.data(); }
    size_t size() const noexcept { return array.size(); }
    std::array<size_t, rank> extents() const noexcept { return {size()}; }
    std::array<size_t, rank> strides() const noexcept { return {size_t(1)}; }

    array_t &array;
};

template<typename ValueT, typename Allocator>
class DynamicArrayTraits< const vector<ValueT, Allocator> > {
public:
    inline static constexpr bool is_array = true;
    inline static constexpr size_t rank = 1;

    typedef const vector<ValueT, Allocator> array_t;
    typedef std::add_const_t<ValueT> value_t;

    DynamicArrayTraits(array_t &a) : array(a) {}

    value_t * buff() const noexcept { return array.data(); }
    size_t size() const noexcept { return array.size(); }
    std::array<size_t, rank> extents() const noexcept { return {size()}; }
    std::array<size_t, rank> strides() const noexcept { return {size_t(1)}; }

    array_t &array;
};

template<typename ValueT, typename Allocator>
DynamicArrayTraits( vector<ValueT, Allocator> & ) 
-> DynamicArrayTraits< vector<ValueT, Allocator> >;

template<typename ValueT, typename Allocator>
DynamicArrayTraits( const vector<ValueT, Allocator> & ) 
-> DynamicArrayTraits< const vector<ValueT, Allocator> >;

} // namespace HIPP
#endif	//_HIPPCNTL_CONCEPT_DYNAMIC_ARRAY_H_


