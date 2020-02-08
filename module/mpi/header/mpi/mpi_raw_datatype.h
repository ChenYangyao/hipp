/**
 * creat: Yangyao CHEN, 2020/01/21
 *      [write   ] _Datatype - the intermediate-level MPI datatype interface.
 */
#ifndef _HIPPMPI_MPI_RAW_DATATYPE_H_
#define _HIPPMPI_MPI_RAW_DATATYPE_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{

/**
 * the intermediate-level MPI datatype interface.
 * 
 * _Datatype should not be constructed by users directly. The life time of it is 
 * handled by the high-level interfaces.
 */
class _Datatype{
public:
    typedef MPI_Datatype mpi_t;
    enum : int { stFREE=0x1, stCOMMIT=0x2 };

    _Datatype( mpi_t val, int state ) noexcept : _val(val), _state(state){
        if( state & stCOMMIT )
            commit( _val );
    }
    ~_Datatype() noexcept { 
        if( bool(_state & stFREE) && !is_null() ){
            if( MPI_Type_free( &_val ) != MPI_SUCCESS )
                ErrMPI::abort( 1, emFLPFB );
        }
    }
    static void free( mpi_t &dtype ){
        ErrMPI::check(MPI_Type_free(&dtype), emFLPFB);
    }
    static void commit( mpi_t &dtype ){
        ErrMPI::check(MPI_Type_commit(&dtype), emFLPFB);
    }
    mpi_t raw() const noexcept { return _val; }
    bool is_null() const noexcept { return _val == nullval(); }
    void extent( aint_t &lb, aint_t &ext ) const{
        ErrMPI::check( MPI_Type_get_extent(_val, &lb, &ext), emFLPFB );
    }
    void true_extent( aint_t &lb, aint_t &ext ) const{
        ErrMPI::check( MPI_Type_get_true_extent(_val, &lb, &ext), emFLPFB );
    }
    int size() const{
        int _size;
        ErrMPI::check( MPI_Type_size(_val, &_size), emFLPFB );
        return _size;
    }
    static void get_envelope(mpi_t dtype, int &num_ints, int &num_addrs, 
        int &num_dtypes, int &combiner){
        ErrMPI::check( 
            MPI_Type_get_envelope( 
                dtype, &num_ints, &num_addrs, &num_dtypes, &combiner ), 
                emFLPFB );
    }

    mpi_t dup()const {
        mpi_t newtype;
        ErrMPI::check( MPI_Type_dup( _val, &newtype ), emFLPFB );
        return newtype;
    }
    static mpi_t resized( mpi_t dtype, aint_t lb, aint_t ext ){
        mpi_t newtype;
        ErrMPI::check( MPI_Type_create_resized( dtype, lb, ext, &newtype ),
            emFLPFB );
        return newtype;
    }
    static mpi_t contiguous( int count, mpi_t dtype ){
        mpi_t newtype;
        ErrMPI::check( MPI_Type_contiguous( count, dtype, &newtype ), emFLPFB );
        return newtype;
    }
    static mpi_t vector( int count, int blklen, int stride, mpi_t dtype ){
        mpi_t newtype;
        ErrMPI::check( 
            MPI_Type_vector( count, blklen, stride, dtype, &newtype ), 
            emFLPFB );
        return newtype;
    }
    static mpi_t hvector( int count, int blklen, aint_t stride, mpi_t dtype ){
        mpi_t newtype;
        ErrMPI::check(
            MPI_Type_create_hvector( count, blklen, stride, dtype, &newtype ),
            emFLPFB);
        return newtype;
    }
    static mpi_t indexed_block( int count, int blklen, 
        const int displs[], mpi_t dtype ){
        mpi_t newtype;
        ErrMPI::check(
            MPI_Type_create_indexed_block(count, blklen, displs, 
            dtype, &newtype), emFLPFB);
        return newtype;
    }
    static mpi_t hindexed_block( int count, int blklen, 
        const aint_t displs[], mpi_t dtype ){
        mpi_t newtype;
        ErrMPI::check(
            MPI_Type_create_hindexed_block(count, blklen, displs,
            dtype, &newtype), emFLPFB);
        return newtype;
    }
    static mpi_t indexed( int count, const int blklens[], 
        const int displs[], mpi_t dtype ){
        mpi_t newtype;
        ErrMPI::check(
            MPI_Type_indexed(count, blklens, displs,
            dtype, &newtype), emFLPFB);
        return newtype;
    }
    static mpi_t hindexed( int count, const int blklens[], 
        const aint_t displs[], mpi_t dtype ){
        mpi_t newtype;
        ErrMPI::check(
            MPI_Type_create_hindexed(count, blklens, displs, 
            dtype, &newtype), emFLPFB);
        return newtype;
    }
    static mpi_t struct_( int count, const int blklens[], 
        const aint_t displs[], const mpi_t dtypes[] ){
        mpi_t newtype;
        ErrMPI::check(
            MPI_Type_create_struct(count, blklens, displs,
            dtypes, &newtype), emFLPFB);
        return newtype;
    }
    static mpi_t darray( int size, int rank, int dims, const int gsizes[],
        const int distribs[], const int dargs[], const int psizes[],
        int order, mpi_t dtype ){
        mpi_t newtype;
        ErrMPI::check(
            MPI_Type_create_darray( size, rank, dims, gsizes, distribs, dargs, 
            psizes, order, dtype, &newtype ), emFLPFB );
        return newtype;
    }
    static mpi_t subarray( int ndims, const int sizes[], const int subsizes[], 
        const int starts[], int order, mpi_t dtype ){
        mpi_t newtype;
        ErrMPI::check(
            MPI_Type_create_subarray( ndims, sizes, subsizes, starts, 
            order, dtype, &newtype ), emFLPFB);
        return newtype;
    }
    static mpi_t nullval() noexcept { return MPI_DATATYPE_NULL; }
protected:
    mpi_t _val;
    int _state;
};


} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_RAW_DATATYPE_H_