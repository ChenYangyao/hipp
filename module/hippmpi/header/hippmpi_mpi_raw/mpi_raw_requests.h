/**
create: Yangyao CHEN, 2020/01/21
     [write   ] _Requests - the intermediate-level MPI requests interface.
*/
#ifndef _HIPPMPI_MPI_RAW_REQUESTS_H_
#define _HIPPMPI_MPI_RAW_REQUESTS_H_
#include "mpi_base.h"
#include "mpi_error.h"

namespace HIPP::MPI {

/**
The intermediate-level MPI requests interface.

``_Requests`` should not be constructed by users directly. The life time of it is 
handled by the high-level interfaces.
*/
class _Requests{
public:
    typedef MPI_Request mpi_t;
    enum : int { stFREE=0x1 };

    _Requests();
    _Requests(mpi_t val, int state);
    _Requests(int n, mpi_t *vals, int *states);
    ~_Requests() noexcept;
    static void free(mpi_t &req);
    mpi_t raw() const;
    mpi_t raw(int i) const;
    bool is_null() const noexcept;
    bool is_null(int i) const noexcept;
    static mpi_t nullval() noexcept;
    int size() const noexcept;
    bool empty();

    void put(_Requests & rqs);
    void put(_Requests && rqs);
    void get(int i, _Requests &rqs);
    void get(int b, int e, _Requests &rqs);

    void start();
    void start(int i);
    void startall();

    MPI_Status wait();
    MPI_Status wait(int i);
    MPI_Status test(int &flag);
    MPI_Status test(int i, int &flag);
    MPI_Status status(int &flag) const;
    MPI_Status status(int i, int &flag) const;
    MPI_Status waitany(int &index);
    MPI_Status testany(int &index, int &flag);
    void waitall(MPI_Status *statuses);
    void testall(int &flag, MPI_Status *statuses);
    void waitsome(int &count, int *indices, MPI_Status *statuses);
    void testsome(int &count, int *indices, MPI_Status *statuses);
    void cancel();
    void cancel(int i);
protected:
    vector<mpi_t> _vals;
    vector<int> _states;
    static void _destroy(mpi_t &val, int &state) noexcept;
    void _destroy_self() noexcept;
};

inline _Requests::_Requests() {}

inline _Requests::_Requests( mpi_t val, int state ) {
    _vals.push_back(val); _states.push_back(state);
}

inline _Requests::_Requests( int n, mpi_t *vals, int *states ){
    _vals.insert( _vals.end(), vals, vals+n );
    _states.insert( _states.end(), states, states+n );
}

inline _Requests::~_Requests() noexcept{ 
    _destroy_self(); 
}

inline void _Requests::free( mpi_t &req ) {
    ErrMPI::check(MPI_Request_free(&req), emFLPFB);
}

inline _Requests::mpi_t _Requests::raw() const { 
    return raw(0); 
}

inline _Requests::mpi_t _Requests::raw(int i) const { 
    return _vals[i]; 
}

inline bool _Requests::is_null() const noexcept { 
    return is_null(0); 
}

inline bool _Requests::is_null(int i) const noexcept { 
    return _vals[i] == nullval(); 
}

inline _Requests::mpi_t _Requests::nullval() noexcept { 
    return MPI_REQUEST_NULL; 
}

inline int _Requests::size() const noexcept { 
    return _vals.size(); 
}

inline bool _Requests::empty() { 
    return _vals.empty(); 
}

inline void _Requests::put( _Requests & rqs) { 
    put( std::move(rqs) ); 
}

inline void _Requests::put( _Requests && rqs){
    _vals.insert( _vals.end(), rqs._vals.begin(), rqs._vals.end() );
    _states.insert( _states.end(), 
        rqs._states.begin(), rqs._states.end() );
    rqs._vals.clear();
    rqs._states.clear();
}

inline void _Requests::get( int i, _Requests &rqs ) {
    rqs._destroy_self();
    rqs._vals.push_back( _vals[i] );
    rqs._states.push_back( _states[i] );
    _vals[i] = _vals.back();
    _states[i] = _states.back();
    _vals.pop_back();
    _states.pop_back();
}

inline void _Requests::get( int b, int e, _Requests &rqs ) {
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

inline void _Requests::start() {
    start(0);
}

inline void _Requests::start(int i) {
    ErrMPI::check(MPI_Start(&_vals[i]), 
        emFLPFB, "  ... index = ", i, '\n');
}

inline void _Requests::startall() {
    int n_rqs = _vals.size();
    auto *rqs = _vals.data();
    ErrMPI::check(MPI_Startall(n_rqs, rqs), 
        emFLPFB, "  ... try to start ", n_rqs, " requests\n");
}

inline MPI_Status _Requests::wait() { 
    return wait(0); 
}

inline MPI_Status _Requests::wait(int i) {
    MPI_Status st;
    ErrMPI::check( MPI_Wait(&_vals[i], &st), emFLPFB );
    return st;
}

inline MPI_Status _Requests::test(int &flag) { 
    return test(0, flag); 
}

inline MPI_Status _Requests::test(int i, int &flag) {
    MPI_Status st;
    ErrMPI::check( MPI_Test(&_vals[i], &flag, &st), emFLPFB );
    return st;
}

inline MPI_Status _Requests::status(int &flag) const { 
    return status(0, flag); 
}

inline MPI_Status _Requests::status(int i, int &flag) const {
    MPI_Status st;
    ErrMPI::check( MPI_Request_get_status(
        _vals[i], &flag, &st), emFLPFB );
    return st;
}

inline MPI_Status _Requests::waitany(int &index) {
    MPI_Status st;
    ErrMPI::check( 
        MPI_Waitany(_vals.size(), _vals.data(), &index, &st), emFLPFB );
    return st;
}

inline MPI_Status _Requests::testany(int &index, int &flag) {
    MPI_Status st;
    ErrMPI::check(
        MPI_Testany(_vals.size(), _vals.data(), &index, &flag, &st), 
        emFLPFB );
    return st;
}

inline void _Requests::waitall( MPI_Status *statuses ) {
    ErrMPI::check(
        MPI_Waitall(_vals.size(), _vals.data(), statuses), 
        emFLPFB );
}

inline void _Requests::testall( int &flag, MPI_Status *statuses ) {
    ErrMPI::check(
        MPI_Testall(_vals.size(), _vals.data(), &flag, statuses), 
        emFLPFB );
}

inline void _Requests::waitsome( int &count, int *indices, 
    MPI_Status *statuses )
{
    ErrMPI::check(
        MPI_Waitsome(_vals.size(), _vals.data(), &count, indices, statuses),
            emFLPFB );
}

inline void _Requests::testsome( int &count, int *indices, 
    MPI_Status *statuses )
{
    ErrMPI::check(
        MPI_Testsome(_vals.size(), _vals.data(), &count, indices, statuses),
            emFLPFB );
}

inline void _Requests::cancel() { 
    cancel(0); 
}

inline void _Requests::cancel(int i) {
    ErrMPI::check(MPI_Cancel(&_vals[i]), emFLPFB, "  ... index=", i, '\n');
}

inline void _Requests::_destroy( mpi_t &val, int &state ) noexcept {
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

inline void _Requests::_destroy_self() noexcept {
    while( !_vals.empty() ){
        _destroy( _vals.back(), _states.back() );
        _vals.pop_back(); _states.pop_back();
    }
}

} // namespace HIPP::MPI

#endif	//_HIPPMPI_MPI_RAW_REQUESTS_H_