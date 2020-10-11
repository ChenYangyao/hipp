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
            "  ... already release");
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
    _win = comm.win_create(base_ptr, buf_size, sizeof(value_t), info);
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
    auto g = _win.lock_g(_win.LOCK_SHARED, target_rank);
    value_t value = 0;
    _win.accumulate(target_rank, REPLACE, {&value, 1, _value_mpi_t}, disp);
}
SpinLock::guard_t SpinLock::lock_g(int lock_id){
    lock(lock_id);
    return guard_t(*this, lock_id);
}
SpinLock::guard_t SpinLock::try_lock_g(int lock_id){
    bool is_locked = try_lock(lock_id);
    return guard_t(*this, is_locked?lock_id:-1);
}

void SpinLock::_chk_lock_id(int lock_id){
    if( lock_id >= _n_locks || lock_id < 0 )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... lock id ", lock_id, " not valid (min=", 0, 
            ", max=", _n_locks-1, ")\n");
}




} // namespace MPI
} // namespace HIPP