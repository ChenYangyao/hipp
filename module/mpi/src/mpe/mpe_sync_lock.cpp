#include "mpe_sync_lock.h"
namespace HIPP{
namespace MPI{

namespace _mpe_sync_lock_helper{
SpinLockGuard::SpinLockGuard(lock_t &lock, int lock_id)
:_lock(&lock), _lock_id(lock_id)
{}
SpinLockGuard::SpinLockGuard(SpinLockGuard &&that) noexcept
:_lock(that._lock), _lock_id(that._lock_id)
{
    that._lock_id = -1;
}
SpinLockGuard & SpinLockGuard::operator=(SpinLockGuard &&that) noexcept{
    using std::swap;
    if(this == &that){
        swap(_lock, that._lock);
        swap(_lock_id, that._lock_id);
    }
    return *this;
}
SpinLockGuard::~SpinLockGuard(){
    if( _lock_id >= 0 ){
        _lock->unlock(_lock_id);
    }
}
SpinLockGuard::operator bool() const noexcept{
    return _lock_id >= 0;
}

void SpinLockGuard::unlock(){
    if( _lock_id >= 0 ){
        _lock->unlock(_lock_id);
    }else
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... already unlocked");
}
MutexGuard::MutexGuard(lock_t &lock, int lock_id)
: _lock(&lock), _lock_id(lock_id)
{}
MutexGuard::MutexGuard(MutexGuard &&that) noexcept
: _lock(that._lock), _lock_id(that._lock_id)
{
    that._lock_id = -1;
}
MutexGuard & MutexGuard::operator=(MutexGuard &&that) noexcept{
    using std::swap;
    if(this != &that){
        swap(_lock, that._lock);
        swap(_lock_id, that._lock_id);
    }
    return *this;
}
MutexGuard::~MutexGuard(){
    if(_lock_id >= 0)
        _lock->unlock(_lock_id);
}
void MutexGuard::unlock(){
    if(_lock_id >= 0){
        _lock->unlock(_lock_id);
    }else 
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... already unlocked");
}
MutexGuard::operator bool() const noexcept{
    return _lock_id >= 0;
}
} // namespace _mpe_sync_lock_helper

const Datatype & SpinLock::_value_mpi_t = *_TypeCvt<value_t>::datatype;

SpinLock::SpinLock(const Comm &comm, int n_locks)
: _win(nullptr), _n_locks(n_locks), _n_procs(comm.size())
{
    if( _n_locks <= 0 )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... no. of locks ", _n_locks, " is non-positive\n");
    if( _n_procs <= 0 )
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB,
            "  ... communicator have ", _n_procs, " processes\n"); 

    int n_each = (_n_locks+_n_procs-1)/_n_procs;
    size_t buf_size = (size_t)n_each * sizeof(value_t);
    value_t *base_ptr = (value_t *)alloc_mem(buf_size);
    std::fill_n(base_ptr, n_each, value_t(0));

    auto info = Info::create();
    info.set("accumulate_ordering", "none");
    try{
        _win = comm.win_create(base_ptr, buf_size, sizeof(value_t), info);
    }catch( ... ){
        free_mem(base_ptr);
        ErrMPI::print_err(emFLPFB, "  ... RMA window creation failed\n");
        throw;
    }
}
SpinLock::SpinLock(SpinLock &&that) noexcept 
: _win(that._win), _n_locks(that._n_locks), _n_procs(that._n_procs)
{
    that._win = Win::nullval();
}
SpinLock & SpinLock::operator=(SpinLock &&that) noexcept{
    if(this != &that){
        this->~SpinLock();
        _win = that._win;
        _n_locks = that._n_locks;
        _n_procs = that._n_procs;
        that._win = Win::nullval();
    }
    return *this;
}
SpinLock::~SpinLock() noexcept{
    if( !_win.is_null() ){
        void *base_ptr = _win.get_base();
        _win.free();
        free_mem(base_ptr);
    }
}
void SpinLock::lock(int lock_id){
    _chk_lock_id(lock_id);
    int disp = lock_id / _n_procs, target_rank = lock_id % _n_procs;
    value_t value=1, one=1, comp=0;
    auto g = _win.lock_g(_win.LOCK_SHARED, target_rank);
    while(value){
        _win.compare_and_swap(target_rank, value, comp, one, disp);
        _win.flush(target_rank);
    }
}
bool SpinLock::try_lock(int lock_id) {
    _chk_lock_id(lock_id);
    int disp = lock_id / _n_procs, target_rank = lock_id % _n_procs;
    value_t value=1, one=1, comp=0;
    {
        auto g = _win.lock_g(_win.LOCK_SHARED, target_rank);
        _win.compare_and_swap(target_rank, value, comp, one, disp);
    }
    return value == 0;
}
void SpinLock::unlock(int lock_id){
    _chk_lock_id(lock_id);
    int disp = lock_id / _n_procs, target_rank = lock_id % _n_procs;
    value_t value = 0;
    {
        auto g = _win.lock_g(_win.LOCK_SHARED, target_rank);
        _win.accumulate(target_rank, REPLACE, {&value, 1, _value_mpi_t}, disp);
    }
}
auto SpinLock::lock_g(int lock_id) -> guard_t {
    lock(lock_id);
    return guard_t(*this, lock_id);
}
auto SpinLock::try_lock_g(int lock_id) -> guard_t {
    bool is_locked = try_lock(lock_id);
    return guard_t(*this, is_locked?lock_id:-1);
}

void SpinLock::_chk_lock_id(int lock_id){
    if( lock_id >= _n_locks || lock_id < 0 )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... lock id ", lock_id, " not valid (min=", 0, 
            ", max=", _n_locks-1, ")\n");
}

const Datatype & Mutex::_value_mpi_t = *_TypeCvt<value_t>::datatype;

Mutex::Mutex(const Comm &comm, int n_locks)
:_win(nullptr), _n_locks(n_locks), _n_procs(comm.size()), _rank(comm.rank())
{
    if( _n_locks <= 0 )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... no. of locks ", _n_locks, " is non-positive\n");
    if( _n_procs <= 0 )
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB,
            "  ... communicator have ", _n_procs, " processes\n"); 

