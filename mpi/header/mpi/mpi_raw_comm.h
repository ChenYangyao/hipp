#ifndef _HIPPMPI_MPI_RAW_COMM_H_
#define _HIPPMPI_MPI_RAW_COMM_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{

class _Comm {
public:
    typedef MPI_Comm mpi_t;

    /**
     * val is the actual internal MPI_Comm variable, state indicates how its 
     * life time is controlled.
     * @state:  0 if the life time is system-managed, that is, not handled by
     *          this instance. Otherwise val is freed when deconstructing.
     */
    _Comm( mpi_t val, int state ) noexcept: _val(val), _state( state ){ }
    ~_Comm() noexcept{
        if( _state != 0 && ! is_null() ){
            if( MPI_Comm_free( &_val ) != MPI_SUCCESS )
                ErrMPI::abort(1, emFLPFB);
        }
    }
    static void free( mpi_t &comm ){
        ErrMPI::check(  MPI_Comm_free(&comm), emFLPFB );
    }
    int size() const{
        int _size;
        ErrMPI::check( MPI_Comm_size(_val, &_size), emFLPFB);
        return _size;
    }
    int rank() const{
        int _rank;
        ErrMPI::check( MPI_Comm_rank(_val, &_rank), emFLPFB );
        return _rank;
    }
    mpi_t raw() const noexcept { return _val; }
    bool is_null() const noexcept { return _val == nullval(); }


    mpi_t split( int color, int key = 0 )const{
        mpi_t newcomm;
        ErrMPI::check( 
            MPI_Comm_split( _val, color, key, &newcomm ), emFLPFB );
        return newcomm;
    }
    mpi_t dup( )const{
        mpi_t newcomm;
        ErrMPI::check( MPI_Comm_dup( _val, &newcomm ), emFLPFB );
        return newcomm;
    }
    static mpi_t create( mpi_t comm, MPI_Group group ){
        mpi_t newcomm;
        ErrMPI::check( MPI_Comm_create(comm, group, &newcomm), emFLPFB );
        return newcomm;
    }
    static mpi_t nullval() noexcept { return MPI_COMM_NULL; }
    static mpi_t selfval() noexcept { return MPI_COMM_SELF; }
    static mpi_t world() noexcept { return MPI_COMM_WORLD; }

    MPI_Group group() const{
        MPI_Group obj;
        ErrMPI::check( MPI_Comm_group(_val, &obj), emFLPFB );
        return obj;
    }

    void send( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const{
        ErrMPI::check( MPI_Send(buff, count, dtype, dest, tag, _val) );
    }
    MPI_Status recv( void *buff, int count, MPI_Datatype dtype, 
        int src, int tag ) const{
        MPI_Status st;
        ErrMPI::check( MPI_Recv(buff, count, dtype, src, tag, _val, &st) );
        return st;
    }
protected:
    mpi_t _val;
    int _state;
};
    
} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_RAW_COMM_H_