/**
create: Yangyao CHEN, 2020/01/15
    [write   ] Datapacket - for user to specify the data buffer in 
        communication.
*/


#ifndef _HIPPMPI_MPI_DATAPACKET_H_
#define _HIPPMPI_MPI_DATAPACKET_H_
#include "mpi_datatype.h"

namespace HIPP::MPI {

namespace _mpi_datapacket_helper {
struct _datapacket_helper {
    template<typename T>
    static constexpr bool _has_def() noexcept { 
        return DatatypeTraits<T>::has_mpi_datatype; 
    }
    template<typename T>
    static constexpr bool _is_predef() noexcept { 
        if constexpr( !_has_def<T>() ) { return false; }
        else return DatatypeTraits<T>::is_predefined; 
    }
    template<typename T>
    static constexpr bool _is_custom() noexcept { 
        if constexpr ( !_has_def<T>() ) { return false; }
        else return ! DatatypeTraits<T>::is_predefined;
    }
    template<typename T>
    static constexpr bool _is_buffer() noexcept {
        return ContiguousBufferTraits<T>::is_buffer;
    }
    template<typename T, typename V=void>
    struct _buffer_value { using value_t = void; };
    template<typename T>
    struct _buffer_value<T, std::enable_if_t<_is_buffer<T>()> > {
        using value_t = typename ContiguousBufferTraits<T>::value_t;
    };
    template<typename T>
    using _buffer_value_t = typename _buffer_value<T>::value_t;
};
} // namespace _mpi_datapacket_helper

/**
A datapacket is defined, in the high-level interface, as a pack of the 
standard MPI data buffer specification, i.e.,
         <buff_addr, buff_size, datatype>.
Datapacket gives the communications calls flexibility, in that multiple types
of objects can be directly passed in as buffer.

The Datapacket type has tuple-like API for structured binding, e.g., 
std::vector<int> v {1,2,3};
auto [buff, size, datatype] = Datapacket(v);
Here, buff, size, datatype are reference type to void *, int and Datatype, 
respectively.

The const variant, ``ConstDatapacket`` refers to a constant buffer, (i.e., the
buff_addr is a pointer to constant buffer). The non-const variant, 
``Datapacket`` refers the non-const buffer. They are the same except this 
difference.
*/
class Datapacket : public _mpi_datapacket_helper::_datapacket_helper {
public:
    /**
    Constructors.

    (1). Default constructor - equivalent to (2) with arguments 
    ``{BOTTOM, 0, INT}``.
    
    (2). MPI standard buffer specification, i.e., with a triplet of starting 
    address, number of data items, and datatype of each item.
    
    (3). The same as (2), but specify the datatype by its name. The actual used
    datatype is like that returned by Datatype::from_name. Ordinary names are 
    supported, e.g., char, int, unsigned short, float, int16_t, bool, etc.
    
    (4). Equivalent to (2) with arguments ``{s.data(), s.size(), CHAR}``.

    (5). The same as (2), but the datatype is inferred from the pointed type 
    ``T``. The actual used datatype is like that return by 
    :expr:`Datatype::from_type`.
    Ordinary types are suppoeted, like char, int, unsigned short, float, bool, 
    etc. Any types that are DatatypeTraits-conformable are also supported.
    
    (6) The buffer is inferred from the argument. The library tries to infer
    the buffer by the following order until success:
    - If ``T`` is Predefined DatatypeTraits-conformable, treat ``x`` as a single 
      data element. Examples include a single int, double, etc.
    - If ``T`` is ContiguousBufferTraits-conformable and its element is 
      Predefined DatatypeTraits-conformable, treat ``x`` as a sequence of 
      data elements typed ContiguousBuffer<T>::value_t.
      e.g., ``vector<int> v{1,2,3,4,5}`` gives the buffer 
      ``{v.data(), 5, INT}``.
    - If ``T`` is Customized DatatypeTraits-conformable, treat ``x`` as a single 
      data element.
    - If T is ContiguousBufferTraits-conformable and its element is Customized 
      DatatypeTraits-conformable, treat ``x`` as a sequence of 
      data elements typed ContiguousBuffer<T>::value_t.
    - If all the above inferences failed, raise a compile error.
    
    Note that in any of the constructors, the data buffer must be non-const.
    */
    Datapacket() noexcept;
    Datapacket(void *buff, int size, Datatype dtype) noexcept;
    Datapacket(void *buff, int size, const string &dtype);

    Datapacket(string &s) noexcept;
    
    template<typename T, std::enable_if_t<_has_def<T>(), int> =0 >
    Datapacket(T *buff, int n) noexcept
    : Datapacket(buff, n, TypeCvt<T>::datatype()) { }

