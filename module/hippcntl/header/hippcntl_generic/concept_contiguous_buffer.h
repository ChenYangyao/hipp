/**
create: Yangyao CHEN, 2021/09/15
    [write   ] ContiguousBufferTraits - compile-time traits and run-time feature
        detection for contiguous-buffer protocol. 
*/

#ifndef _HIPPCNTL_CONCEPT_CONTIGUOUS_BUFFER_H_
#define _HIPPCNTL_CONCEPT_CONTIGUOUS_BUFFER_H_
#include "generic_base.h"
#include "concept_raw_array.h"
#include "concept_dynamic_array.h"
namespace HIPP {

/**
ContiguousBuffer protocol. 

For any type ``T`` that does not satisfy this protocol, member ``is_buffer`` is 
``false`` and ``buffer_t`` is ``T`` itself.

Otherwise, the following members must be defined for the specialization of
``ContiguousBufferTraits``:
- ``is_buffer``: ``true``.
- ``is_const``: whether or not the buffer is constant.
- ``value_t`` and ``cvalue_t``:  type of the element in the buffer, and its 
  const counterpart. If ``is_const``, ``cvalue_t`` equals to ``value_t``.
- ``buffer_t``: the type of the object that hosts the buffer.

Template argument V is not significant but reserved for type matching.

The traits type must be default-constructable, resulting in a ``size == 0`` 
buffer.

The traits object can be constructed by passing a reference to the 
buffer-host object.

ContiguousBufferTraits is copy-constructable and copy-assignable, resulting
in object that refers to the same buffer.

The traits object has methods ``get_buff()``, ``get_cbuff()`` and ``get_size()``
which returns the buffer pointer, const buffer pointer and size (i.e., number 
of elements) of the buffer, respectively. Structured binding 
``cv-ref auto [buff, size]`` is allowed.

``begin()``, ``end()``, ``cbegin()``, ``cend()`` are defined to iterate over
the referred buffer. They all returns raw pointer typed ``value_t *`` or 
``cvalue_t``.

A special case is ``T = ValueT *`` or ``T = const ValueT *``, i.e., the 
buffer-host type is a pointer. In this case, the specialization class is used.

Predefined contiguous-buffer-compliant types include any RawArray-compliant 
type (i.e., C-style raw array, std::array) and DynamicArray-compliant 
type (e.g., std::vector).
*/
template<typename T, typename V=void>
class ContiguousBufferTraits {
public:
    inline static constexpr bool is_buffer = false;
    typedef T buffer_t;
};

/**
Specialization for const raw pointer.
Template parameters:
@ValueT: value of the element in the buffer.

e.g., 
float a1[4];
const float a2[3];
ContiguousBufferTraits<const float *> cbt1 {a1, 4}, cbt21 {a2, 3};
ContiguousBufferTraits cbt22 {a2, 3};           // deduced <const float *>
*/
template<typename ValueT>
class ContiguousBufferTraits< const ValueT * > {
public:
    inline static constexpr bool is_buffer = true,
        is_const = true;
    typedef const ValueT *buffer_t;
    typedef const ValueT value_t;
    typedef value_t cvalue_t;

    /**
    Constructors
    @p: pointer to the first element in the buffer.
    @n: size of the buffer (i.e., number of elements).

    Default constructor: equivalent to ``p=nullptr; n=0``.
    */
    constexpr ContiguousBufferTraits(value_t *p, size_t n) noexcept
        : buff(p), size(n) {}
    constexpr ContiguousBufferTraits() noexcept
        : ContiguousBufferTraits(nullptr, 0) {}

    constexpr value_t * get_buff() const noexcept { return buff; }
    constexpr cvalue_t * get_cbuff() const noexcept { return buff; }
    constexpr size_t get_size() const noexcept { return size; }

    constexpr value_t * begin() const noexcept { return buff; }
    constexpr value_t * end() const noexcept { return buff + size; }
    constexpr cvalue_t * cbegin() const noexcept { return buff; }
    constexpr cvalue_t * cend() const noexcept { return buff + size; }

    friend ostream & operator<<(ostream &os, const ContiguousBufferTraits &cbt) 
    {
        PStream ps(os);
        auto [p, n] = cbt;
        ps << "ContiguousBufferTraits{buff=[const]", 
            (void *)p, ", size=", n, "}";
        return os;
    }

