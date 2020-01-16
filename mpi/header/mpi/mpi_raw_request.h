#ifndef _HIPPMPI_MPI_RAW_REQUEST_H_
#define _HIPPMPI_MPI_RAW_REQUEST_H_
#include "mpi_base.h"
#include "mpi_error.h"
#include "mpi_status.h"
namespace HIPP{
namespace MPI{
class _Request{
public:
    typedef MPI_Request mpi_t;
    enum : int { stFREE=0x1 };

    _Request( mpi_t val, int state ) noexcept: _val(val), _state(state){ }
    ~_Request() noexcept{
        if( _state & stFREE  ){
            if( !is_null() ){
                if( MPI_Request_free( &_val ) != MPI_SUCCESS )
                    ErrMPI::abort(1, emFLPFB);
            }
        }else if( !is_null() )
            ErrMPI::abort(1, emFLPFB);
    }
    static void free( mpi_t &req ){
        ErrMPI::check(MPI_Request_free(&req), emFLPFB);
    }
    mpi_t raw() const noexcept { return _val; }
    bool is_null() const noexcept { return _val == nullval(); }
    static mpi_t nullval() noexcept { return MPI_REQUEST_NULL; }

    Status::mpi_t wait() {
        Status::mpi_t st;
        ErrMPI::check( MPI_Wait(&_val, &st), emFLPFB );
        return st;
    }
    bool test( Status::mpi_t &status ) {
        int flag;
        ErrMPI::check( MPI_Test(&_val, &flag, &status), emFLPFB );
        return bool(flag);
    }
protected:
    mpi_t _val;
    int _state;
};

} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_RAW_REQUEST_H_