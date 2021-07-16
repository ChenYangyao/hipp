/**
 * creat: Yangyao CHEN, 2020/01/15
 *      [write   ] Datapacket - for user to specify the data buffer in 
 *                              communication.
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
 * a datapacked is defined, in the high-level interface, as a triplet 
 *          <buff_addr, buff_size, datatype>.
 * This is consistent with most MPI data buffer specification in the original
 * interface. 
 * 
 * However, we provide more ways to constructs a packet, for
 * your convinience, including by the original MPI triplet, by a scalar,
 * or by an array-like instance. See the constructors of Datapacket.
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

    /**
     * Formally specify the buffer as a triplet. The second version with string 
     * "dtype" automatically converts to a suitable underlying dtype.
     */
    Datapacket(const void *buff, int size, Datatype dtype) noexcept;
    Datapacket(const void *buff, int size, const string &dtype);

    /**
     * Use a scalar or std::string as the data buffer. 
     * The scalar can be either a arithmetic type (e.g., int, float, bool) 
     * or std::complex<> of floats.
     */
    Datapacket(const string &buff) noexcept
    : Datapacket( buff.data(), buff.size(), CHAR){ }
    
    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), int> =0>
    Datapacket(const T &buff) noexcept
    : Datapacket( &buff, 1, *_TypeCvt<T>::datatype){ }

    /**
     * Use array-like instance as the buffer. It can be a raw-array of 
     * scalar (like int [3]), a raw buffer of scalar (like int * which points
     * to n integers), a instance of std::array or std::vector.
     */
    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), size_t> N>
    Datapacket(const T (&buff)[N]) noexcept
    : Datapacket( buff, N, *_TypeCvt<T>::datatype){ }

    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), int> =0>
    Datapacket(const T *buff, size_t n) noexcept
    : Datapacket( buff, n, *_TypeCvt<T>::datatype ){ }

    template<typename T, std::enable_if_t<_is_intern_dtype<T>(), size_t> N>
    Datapacket(const std::array<T, N> &buff) noexcept
    : Datapacket( buff.data(), N, *_TypeCvt<T>::datatype ){ }

    template<typename T, typename A, 
        std::enable_if_t<_is_intern_dtype<T>(), int> =0>
    Datapacket(const vector<T,A> &buff) noexcept
    : Datapacket( buff.data(), buff.size(), *_TypeCvt<T>::datatype ){ }

    /**
     * Sometimes, triplet is used to specify a memory segement relative to 
     * a base address (e.g., in RMA operations). In such cases, the first 
     * element is a displacement relative to the base address. 
     * 
     * The Datapacket type can represent such triplets, too. Internally, the
     * displacement is stored by casting into a (void *).
     */
    Datapacket(aint_t disp, int size, Datatype dtype) noexcept
    : Datapacket((char *)(0)+disp, size, dtype){ }
    Datapacket(aint_t disp, int size, const string &dtype)
    : Datapacket((char *)(0)+disp, size, dtype){ }

    Datapacket(const Datapacket &p) noexcept
    :Datapacket( p._buff, p._size, p._dtype ){ }
    Datapacket(Datapacket &&p) noexcept
    :Datapacket( p._buff, p._size, std::move(p._dtype) ){ }
    Datapacket & operator=(const Datapacket &p) noexcept;
    Datapacket & operator=(Datapacket &&p) noexcept;
    ~Datapacket() noexcept {}
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



} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_DATAPACKET_H_