    value_t *buff;
    size_t size;
};

/**
Specialization for raw pointer.
Template parameters:
@ValueT: value of the element in the buffer.

e.g., 
float a1[4];
const float a2[3];
ContiguousBufferTraits<float *> cbt11 {a1, 4},    // OK
    cbt2{a2, 4};                                  // ERROR, compile failure
ContiguousBufferTraits cbt12 {a1, 4};             // deduced <float *>
*/
template<typename ValueT>
class ContiguousBufferTraits< ValueT * > {
public:
    inline static constexpr bool is_buffer = true,
        is_const = false;
    typedef ValueT *buffer_t;
    typedef ValueT value_t;
    typedef std::add_const_t<value_t> cvalue_t;
    
    /**
    Constructors
    @p: pointer to the first element in the buffer.
    @n: size of the buffer (i.e., number of elements).

    Default constructor: equivalent to ``p=nullptr; n=0``.
    */
    constexpr ContiguousBufferTraits(value_t *p, size_t n) noexcept
        : buff(p), size(n) {}
    constexpr ContiguousBufferTraits() noexcept
        : ContiguousBufferTraits(nullptr, 0) {}

    constexpr value_t * get_buff() const noexcept { return buff; }
    constexpr cvalue_t * get_cbuff() const noexcept { return buff; }
    constexpr size_t get_size() const noexcept { return size; }

    constexpr value_t * begin() const noexcept { return buff; }
    constexpr value_t * end() const noexcept { return buff + size; }
    constexpr cvalue_t * cbegin() const noexcept { return buff; }
    constexpr cvalue_t * cend() const noexcept { return buff + size; }

    friend ostream & operator<<(ostream &os, const ContiguousBufferTraits &cbt) 
    {
        PStream ps(os);
        auto [p, n] = cbt;
        ps << "ContiguousBufferTraits{buff=", (void *)p, ", size=", n, "}";
        return os;
    }

    value_t *buff;
    size_t size;
};

template<typename ValueT>
ContiguousBufferTraits(ValueT *, size_t n) -> ContiguousBufferTraits<ValueT *>;

/** 
Specialization for RawArray protocol objects 
(include C-style raw array, std::array, etc. ). In these case, they are 
treated as row-major storage 1D buffer.

e.g.,
std::array<std::array<double, 3>, 2> arr1 {};
const std::array<double, 3> arr2 {};

ContiguousBufferTraits<decltype(arr1)> cbt1{arr1};  
                                        // explicit set the template argument
ContiguousBufferTraits cbr2{arr2};      // auto deduced
*/
template<typename RawArrayT>
struct ContiguousBufferTraits< 
    RawArrayT, 
    std::enable_if_t<RawArrayTraits<RawArrayT>::is_array> >
: ContiguousBufferTraits< typename RawArrayTraits<RawArrayT>::value_t * >
{
    typedef RawArrayTraits<RawArrayT> _traits_t;
    typedef ContiguousBufferTraits< typename _traits_t::value_t * > _parent_t;
    typedef RawArrayT buffer_t;
    using typename _parent_t::value_t;

    constexpr ContiguousBufferTraits( buffer_t &arr ) noexcept
        : _parent_t( reinterpret_cast<value_t *>(&arr), _traits_t::size ) {}
};


template<typename RawArrayT, 
    std::enable_if_t<RawArrayTraits<RawArrayT>::is_array, int> = 0>
ContiguousBufferTraits(RawArrayT &) -> ContiguousBufferTraits<RawArrayT>;

/** 
Specialization for DynamicArray protocol objects (e.g., std::vector). 
Any of these object is treated as row-major 1D buffer.

e.g.,
const vector<int> a1{1,2,3};
vector<int> a2{1,2,3};

ContiguousBufferTraits<vector<int> > cbt1 {a1}; 
                                        // explicit set the template argument
ContiguousBufferTraits cbt2 {a2};       // auto deduced
*/
template<typename DynArrayT>
struct ContiguousBufferTraits< 
    DynArrayT,
    std::enable_if_t< DynamicArrayTraits<DynArrayT>::is_array >
> : ContiguousBufferTraits< typename DynamicArrayTraits<DynArrayT>::value_t * >
{
    typedef DynamicArrayTraits<DynArrayT> _traits_t;
    typedef ContiguousBufferTraits< typename _traits_t::value_t * > _parent_t;
    typedef DynArrayT buffer_t;
    typedef typename _parent_t::value_t value_t;

    ContiguousBufferTraits( buffer_t &arr )
        : _parent_t( _traits_t(arr).buff(), _traits_t(arr).size() ) {}
};
template<typename DynArrayT, 
    std::enable_if_t<DynamicArrayTraits<DynArrayT>::is_array, int> = 0>
ContiguousBufferTraits(DynArrayT &) -> ContiguousBufferTraits<DynArrayT>;
  

/**
ContiguousBuffer object refers a buffer of element type ``ValueT``.
Structured binding ``cv-ref auto [buff, size]`` is allowed.

ContiguousBuffer is copy-constructable and copy-assignable, resulting in 
object referring to the same buffer.
*/
template<typename ValueT>
class ContiguousBuffer {
public:
    template<typename T> using traits_t = ContiguousBufferTraits<T>;

