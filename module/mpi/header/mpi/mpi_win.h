/**
 * creat: Yangyao CHEN, 2020/10/11
 *      [write   ] Win - the high-level RMA window interface for MPI system
 */ 

#ifndef _HIPPMPI_MPI_WIN_H_
#define _HIPPMPI_MPI_WIN_H_
#include "mpi_raw_win.h"
#include "mpi_obj_base.h"
#include "mpi_datapacket.h"
#include "mpi_op.h"
#include "mpi_info.h"
#include "mpi_group.h"
#include "mpi_requests.h"
namespace HIPP{
namespace MPI{
    
namespace _mpi_win_helper{
class SyncGuard;
} // namespace _mpi_win_helper
class Comm;

/**
 * the high-level RMA window interface for MPI system
 * 
 * Win represents a RMA window object (internally, MPI_Win). It can be created
 * by, e.g., Comm::win_create().
 * 
 * Win can perform RMA operation by, e.g., put, get, accumulate methods, and 
 * perform synchronization by, e.g., fence (for active target access), 
 * lock/unlock (for passive target access).
 */
class Win: public MPIObj<_Win>{
public:
    typedef MPIObj<_Win> _obj_base_t;
    using _obj_base_t::_obj_base_t;
    typedef _mpi_win_helper::SyncGuard sync_guard_t;

    enum : int { LOCK_SHARED=MPI_LOCK_SHARED, 
        LOCK_EXCLUSIVE=MPI_LOCK_EXCLUSIVE };
    enum : int {
        MODE_NOSTORE=MPI_MODE_NOSTORE,
        MODE_NOPUT=MPI_MODE_NOPUT,
        MODE_NOPRECEDE=MPI_MODE_NOPRECEDE,
        MODE_NOSECCEED=MPI_MODE_NOSUCCEED,
        MODE_NOCHECK=MPI_MODE_NOCHECK
    };
    enum : int {
        BASE=MPI_WIN_BASE, 
        SIZE=MPI_WIN_SIZE,
        DISP_UNIT=MPI_WIN_DISP_UNIT,
        CREATE_FLAVOR=MPI_WIN_CREATE_FLAVOR,
        MODEL=MPI_WIN_MODEL
    };
    enum : int {
        UNIFIED=MPI_WIN_UNIFIED, SEPARATE=MPI_WIN_SEPARATE 
    };
    enum : int {
        FLAVOR_CREATE=MPI_WIN_FLAVOR_CREATE, 
        FLAVOR_ALLOCATE=MPI_WIN_FLAVOR_ALLOCATE,
        FLAVOR_DYNAMIC=MPI_WIN_FLAVOR_DYNAMIC,
        FLAVOR_SHARED=MPI_WIN_FLAVOR_SHARED
    };

    ostream &info(ostream &os = cout, int fmt_cntl = 1)const;
    friend ostream & operator<<(ostream &os, const Win &win);

    /**
     * free the current window object, and set it to a null value as returned by
     * nullval().
     * free() can be called at any time and multiple times.
     */
    void free() noexcept;
    /**
     * query the information of the instance.
     * is_null() - whether this is a null window object (internally, 
     *  MPI_WIN_NULL).
     * 
     * shared_query() - for window created with shared memory, we can query
     * the base pointer (return value), its size and displacement unit, given
     * the rank of the remote window.
     * 
     * get_attr() - get cache attribute, including the predefined values.
     * The predefined attribute can also be access by get_base(), get_size(),
     * get_disp_unit(), get_create_flavor(), and get_model().
     * 
     * get_group() - return the processes group associated with this window.
     * 
     * get_info() and set_info()- get used hints and set new hints.
     */
    bool is_null() const;
    void * shared_query(int rank, aint_t &size, int &disp_unit) const;
    bool get_attr(int keyval, void * &attr_val) const;
    Group get_group() const;
    void * get_base() const;
    aint_t get_size() const;
    int get_disp_unit() const;
    int get_create_flavor() const;
    int get_model() const;
    void set_info(const Info &info);
    Info get_info();

    static Win nullval() noexcept;
    void attach(void *base, aint_t size);
    void detach(const void *base);

    void put(int target_rank, const Datapacket &origin_dpacket, 
        const Datapacket &target_dpacket);
    void put(int target_rank, const Datapacket &origin_dpacket, 
        aint_t target_disp);
        
    void get(int target_rank, const Datapacket &origin_dpacket, 
        const Datapacket &target_dpacket);
    void get(int target_rank, const Datapacket &origin_dpacket, 
        aint_t target_disp);