    int n_each = (_n_locks+_n_procs-1)/_n_procs;
    int n_link = NUSE * _n_locks;
    size_t buf_size = (size_t)(n_each+n_link) * sizeof(value_t);
    value_t *base_ptr = (value_t *)alloc_mem(buf_size);
    std::fill_n(base_ptr, n_each+n_link, value_t(-1));

    auto info = Info::create();
    info.set("accumulate_ordering", "none");
    try{
        _win = comm.win_create(base_ptr, buf_size, sizeof(value_t), info);
    }catch( ... ){
        free_mem(base_ptr);
        ErrMPI::print_err(emFLPFB, "  ... RMA window creation failed\n");
        throw;
    }
    _base_ptr = base_ptr;
}
Mutex::Mutex(Mutex &&that) noexcept 
:_win(that._win), _n_locks(that._n_locks), _n_procs(that._n_procs), 
_rank(that._rank), _base_ptr(that._base_ptr)
{
    that._win = Win::nullval();
}
Mutex & Mutex::operator=(Mutex &&that) noexcept {
    if(this != &that){
        this->~Mutex();
        _win = that._win;
        _n_locks = that._n_locks;
        _n_procs = that._n_procs;
        _rank = that._rank;
        _base_ptr = that._base_ptr;
        that._win = Win::nullval();
    }
    return *this;
}
Mutex::~Mutex() noexcept {
    if( !_win.is_null() ){
        _win.free();
        free_mem(_base_ptr);
    }
}
void Mutex::lock(int lock_id) {
    _chk_lock_id(lock_id);
    int target_rank_tail = lock_id % _n_procs, 
        disp_tail = lock_id / _n_procs + NUSE*_n_locks,
        disp_localflag = NUSE*lock_id + LOCALFLAG,
        disp_nextrank = NUSE*lock_id + NEXTRANK;
    value_t tail, localflag;
    const value_t flag_wait=0;
    {
        auto g = _win.lock_all_g();
        _win.fetch_and_op(target_rank_tail, REPLACE, tail, _rank, disp_tail);
        _win.flush(target_rank_tail);
        if( tail == -1 ) return;
        
        _win.accumulate(_rank, REPLACE, 
            {&flag_wait, 1, _value_mpi_t}, disp_localflag);
        _win.flush(_rank);
        _win.accumulate(tail, REPLACE, 
            {&_rank, 1, _value_mpi_t}, disp_nextrank);
        do{
            _win.flush(_rank);
            localflag = _base_ptr[disp_localflag];
        }while( localflag == flag_wait);
    }
}
bool Mutex::try_lock(int lock_id){
    _chk_lock_id(lock_id);
    int target_rank_tail = lock_id % _n_procs,
        disp_tail = lock_id / _n_procs + NUSE*_n_locks;
    value_t tail;
    {
        auto g = _win.lock_g(_win.LOCK_EXCLUSIVE, target_rank_tail);
        _win.get(target_rank_tail, {&tail, 1, _value_mpi_t}, disp_tail);
        _win.flush(target_rank_tail);
        if( tail != -1 )
            return false;
        _win.put(target_rank_tail, {&_rank, 1, _value_mpi_t}, disp_tail);
    }
    return true;
}
void Mutex::unlock(int lock_id){
    int target_rank_tail = lock_id % _n_procs,
        disp_tail = lock_id / _n_procs + NUSE*_n_locks,
        disp_nextrank = NUSE*lock_id + NEXTRANK,
        disp_localflag = NUSE*lock_id + LOCALFLAG;
    value_t tail, nextrank; 
    const value_t nullrank=-1, flag_pass=-1, nullnext=-1;
    {
        auto g = _win.lock_all_g();
        _win.compare_and_swap(target_rank_tail, tail, 
            _rank, nullrank, disp_tail);
        _win.flush(target_rank_tail);
        if( tail == _rank )
            return;
        do{
            _win.flush(_rank);
            nextrank = _base_ptr[disp_nextrank];
        }while( nextrank == nullnext );
        _win.accumulate(nextrank, REPLACE, 
            {&flag_pass, 1, _value_mpi_t}, disp_localflag);
        _win.accumulate(_rank, REPLACE, 
            {&nullnext, 1, _value_mpi_t}, disp_nextrank);
    }
}

auto Mutex::lock_g(int lock_id) -> guard_t {
    lock(lock_id);
    return {*this, lock_id};
}
auto Mutex::try_lock_g(int lock_id) -> guard_t {
    bool is_locked = try_lock(lock_id);
    return {*this, is_locked?lock_id:-1};
}
void Mutex::_chk_lock_id(int lock_id){
    if( lock_id >= _n_locks || lock_id < 0 )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... lock id ", lock_id, " not valid (min=", 0, 
            ", max=", _n_locks-1, ")\n");
}


} // namespace MPI
} // namespace HIPP