/**
 * creat: Yangyao CHEN, 2020/01/21
 *      [write   ] _Requests - the intermediate-level MPI requests interface.
 */
#ifndef _HIPPMPI_MPI_RAW_REQUESTS_H_
#define _HIPPMPI_MPI_RAW_REQUESTS_H_
#include "mpi_base.h"
#include "mpi_error.h"
#include "mpi_status.h"
namespace HIPP{
namespace MPI{

/**
 * the intermediate-level MPI requests interface.
 * 
 * _Requests should not be constructed by users directly. The life time of it is 
 * handled by the high-level interfaces.
 */
class _Requests{
public:
    typedef MPI_Request mpi_t;
    enum : int { stFREE=0x1 };

    _Requests( ) {}
    _Requests( mpi_t val, int state ) {
        _vals.push_back(val); _states.push_back(state);
    }
    _Requests( int n, mpi_t *vals, int *states ){
        _vals.insert( _vals.end(), vals, vals+n );
        _states.insert( _states.end(), states, states+n );
    }
    ~_Requests() noexcept{ _destroy_self(); }
    static void free( mpi_t &req ){
        ErrMPI::check(MPI_Request_free(&req), emFLPFB);
    }
    mpi_t raw() const { return raw(0); }
    mpi_t raw(int i) const { return _vals[i]; }
    bool is_null() const noexcept { return is_null(0); }
    bool is_null(int i) const noexcept { return _vals[i] == nullval(); }
    static mpi_t nullval() noexcept { return MPI_REQUEST_NULL; }
    int size() const noexcept { return _vals.size(); }
    bool empty(){ return _vals.empty(); }

    void put( _Requests & rqs){ put( std::move(rqs) ); }
    void put( _Requests && rqs){
        _vals.insert( _vals.end(), rqs._vals.begin(), rqs._vals.end() );
        _states.insert( _states.end(), 
            rqs._states.begin(), rqs._states.end() );
        rqs._vals.clear();
        rqs._states.clear();
    }
    void get( int i, _Requests &rqs ){
        rqs._destroy_self();
        rqs._vals.push_back( _vals[i] );
        rqs._states.push_back( _states[i] );
        _vals[i] = _vals.back();
        _states[i] = _states.back();
        _vals.pop_back();
        _states.pop_back();
    }
    void get( int b, int e, _Requests &rqs ){
        rqs._destroy_self();
        rqs._vals.insert( rqs._vals.end(), _vals.begin()+b, _vals.begin()+e );
        rqs._states.insert( rqs._states.end(), 
            _states.begin()+b, _states.begin()+e );
        int n_get = e-b, n_after = size()-e, n_move = std::min(n_get, n_after);
        std::copy( _vals.end()-n_move, _vals.end(), _vals.begin()+b );
        std::copy( _states.end()-n_move, _states.end(), _states.begin()+b );
        int n_new = size() - n_get;
        _vals.resize( n_new );
        _vals.resize( n_new);
    }

    Status::mpi_t wait() { return wait(0); }
    Status::mpi_t wait(int i) {
        Status::mpi_t st;
        ErrMPI::check( MPI_Wait(&_vals[i], &st), emFLPFB );
        return st;
    }
    Status::mpi_t test(int &flag) { return test(0, flag); }
    Status::mpi_t test(int i, int &flag) {
        Status::mpi_t st;
        ErrMPI::check( MPI_Test(&_vals[i], &flag, &st), emFLPFB );
        return st;
    }
    Status::mpi_t status(int &flag) const { return status(0, flag); }
    Status::mpi_t status(int i, int &flag) const {
        Status::mpi_t st;
        ErrMPI::check( MPI_Request_get_status(
            _vals[i], &flag, &st), emFLPFB );
        return st;
    }
    Status::mpi_t waitany(int &index){
        Status::mpi_t st;
        ErrMPI::check( 
            MPI_Waitany(_vals.size(), _vals.data(), &index, &st), emFLPFB );
        return st;
    }
    Status::mpi_t testany(int &index, int &flag){
        Status::mpi_t st;
        ErrMPI::check(
            MPI_Testany(_vals.size(), _vals.data(), &index, &flag, &st), 
            emFLPFB );
        return st;
    }
    void waitall( Status::mpi_t *statuses ){
        ErrMPI::check(
            MPI_Waitall(_vals.size(), _vals.data(), statuses), 
            emFLPFB );
    }
    void testall( int &flag, Status::mpi_t *statuses ){
        ErrMPI::check(
            MPI_Testall(_vals.size(), _vals.data(), &flag, statuses), 
            emFLPFB );
    }
    void waitsome( int &count, int *indices, Status::mpi_t *statuses ){
        ErrMPI::check(
            MPI_Waitsome(_vals.size(), _vals.data(), &count, indices, statuses),
                emFLPFB );
    }
    void testsome( int &count, int *indices, Status::mpi_t *statuses ){
        ErrMPI::check(
            MPI_Testsome(_vals.size(), _vals.data(), &count, indices, statuses),
                emFLPFB );
    }
protected:
    vector<mpi_t> _vals;
    vector<int> _states;
    static void _destroy( mpi_t &val, int &state ) noexcept{
        if( state & stFREE  ){
            if( val != nullval() ){
                int e = MPI_Request_free( &val );
                if( e != MPI_SUCCESS ) ErrMPI::abort( e, emFLPFB, 
                    "  free failed\n" );
            }
        }else{
            if( val != nullval() )
                ErrMPI::abort(1, emFLPFB, "  none-null object is not freed");
        }
        state = 0;
    }
    void _destroy_self() noexcept{
        while( !_vals.empty() ){
            _destroy( _vals.back(), _states.back() );
            _vals.pop_back(); _states.pop_back();
        }
    }
};

} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_RAW_REQUESTS_H_