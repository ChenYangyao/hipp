/**
 * creat: Yangyao CHEN, 2020/01/21
 *      [write   ] _Op - the intermediate-level MPI op interface.
 */
#ifndef _HIPPMPI_MPI_RAW_OP_H_
#define _HIPPMPI_MPI_RAW_OP_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{
    
class Op;


/**
 * the intermediate-level MPI op interface.
 * 
 * _Op should not be constructed by users directly. The life time of it is 
 * handled by the high-level interfaces.
 */
class _Op{
public:
    typedef MPI_Op mpi_t;
    typedef MPI_User_function user_fn_t;

    enum: int { stFREE=0x1 };

    _Op( mpi_t val, int state ) noexcept : _val(val), _state(state){ }
    ~_Op() noexcept{
        if( bool(_state & stFREE) & ! is_null() )
            if( MPI_Op_free( &_val ) != MPI_SUCCESS )
                ErrMPI::abort( 1, emFLPFB );
    }
    static void free( mpi_t op ) {
        ErrMPI::check( MPI_Op_free(&op), emFLPFB );
    }
    mpi_t raw()const noexcept{ return _val; }
    bool is_null() const noexcept { return _val == nullval(); }
    bool commutative() const {
        int commute;
        ErrMPI::check( MPI_Op_commutative( _val, &commute ), emFLPFB );
        return bool(commute);
    }
    static mpi_t create( user_fn_t *user_fn, int commute ){
        mpi_t user_op;
        ErrMPI::check( MPI_Op_create(user_fn, commute, &user_op), emFLPFB );
        return user_op;
    }

    static mpi_t nullval()noexcept { return MPI_OP_NULL; }
protected:
    mpi_t _val;
    int _state;

    friend class Op;
};



} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_RAW_OP_H_