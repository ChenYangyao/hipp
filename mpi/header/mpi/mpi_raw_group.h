#ifndef _HIPPMPI_MPI_RAW_GROUP_H_
#define _HIPPMPI_MPI_RAW_GROUP_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{
class _Group{
public:
    typedef MPI_Group mpi_t;
    _Group( mpi_t val, int state ) noexcept: _val(val), _state(state){ }
    ~_Group() noexcept {
        if( _state != 0 && ! is_null() )
            if( MPI_Group_free( &_val ) != MPI_SUCCESS )
                ErrMPI::abort(1, emFLPFB);
    }
    static void free( mpi_t &group ){
        ErrMPI::check( MPI_Group_free(&group), emFLPFB );
    }
    int size() const {
        int _size;
        ErrMPI::check( MPI_Group_size(_val, &_size), emFLPFB );
        return _size;
    }
    int rank() const {
        int _rank;
        ErrMPI::check( MPI_Group_rank(_val, &_rank), emFLPFB );
        return _rank;
    }
    mpi_t raw()const noexcept { return _val; }
    static void translate_ranks( 
        mpi_t group1, int n, const int ranks1[], mpi_t group2, int ranks2[] )
    {
        ErrMPI::check( 
            MPI_Group_translate_ranks(group1, n, ranks1, group2, ranks2),
            emFLPFB );
    }
    static int compare( mpi_t group1, mpi_t group2 ) {
        int res;
        ErrMPI::check( MPI_Group_compare(group1, group2, &res), emFLPFB);
        return res;
    }
    static mpi_t union_( mpi_t group1, mpi_t group2 ){
        mpi_t newgroup;
        ErrMPI::check( MPI_Group_union(group1, group2, &newgroup), emFLPFB );
        return newgroup;
    }
    static mpi_t intersection( mpi_t group1, mpi_t group2 ){
        mpi_t newgroup;
        ErrMPI::check( 
            MPI_Group_intersection(group1, group2, &newgroup), emFLPFB );
        return newgroup;
    }
    static mpi_t difference( mpi_t group1, mpi_t group2 ){
        mpi_t newgroup;
        ErrMPI::check( 
            MPI_Group_difference(group1, group2, &newgroup), emFLPFB );
        return newgroup;
    }
    mpi_t incl( int n, const int ranks[] ) const {
        mpi_t newgroup;
        ErrMPI::check(
            MPI_Group_incl(_val, n, ranks, &newgroup), emFLPFB);
        return newgroup;
    };
    mpi_t excl( int n, const int ranks[] ) const {
        mpi_t newgroup;
        ErrMPI::check(
            MPI_Group_excl(_val, n, ranks, &newgroup), emFLPFB);
        return newgroup;
    };
    mpi_t range_incl( int n, int ranges[][3] ) const {
        mpi_t newgroup;
        ErrMPI::check(
            MPI_Group_range_incl(_val, n, ranges, &newgroup), emFLPFB );
        return newgroup;
    }
    mpi_t range_excl( int n, int ranges[][3] ) const {
        mpi_t newgroup;
        ErrMPI::check(
            MPI_Group_range_excl(_val, n, ranges, &newgroup), emFLPFB );
        return newgroup;
    }
    static mpi_t nullval() noexcept { return MPI_GROUP_NULL; }
    static mpi_t emptyval() noexcept { return MPI_GROUP_EMPTY; }
    bool is_null() const noexcept { return _val == nullval(); }
protected:
    mpi_t _val;
    int _state;
};


} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_RAW_GROUP_H_