    template<typename T>
    Datapacket(T &x);

    /**
    Sometimes, triplet is used to specify a memory segment relative to 
    a base address (e.g., in RMA operations). In such cases, the first 
    part of the triplet is a displacement relative to the base address. 
    
    The ``Datapacket`` type can represent such triplets, too. Internally, the
    displacement is stored by casting into a ``void *``.
    */
    Datapacket(aint_t disp, int size, Datatype dtype) noexcept;
    Datapacket(aint_t disp, int size, const string &dtype);

    /**
    Copy/Move operation. The result refers to the same data buffer.
    */
    Datapacket(const Datapacket &p) noexcept;
    Datapacket(Datapacket &&p) noexcept;
    Datapacket & operator=(const Datapacket &p) noexcept;
    Datapacket & operator=(Datapacket &&p) noexcept;
    ~Datapacket() noexcept {}

    /**
    info() prints short (``fmt_cntl=0``) or verbose (``fmt_cntl=1``) information 
    to the stream ``os``. 
    The ``<<`` operator is equivalent to ``info()`` with ``fmt_cntl=0``.
    */
    ostream &info(ostream &os = cout, int fmt_cntl = 1) const;
    friend ostream & operator<<(ostream &os, const Datapacket &dpacket);

    /**
    Getters.
    get_buff(), get_size(), get_dtype() return the buffer starting address,
    buffer size, and datatype, respectively.
    */
    void * const & get_buff() const noexcept;
    int const & get_size() const noexcept;
    Datatype const & get_dtype() const noexcept;

    void * & get_buff() noexcept;
    int & get_size() noexcept;
    Datatype & get_dtype() noexcept;
protected:
    void *_buff;
    int _size;
    Datatype _dtype;
};

/**
See the docs for Datapacket.
*/
class ConstDatapacket : public _mpi_datapacket_helper::_datapacket_helper {
public:
    ConstDatapacket() noexcept;
    ConstDatapacket(const void *buff, int size, Datatype dtype) noexcept;
    ConstDatapacket(const void *buff, int size, const string &dtype);
    
    ConstDatapacket(const string &s) noexcept;

    template<typename T, std::enable_if_t<_has_def<T>(), int> =0 >
    ConstDatapacket(const T *buff, int n) noexcept
    : ConstDatapacket(buff, n, TypeCvt<T>::datatype()) { }

    template<typename T>
    ConstDatapacket(const T &x);

    ConstDatapacket(aint_t disp, int size, Datatype dtype) noexcept;
    ConstDatapacket(aint_t disp, int size, const string &dtype);

    /**
    It is valid to use a ``Datapacket`` as a ``ConstDatapacket``. The 
    constructed ``ConstDatapacket`` refers to the same buffer, has the same
    size, and share the same datatype as ``p``.
    */
    ConstDatapacket(const Datapacket &p) noexcept;

    ConstDatapacket(const ConstDatapacket &p) noexcept;
    ConstDatapacket(ConstDatapacket &&p) noexcept;
    ConstDatapacket & operator=(const ConstDatapacket &p) noexcept;
    ConstDatapacket & operator=(ConstDatapacket &&p) noexcept;
    ~ConstDatapacket() noexcept {}

    ostream &info(ostream &os = cout, int fmt_cntl = 1) const;
    friend ostream & operator<<(ostream &os, const ConstDatapacket &dpacket);

    const void * const & get_buff() const noexcept;
    int const & get_size() const noexcept;
    Datatype const & get_dtype() const noexcept;