    void accumulate(int target_rank, const Oppacket &op, 
        const Datapacket &origin_dpacket, const Datapacket &target_dpacket);
    void accumulate(int target_rank, const Oppacket &op, 
        const Datapacket &origin_dpacket, aint_t target_disp);
    
    void get_accumulate(int target_rank, const Oppacket &op, 
        const Datapacket &result_dpacket, 
        const Datapacket &origin_dpacket, 
        const Datapacket &target_dpacket);
    void get_accumulate(int target_rank, const Oppacket &op, 
        const Datapacket &result_dpacket, const void *origin_addr, 
        aint_t target_disp);

    void fetch_and_op(int target_rank, const Oppacket &op, 
        const Datatype &dtype, 
        void *result_addr, const void *origin_addr, aint_t target_disp);
    template<typename T>
    void fetch_and_op(int target_rank, const Oppacket &op, 
        T &result, const T &origin, aint_t target_disp);

    void compare_and_swap(int target_rank, const Datatype &dtype, 
        void *result_addr, const void *compare_addr, 
        const void *origin_addr, aint_t target_disp);
    template<typename T>
    void compare_and_swap(int target_rank, T &result, const T &compare, 
        const T &origin, aint_t target_disp);
    
    Requests rput(int target_rank, const Datapacket &origin_dpacket, 
        const Datapacket &target_dpacket);
    Requests rput(int target_rank, const Datapacket &origin_dpacket, 
        aint_t target_disp);
        
    Requests rget(int target_rank, const Datapacket &origin_dpacket, 
        const Datapacket &target_dpacket);
    Requests rget(int target_rank, const Datapacket &origin_dpacket, 
        aint_t target_disp);

    Requests raccumulate(int target_rank, const Oppacket &op, 
        const Datapacket &origin_dpacket, const Datapacket &target_dpacket);
    Requests raccumulate(int target_rank, const Oppacket &op, 
        const Datapacket &origin_dpacket, aint_t target_disp);
    
    Requests rget_accumulate(int target_rank, const Oppacket &op, 
        const Datapacket &result_dpacket, 
        const Datapacket &origin_dpacket, 
        const Datapacket &target_dpacket);
    Requests rget_accumulate(int target_rank, const Oppacket &op, 
        const Datapacket &result_dpacket, const void *origin_addr, 
        aint_t target_disp);

    void fence(int assert=0);
    void start(const Group &group, int assert=0);
    void complete();
    void post(const Group &group, int assert=0);
    void wait();
    bool test();

    void lock(int lock_type, int rank, int assert=0);
    void unlock(int rank);
    void lock_all(int assert=0);
    void unlock_all();

    void flush(int rank);
    void flush_all();
    void flush_local(int rank);
    void flush_local_all();

    void sync();

    sync_guard_t fence_g(int begin_assert=0, int end_assert=0);
    sync_guard_t start_g(const Group &group, int assert=0);
    sync_guard_t post_g(const Group &group, int assert=0);
    sync_guard_t lock_g(int lock_type, int rank, int assert=0);
    sync_guard_t lock_all_g(int assert=0);
protected:
    static Win _from_raw(mpi_t obj, int state) noexcept;
    friend class Comm;
};

namespace _mpi_win_helper{
class SyncGuard {
public:
    enum: int { syncOVER=0,
        syncFENCE=1, syncSTART=2, syncPOST=3, 
        syncLOCK=4, syncLOCKALL=5 };
    SyncGuard(Win &win, int sync_type, int rank, int assert);
    SyncGuard(const SyncGuard &) = delete;
    SyncGuard & operator=(const SyncGuard &) = delete;
    SyncGuard(SyncGuard &&) noexcept;
    SyncGuard & operator=(SyncGuard &&)noexcept;
    ~SyncGuard();
    void release();
private:
    Win *_win;
    int _sync_type;
    int _rank; 
    int _assert;
};
} // namespace _mpi_win_helper

inline ostream & operator<<(ostream &os, const Win &win){
    return win.info(os);    
}
inline Win Win::_from_raw(mpi_t obj, int state) noexcept{
    return Win(std::make_shared<_obj_raw_t>(obj, state));
}

template<typename T>
void Win::fetch_and_op(int target_rank, const Oppacket &op, 
    T &result, const T &origin, aint_t target_disp){
    fetch_and_op(target_rank, op, *_TypeCvt<T>::datatype, 
        &result, &origin, target_disp);
}
template<typename T>
void Win::compare_and_swap(int target_rank, T &result, const T &compare, 
    const T &origin, aint_t target_disp){
    compare_and_swap(target_rank, *_TypeCvt<T>::datatype, &result, &compare, 
        &origin, target_disp);
}



} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_WIN_H_