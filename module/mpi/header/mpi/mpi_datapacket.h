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

/**
 * a datapacked is defined, in the high-level interface, as a triplet 
 *          <buff_addr, buff_size, datatype>.
 * This is consistent with most MPI data buffer specification in the oringinal
 * interface. However, here we provides many ways to constructs a packet for
 * your convinient.
 */
class Datapacket{
public:
    /**
     * data packet constructors
     * (1)  formally specify the buffer as a triplet.
     * (2)  same with (1), but use a string that can be converted to a datatype
     *      by the high-level library.
     * (3)  used a string. This is equivalent to specify the triplet as
     *          <(void *)buff.data(), buff.size(), "char">
     * (4)  use a vector. This is equivalent to specify the triplet as
     *          <(void *)buff.data(), buff.size(), "T">
     *      Not that only high-level-interface-predefined types are supported.
     * A data packed can be copied-construncted, copied-assigned, move-
     * constructed or move-assigned.
     */
    Datapacket( const void *buff, int size, Datatype dtype ) noexcept;
    Datapacket( const void *buff, int size, const string &dtype );
    explicit Datapacket( const string &buff ) noexcept;
    template<typename T, typename A>
    explicit Datapacket( const vector<T,A> &buff ) noexcept;

    Datapacket(const Datapacket &) noexcept;
    Datapacket(Datapacket &&) noexcept;
    Datapacket & operator=(const Datapacket &) noexcept;
    Datapacket & operator=(Datapacket &&) noexcept;
    ~Datapacket() noexcept {}
protected:
    void *_buff;
    int _size;
    Datatype _dtype;
    friend class Comm;
};

template<typename T, typename A>
Datapacket::Datapacket( const vector<T,A> &buff ) noexcept
    :Datapacket( buff.data(), buff.size(), *_TypeCvt<T>::datatype ){ }



} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_DATAPACKET_H_