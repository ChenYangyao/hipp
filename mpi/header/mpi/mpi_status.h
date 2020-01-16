#ifndef _HIPPMPI_MPI_STATUS_H_
#define _HIPPMPI_MPI_STATUS_H_
#include "mpi_obj_base.h"
#include "mpi_datapacket.h"
namespace HIPP{
namespace MPI{
class Status{
public:
    typedef MPI_Status mpi_t;
    Status( mpi_t status): _status( status ){ }

    int source() const noexcept { return _status.MPI_SOURCE; }
    int tag() const noexcept { return _status.MPI_TAG; }
    int count( const Datatype &dtype ) const { return _count(dtype.raw()); }
    int count( const string &dtype ) const 
        { return _count( _typecvt[dtype]->raw() ); }
protected:
    mpi_t _status;
    int _count( Datatype::mpi_t dtype ) const {
        int cnt;
        ErrMPI::check( MPI_Get_count( &_status, dtype, &cnt ), emFLPFB );
        return cnt;
    }
};
} // namespace MPI    
} // namespace HIPP

#endif	//_HIPPMPI_MPI_STATUS_H_