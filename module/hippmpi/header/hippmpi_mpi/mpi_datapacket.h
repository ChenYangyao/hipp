/**
create: Yangyao CHEN, 2020/01/15
    [write   ] Datapacket - for user to specify the data buffer in 
        communication.
*/


#ifndef _HIPPMPI_MPI_DATAPACKET_H_
#define _HIPPMPI_MPI_DATAPACKET_H_
#include "mpi_datatype.h"
namespace HIPP{
namespace MPI{

class Comm;
class File;
class Win;
class Message;
class Pack;
class ExternalPack;

/**
A datapacket is defined, in the high-level interface, as a pack of the 
standard MPI data buffer specification, i.e.,
         <buff_addr, buff_size, datatype>.
Datapacket gives the communications calls flexibility in that multiple types
of objects can be directly passed in as buffer.

The Datapacket type has tuple-like API for structured binding, e.g., 
std::vector<int> v {1,2,3};
auto [buff, size, datatype] = Datapacket(v);
Here, buff, size, datatype are reference type to void *, int and Datatype, 
respectively.
*/
class Datapacket{
public:
    template<typename T>
    static constexpr bool _is_intern_dtype(){ 
        return std::is_arithmetic_v<T> || 
            std::is_same_v< std::complex<float>, T > || 
            std::is_same_v< std::complex<double>, T > || 
            std::is_same_v< std::complex<long double>, T >;
    }

    /*
    Equivalent to (BOTTOM, 0, INT) buffer.
    */
    Datapacket() noexcept : _buff(BOTTOM), _size(0), _dtype(INT) {}

    /**
    Formally specify the buffer as a triplet. The second version with string 
    "dtype" automatically converts to a suitable underlying dtype.
    */
    Datapacket(const void *buff, int size, Datatype dtype) noexcept;
    Datapacket(const void *buff, int size, const string &dtype);

    /**
    Use a scalar or std::string as the data buffer. 
    The scalar can be either a arithmetic type (e.g., int, float, bool) 
    or std::complex<> of floats.
    */
    Datapacket(const string &buff) noexcept
    : Datapacket( buff.data(), buff.size(), CHAR){ }
    
    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), int> =0>
    Datapacket(const T &buff) noexcept
    : Datapacket( &buff, 1, *_TypeCvt<T>::datatype){ }

    /**
    Use array-like instance as the buffer. It can be a raw-array of 
    scalar (like int [3]), a raw buffer of scalar (like int * which points
    to n integers), a instance of std::array or std::vector.
    */
    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), size_t> N>
    Datapacket(const T (&buff)[N]) noexcept
    : Datapacket( buff, static_cast<int>(N), *_TypeCvt<T>::datatype){ }

    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), int> =0>
    Datapacket(const T *buff, int n) noexcept
    : Datapacket( buff, n, *_TypeCvt<T>::datatype ){ }

    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), size_t> N>
    Datapacket(const std::array<T, N> &buff) noexcept
    : Datapacket( buff.data(), static_cast<int>(N), *_TypeCvt<T>::datatype ){ }

    template<typename T, typename A, 
        std::enable_if_t<_is_intern_dtype<T>(), int> =0>
    Datapacket(const vector<T,A> &buff) noexcept
    : Datapacket( buff.data(), static_cast<int>(buff.size()), 
        *_TypeCvt<T>::datatype ){ }

    /**
    Sometimes, triplet is used to specify a memory segement relative to 
    a base address (e.g., in RMA operations). In such cases, the first 
    element is a displacement relative to the base address. 
    
    The Datapacket type can represent such triplets, too. Internally, the
    displacement is stored by casting into a (void *).
    */
    Datapacket(aint_t disp, int size, Datatype dtype) noexcept
    : Datapacket((char *)(0)+disp, size, dtype){ }
    Datapacket(aint_t disp, int size, const string &dtype)
    : Datapacket((char *)(0)+disp, size, dtype){ }


    /**
    Copy/Move operation. The result refers to the same data buffer.
    */
    Datapacket(const Datapacket &p) noexcept
    :Datapacket( p._buff, p._size, p._dtype ){ }
    Datapacket(Datapacket &&p) noexcept
    :Datapacket( p._buff, p._size, std::move(p._dtype) ){ }
    Datapacket & operator=(const Datapacket &p) noexcept;
    Datapacket & operator=(Datapacket &&p) noexcept;
    ~Datapacket() noexcept {}

    /**
    Getters.
    get_buff(), get_size(), get_dtype() returns the buffer starting address,
    buffer size, and datatype, respectively.
    */
    void * const & get_buff() const noexcept { return _buff; }
    int const & get_size() const noexcept { return _size; }
    Datatype const & get_dtype() const noexcept { return _dtype; }

    void * & get_buff() noexcept { return _buff; }
    int & get_size() noexcept { return _size; }
    Datatype & get_dtype() noexcept { return _dtype; }
protected:
    void *_buff;
    int _size;
    Datatype _dtype;

    friend class Comm;
    friend class File;
    friend class Win;
    friend class Message;
    friend class Pack;
    friend class ExternalPack;
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

} // namespace MPI
} // namespace HIPP

/* Tuple-like API: size and member type. */
namespace std {

template<> 
struct tuple_size<HIPP::MPI::Datapacket> {
    static constexpr std::size_t value = 3;
};

template<> 
struct tuple_element<0, HIPP::MPI::Datapacket> { using type = void *; };

template<> 
struct tuple_element<1, HIPP::MPI::Datapacket> { using type = int; };

template<> 
struct tuple_element<2, HIPP::MPI::Datapacket> { 
    using type = HIPP::MPI::Datatype; 
};

} // namespace std




#endif	//_HIPPMPI_MPI_DATAPACKET_H_