    /* Value type of the buffer element, and its const counterpart. */
    typedef ValueT value_t;                     
    typedef std::add_const_t<value_t> cvalue_t;

    /**
    Constructors
    (1) Default constructor - an empty buffer as initialized with (2) by passing 
        ``p=nullptr, n=0``.
    (2) by passing a pointer ``p`` to the buffer starting address and a size 
        ``n`` indicating the number of elements.
    (3) by any other ContiguousBuffer protocol compliant object typed ``T``, 
        i.e., ``ContiguousBufferTraits<T>::is_buffer == true``. ``T`` can also
        be a ContiguousBuffer with proper ``value_t``, e.g., copy-construct
        a ContiguousBuffer<const int> from ContiguousBuffer<int> is allowed.
    */
    constexpr ContiguousBuffer() : ContiguousBuffer((value_t *)nullptr, 0) {}

    template<typename T>
    constexpr ContiguousBuffer(T *p, size_t n) noexcept { _set_buff(p, n); }
    
    template<typename T> 
    ContiguousBuffer( T &x ) {
        if constexpr( std::is_same_v<T, ContiguousBuffer<value_t> >
            || std::is_same_v<T, ContiguousBuffer<cvalue_t> > )
        {
            _set_buff(x.buff, x.size);
        }else {
            auto [p, n] = ContiguousBufferTraits<T>{x};
            _set_buff(p, n);
        }
    }

    template<typename T> 
    constexpr ContiguousBuffer & operator=(
        const ContiguousBuffer<T> &cb) noexcept 
    {
        _set_buff(cb.buff, cb.size);
        return *this;
    }

    /* Return the buffer starting pointer and the number of elements. */
    constexpr value_t * get_buff() const noexcept { return buff; }
    constexpr cvalue_t * get_cbuff() const noexcept { return buff; }
    constexpr size_t get_size() const noexcept { return size; }

    /* Return the beginning and ending pointers to the buffer. */
    constexpr value_t * begin() const noexcept { return buff; }
    constexpr value_t * end() const noexcept { return buff + size; }
    constexpr cvalue_t * cbegin() const noexcept { return buff; }
    constexpr cvalue_t * cend() const noexcept { return buff + size; }

    /* Print the detail of the buffer. */
    friend ostream & operator<<(ostream &os, const ContiguousBuffer &cb) 
    {
        PStream ps(os);
        auto [p, n] = cb;
        ps << "ContiguousBuffer{buff=", 
            traits_t<value_t *>::is_const?"[const]":"", (void *)p, 
            ", size=", n, "}";
        return os;
    }

    value_t *buff;
    size_t size;
protected:
    template<typename T>
    void _set_buff(T *p, size_t n) noexcept { buff = p; size = n; }
};

template<typename T>
ContiguousBuffer(T *p, size_t n) -> ContiguousBuffer<T>;

template<typename T>
ContiguousBuffer(T &x)  
-> ContiguousBuffer<typename ContiguousBufferTraits<T>::value_t >;

} // namespace HIPP

#endif	//_HIPPCNTL_CONCEPT_CONTIGUOUS_BUFFER_H_