    const void * & get_buff() noexcept;
    int & get_size() noexcept;
    Datatype & get_dtype() noexcept;
protected:
    const void *_buff;
    int _size;
    Datatype _dtype;
};


/* Tuple-like API: initializers for bindings. */
template<std::size_t I> 
decltype(auto) get( Datapacket &dp ) {
    static_assert( I < 3 );
    if constexpr ( I == 0 ) return dp.get_buff();
    else if constexpr ( I == 1 ) return dp.get_size();
    else return dp.get_dtype();
}

template<std::size_t I> 
decltype(auto) get( const Datapacket &dp ) {
    static_assert( I < 3 );
    if constexpr ( I == 0 ) return dp.get_buff();
    else if constexpr ( I == 1 ) return dp.get_size();
    else return dp.get_dtype();
}

template<std::size_t I> 
decltype(auto) get( Datapacket &&dp ) {
    static_assert( I < 3 );
    if constexpr ( I == 0 ) return std::move(dp.get_buff());
    else if constexpr ( I == 1 ) return std::move(dp.get_size());
    else return std::move(dp.get_dtype());
}

template<std::size_t I> 
decltype(auto) get( ConstDatapacket &dp ) {
    static_assert( I < 3 );
    if constexpr ( I == 0 ) return dp.get_buff();
    else if constexpr ( I == 1 ) return dp.get_size();
    else return dp.get_dtype();
}

template<std::size_t I> 
decltype(auto) get( const ConstDatapacket &dp ) {
    static_assert( I < 3 );
    if constexpr ( I == 0 ) return dp.get_buff();
    else if constexpr ( I == 1 ) return dp.get_size();
    else return dp.get_dtype();
}

template<std::size_t I> 
decltype(auto) get( ConstDatapacket &&dp ) {
    static_assert( I < 3 );
    if constexpr ( I == 0 ) return std::move(dp.get_buff());
    else if constexpr ( I == 1 ) return std::move(dp.get_size());
    else return std::move(dp.get_dtype());
}

inline Datapacket::Datapacket() noexcept : Datapacket(BOTTOM, 0, INT) { }

inline Datapacket::Datapacket(void *buff, int size, Datatype dtype) noexcept
: _buff(buff), _size(size), _dtype( std::move(dtype) ) { }

inline Datapacket::Datapacket(void *buff, int size, const string &dtype)
: Datapacket(buff, size, Datatype::from_name(dtype)) { }

inline Datapacket::Datapacket(string &s) noexcept
: Datapacket(s.data(), static_cast<int>(s.size()), CHAR) { }

template<typename T>
inline Datapacket::Datapacket(T &x) : Datapacket(BOTTOM, 0, Datatype{nullptr})
{   
    typedef _buffer_value_t<T> value_t;

    if constexpr( _is_predef<T>() ) {
        // Single element of predefined datatype.
        _buff = &x; _size = 1; _dtype = TypeCvt<T>::datatype();
    }else if constexpr ( _is_buffer<T>() && _is_predef<value_t>() ) 
    {
        // Contiguous buffer of elements with predefined datatype.
        auto [p,n] = ContiguousBuffer<value_t>(x);
        _buff = p; _size = static_cast<int>(n);
        _dtype = TypeCvt<value_t>::datatype();
    }else if constexpr ( _is_custom<T>() ) {
        // Single element of customized datatype.
        _buff = &x; _size = 1; _dtype = TypeCvt<T>::datatype();
    }else { 
        // Contiguous buffer of elements with customized datatype.
        auto [p,n] = ContiguousBuffer<value_t>(x);
        _buff = p; _size = static_cast<int>(n);
        _dtype = TypeCvt<value_t>::datatype();
    }
}

inline Datapacket::Datapacket(aint_t disp, int size, Datatype dtype) noexcept
: Datapacket((char *)(0)+disp, size, std::move(dtype) ){ }

inline Datapacket::Datapacket(aint_t disp, int size, const string &dtype)
: Datapacket((char *)(0)+disp, size, dtype){ }

inline Datapacket::Datapacket(const Datapacket &p) noexcept
:Datapacket( p._buff, p._size, p._dtype ){ }

inline Datapacket::Datapacket(Datapacket &&p) noexcept
:Datapacket( p._buff, p._size, std::move(p._dtype) ){ }

inline Datapacket & Datapacket::operator=(const Datapacket &p) noexcept{
    if( this != &p ){
        _buff = p._buff;
        _size = p._size;
        _dtype = p._dtype;
    }
    return *this;
}

inline Datapacket & Datapacket::operator=(Datapacket &&p) noexcept{
    if( this != &p ){
        _buff = p._buff;
        _size = p._size;
        _dtype = std::move(p._dtype);
    }
    return *this;
}

inline ostream & operator<<(ostream &os, const Datapacket &dpacket) {
    return dpacket.info(os, 0);
}

inline void * const & Datapacket::get_buff() const noexcept { 
    return _buff; 
}

inline int const & Datapacket::get_size() const noexcept { 
    return _size; 
}

inline Datatype const & Datapacket::get_dtype() const noexcept { 
    return _dtype;
}

inline void * & Datapacket::get_buff() noexcept { 
    return _buff; 
}

inline int & Datapacket::get_size() noexcept { 
    return _size; 
}

inline Datatype & Datapacket::get_dtype() noexcept { 
    return _dtype; 
}

inline ConstDatapacket::ConstDatapacket() noexcept 
: ConstDatapacket(BOTTOM, 0, INT) { }

inline ConstDatapacket::ConstDatapacket(const void *buff, int size, 
Datatype dtype) noexcept
: _buff(buff), _size(size), _dtype( std::move(dtype) ) { }

inline ConstDatapacket::ConstDatapacket(const void *buff, int size, 
const string &dtype)
: ConstDatapacket(buff, size, Datatype::from_name(dtype)) { }

inline ConstDatapacket::ConstDatapacket(const string &s) noexcept 
: ConstDatapacket(s.data(), static_cast<int>(s.size()), CHAR) { }

template<typename T>
inline ConstDatapacket::ConstDatapacket(const T &x) 
: ConstDatapacket(BOTTOM, 0, Datatype{nullptr})
{
    typedef _buffer_value_t<const T> value_t;

    if constexpr( _is_predef<T>() ) {
        _buff = &x; _size = 1; _dtype = TypeCvt<T>::datatype();
    }else if constexpr ( _is_buffer<T>() && _is_predef<value_t>() ) 
    {
        auto [p,n] = ContiguousBuffer<value_t>(x);
        _buff = p; _size = static_cast<int>(n);
        _dtype = TypeCvt<value_t>::datatype();
    }else if constexpr ( _is_custom<T>() ) {
        _buff = &x; _size = 1; _dtype = TypeCvt<T>::datatype();
    }else { 
        auto [p,n] = ContiguousBuffer<value_t>(x);
        _buff = p; _size = static_cast<int>(n);
        _dtype = TypeCvt<value_t>::datatype();
    }
}

inline ConstDatapacket::ConstDatapacket(aint_t disp, int size, 
Datatype dtype) noexcept
: ConstDatapacket((const char *)(0)+disp, size, std::move(dtype) ){ }

inline ConstDatapacket::ConstDatapacket(aint_t disp, int size, 
const string &dtype)
: ConstDatapacket((const char *)(0)+disp, size, dtype){ }

inline ConstDatapacket::ConstDatapacket(const Datapacket &p) noexcept 
: ConstDatapacket(p.get_buff(), p.get_size(), p.get_dtype()){ }

inline ConstDatapacket::ConstDatapacket(const ConstDatapacket &p) noexcept
:ConstDatapacket( p._buff, p._size, p._dtype ){ }

inline ConstDatapacket::ConstDatapacket(ConstDatapacket &&p) noexcept
:ConstDatapacket( p._buff, p._size, std::move(p._dtype) ){ }

inline ConstDatapacket & ConstDatapacket::operator=(
    const ConstDatapacket &p) noexcept
{
    if( this != &p ){
        _buff = p._buff;
        _size = p._size;
        _dtype = p._dtype;
    }
    return *this;
}

inline ConstDatapacket & ConstDatapacket::operator=(
    ConstDatapacket &&p) noexcept
{
    if( this != &p ){
        _buff = p._buff;
        _size = p._size;
        _dtype = std::move(p._dtype);
    }
    return *this;
}

inline ostream & operator<<(ostream &os, const ConstDatapacket &dpacket) {
    return dpacket.info(os, 0);
}

inline const void * const & ConstDatapacket::get_buff() const noexcept  { 
    return _buff; 
}

inline int const & ConstDatapacket::get_size() const noexcept  { 
    return _size; 
}

inline Datatype const & ConstDatapacket::get_dtype() const noexcept  { 
    return _dtype;
}

inline const void * & ConstDatapacket::get_buff() noexcept  { 
    return _buff; 
}

inline int & ConstDatapacket::get_size() noexcept  { 
    return _size; 
}

inline Datatype & ConstDatapacket::get_dtype() noexcept  { 
    return _dtype; 
}


} // namespace HIPP::MPI

/* Tuple-like API: size and member type. */
namespace std {

template<> 
struct tuple_size<HIPP::MPI::Datapacket> {
    inline static constexpr std::size_t value = 3;
};

template<> 
struct tuple_element<0, HIPP::MPI::Datapacket> { using type = void *; };

template<> 
struct tuple_element<1, HIPP::MPI::Datapacket> { using type = int; };

template<> 
struct tuple_element<2, HIPP::MPI::Datapacket> { 
    using type = HIPP::MPI::Datatype; 
};

template<> 
struct tuple_size<HIPP::MPI::ConstDatapacket> {
    inline static constexpr std::size_t value = 3;
};

template<> 
struct tuple_element<0, HIPP::MPI::ConstDatapacket> { 
    using type = const void *; 
};

template<> 
struct tuple_element<1, HIPP::MPI::ConstDatapacket> { using type = int; };

template<> 
struct tuple_element<2, HIPP::MPI::ConstDatapacket> { 
    using type = HIPP::MPI::Datatype; 
};

} // namespace std

#endif	//_HIPPMPI_MPI_DATAPACKET_H_