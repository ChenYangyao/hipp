/**
create: Yangyao CHEN, 2020/01/18
    [write   ] Status - point-to-point communication returning status interface.
*/ 

#ifndef _HIPPMPI_MPI_STATUS_H_
#define _HIPPMPI_MPI_STATUS_H_
#include "mpi_obj_base.h"
#include "mpi_datapacket.h"
namespace HIPP{
namespace MPI{

/**
Point-to-point communication returning status interface.

The Status object is binary compatible with original ``MPI_Status``. That is,
a conversion from ``Status *`` to ``MPI_Status *`` is always valid. This design
is to reduce the overhead when waiting/testing multiple messages in the 
non-block communications.
*/
class Status {
public:
    typedef MPI_Status mpi_t;
    Status() noexcept { }
    Status(mpi_t status) noexcept : _status( status ){ }
    ~Status() noexcept { }

    Status(const Status &) noexcept = default;
    Status(Status &&) noexcept = default;
    Status & operator=(const Status &) noexcept = default;
    Status & operator=(Status &&) noexcept = default;
    
    /**
    Query the message properties, rank of srouce process, tag of the matched
    message, error code, and count of the items.

    The error code is set only when the multiple completion call failed and
    an ERR_IN_STATUS is returned.

    @dtype: pre-defined or derived datatype. Should be exactly the same 
        datatype used in the communication that returns this status.
        Only pre-defined datatypes support the string version.
    */
    int source() const noexcept;
    int tag() const noexcept;
    int error() const noexcept;
    int count( const Datatype &dtype ) const;
    int count( const string &dtype ) const;

    bool test_cancelled() const;

    /**
    Return the internal value representing the MPI object.
    */
    mpi_t raw() const noexcept;
protected:
    mpi_t _status;
    int _count( Datatype::mpi_t dtype ) const;
};

inline int Status::source() const noexcept { 
    return _status.MPI_SOURCE; 
}

inline int Status::tag() const noexcept { 
    return _status.MPI_TAG; 
}

inline int Status::error() const noexcept { 
    return _status.MPI_ERROR; 
}

inline int Status::count( const Datatype &dtype ) const { 
    return _count(dtype.raw()); 
}

inline int Status::count( const string &dtype ) const { 
    return _count( Datatype::from_name(dtype).raw() ); 
}

inline bool Status::test_cancelled() const{
    int flag;
    ErrMPI::check(MPI_Test_cancelled(&_status, &flag), emFLPFB); 
    return flag;
}

inline Status::mpi_t Status::raw() const noexcept {
    return _status;
}

inline int Status::_count( Datatype::mpi_t dtype ) const {
    int cnt;
    ErrMPI::check( MPI_Get_count( &_status, dtype, &cnt ), emFLPFB );
    return cnt;
}

} // namespace MPI    
} // namespace HIPP

#endif	//_HIPPMPI_MPI_STATUS_H_