/**
 * creat: Yangyao CHEN, 2020/01/18
 *      [write   ] Status - point-to-point communication returning status 
 *                          interface.
 */ 

#ifndef _HIPPMPI_MPI_STATUS_H_
#define _HIPPMPI_MPI_STATUS_H_
#include "mpi_obj_base.h"
#include "mpi_datapacket.h"
namespace HIPP{
namespace MPI{

/**
 * point-to-point communication returning status interface.
 * 
 * The Status object is binary compatible with original MPI_Status. That is,
 * a conversion from (Status *) to (MPI_Status *) is always valid. This design
 * is to reduce the overhead when waiting/testing multiple messages in the 
 * non-block communications.
 */
class Status{
public:
    typedef MPI_Status mpi_t;
    Status() noexcept { }
    Status( mpi_t status) noexcept : _status( status ){ }
    ~Status() noexcept { }
    
    /**
     * query the message properties, rank of srouce process, tag of the matched
     * message, error code, and count of the items.
     * 
     * The error code is set only when the multiple completion call failed and
     * an ERR_IN_STATUS is returned.
     * 
     * @dtype:  pre-defined or derived datatype. Should be exactly the same 
     *          datatype used in the communication that returns this status.
     *          Only pre-defined datatypes support the string version.
     */
    int source() const noexcept { return _status.MPI_SOURCE; }
    int tag() const noexcept { return _status.MPI_TAG; }
    int error() const noexcept { return _status.MPI_ERROR; }
    int count( const Datatype &dtype ) const { return _count(dtype.raw()); }
    int count( const string &dtype ) const;
protected:
    mpi_t _status;
    int _count( Datatype::mpi_t dtype ) const {
        int cnt;
        ErrMPI::check( MPI_Get_count( &_status, dtype, &cnt ), emFLPFB );
        return cnt;
    }
};

inline int Status::count( const string &dtype ) const { 
    auto it = _typecvt.find(dtype);
    if( it == _typecvt.end() )
        ErrLogic::throw_( ErrLogic::eINVALIDARG, emFLPFB, 
            "  ... datatype ", dtype, " is invalid\n" );
    return _count( it->second->raw() );
}

} // namespace MPI    
} // namespace HIPP

#endif	//_HIPPMPI_MPI_STATUS_H_