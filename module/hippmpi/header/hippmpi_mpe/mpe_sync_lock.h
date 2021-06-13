#ifndef _HIPPMPI_MPE_SYNC_LOCK_H_
#define _HIPPMPI_MPE_SYNC_LOCK_H_
#include "mpe_base.h"
namespace HIPP{
namespace MPI{

namespace _mpe_sync_lock_helper{
class SpinLockGuard;
class MutexGuard;
} // namespace _mpe_sync_lock_helper

class SpinLock{
public:
    typedef _mpe_sync_lock_helper::SpinLockGuard guard_t;

    SpinLock(const Comm &comm, int n_locks=1);
    SpinLock(const SpinLock &) = delete;
    SpinLock & operator=(const SpinLock &) = delete;
    SpinLock(SpinLock &&) noexcept;
    SpinLock & operator=(SpinLock &&) noexcept;
    ~SpinLock() noexcept;

    void lock(int lock_id=0);
    bool try_lock(int lock_id=0);
    void unlock(int lock_id=0);

    guard_t lock_g(int lock_id=0);
    guard_t try_lock_g(int lock_id=0);
protected:
    typedef int value_t;
    static const Datatype & _value_mpi_t;

    Win _win;
    int _n_locks, _n_procs;
    void _chk_lock_id(int lock_id);
};


class Mutex {
public:
    typedef _mpe_sync_lock_helper::MutexGuard guard_t;

    Mutex(const Comm &comm, int n_locks=1);
    Mutex(const Mutex &) = delete;
    Mutex & operator=(const Mutex &) = delete;
    Mutex(Mutex &&) noexcept;
    Mutex & operator=(Mutex &&) noexcept;
    ~Mutex() noexcept;

    void lock(int lock_id=0);
    bool try_lock(int lock_id=0);
    void unlock(int lock_id=0);

    guard_t lock_g(int lock_id=0);
    guard_t try_lock_g(int lock_id=0);
protected:
    typedef int value_t;
    static const Datatype & _value_mpi_t;
    enum : int {NUSE=2, NEXTRANK=0, LOCALFLAG=1};

    Win _win;
    int _n_locks, _n_procs; 
    value_t _rank;
    value_t *_base_ptr;
    void _chk_lock_id(int lock_id);
};

namespace _mpe_sync_lock_helper{
class SpinLockGuard {
public:
    typedef SpinLock lock_t;
    SpinLockGuard(lock_t &lock, int lock_id);
    SpinLockGuard(const SpinLockGuard &) = delete;
    SpinLockGuard & operator=(const SpinLockGuard &) = delete;
    SpinLockGuard(SpinLockGuard &&) noexcept;
    SpinLockGuard & operator=(SpinLockGuard &&) noexcept;
    ~SpinLockGuard();

    void unlock();
    explicit operator bool() const noexcept;
private:
    lock_t *_lock; 
    int _lock_id;
};
class MutexGuard {
public:
    typedef Mutex lock_t;
    MutexGuard(lock_t &lock, int lock_id);
    MutexGuard(const MutexGuard &) = delete;
    MutexGuard & operator=(const MutexGuard &) = delete;
    MutexGuard(MutexGuard &&) noexcept;
    MutexGuard & operator=(MutexGuard &&) noexcept;
    ~MutexGuard();

    void unlock();
    explicit operator bool() const noexcept;
private:
    lock_t *_lock; 
    int _lock_id;
};
} // namespace _mpe_sync_lock_helper



} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPE_SYNC_LOCK_H_