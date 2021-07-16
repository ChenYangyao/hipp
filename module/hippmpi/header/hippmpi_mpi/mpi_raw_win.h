#ifndef _HIPPMPI_MPI_RAW_WIN_H_
#define _HIPPMPI_MPI_RAW_WIN_H_
#include "mpi_base.h"
#include "mpi_error.h"

namespace HIPP{
namespace MPI{

class _Win{
public:
    typedef MPI_Win mpi_t;
    typedef MPI_Win_copy_attr_function copy_attr_fn_t;
    typedef MPI_Win_delete_attr_function del_attr_fn_t;
    enum : int { stFREE=0x1 };

    _Win(mpi_t val, int state) noexcept;
    ~_Win() noexcept;
    _Win(const _Win &) = delete;
    _Win & operator=(const _Win &) = delete;
    _Win(_Win &&) noexcept;
    _Win & operator=(_Win &&) noexcept;
    static void free(mpi_t &win);

    mpi_t raw() const noexcept;
    bool is_null() const noexcept;
    void shared_query(int rank, aint_t *size, int *disp_unit, 
        void *base_ptr) const;
    bool get_attr(int keyval, void *attr_val) const;
    MPI_Group get_group()const;
    void set_info(MPI_Info info);
    MPI_Info get_info() const;

    static mpi_t nullval() noexcept;
    static mpi_t create(void *base, aint_t size, int disp_unit, MPI_Info info, 
        MPI_Comm comm);
    static mpi_t allocate(aint_t size, int disp_unit, MPI_Info info, 
        MPI_Comm comm, void *base_ptr);
    static mpi_t allocate_shared(aint_t size, int disp_unit, MPI_Info info, 
        MPI_Comm comm, void *base_ptr);
    static mpi_t create_dynamic(MPI_Info info, MPI_Comm comm);
    void attach(void *base, aint_t size);
    void detach(const void *base);

    void put(const void *origin_addr, int origin_count, 
        MPI_Datatype origin_dtype, 
        int target_rank, aint_t target_disp, int target_count, 
        MPI_Datatype target_dtype);
    void get(void *origin_addr, int origin_count, 
        MPI_Datatype origin_dtype, 
        int target_rank, aint_t target_disp, int target_count, 
        MPI_Datatype target_dtype);
    void accumulate(const void *origin_addr, int origin_count, 
        MPI_Datatype origin_dtype, 
        int target_rank, aint_t target_disp, int target_count, 
        MPI_Datatype target_dtype, MPI_Op op);
    void get_accumulate(const void *origin_addr, int origin_count, 
        MPI_Datatype origin_dtype, 
        void *result_addr, int result_count, MPI_Datatype result_dtype,
        int target_rank, aint_t target_disp, int target_count, 
        MPI_Datatype target_dtype, MPI_Op op);
    void fetch_and_op(const void *origin_addr, void *result_addr,
        MPI_Datatype dtype, int target_rank, aint_t target_disp, 
        MPI_Op op);
    void compare_and_swap(const void *origin_addr, const void *compare_addr, 
        void *result_addr, MPI_Datatype dtype, 
        int target_rank, aint_t target_disp);

    MPI_Request rput(const void *origin_addr, int origin_count, 
        MPI_Datatype origin_dtype, 
        int target_rank, aint_t target_disp, int target_count, 
        MPI_Datatype target_dtype);
    MPI_Request rget(void *origin_addr, int origin_count, 
        MPI_Datatype origin_dtype, 
        int target_rank, aint_t target_disp, int target_count, 
        MPI_Datatype target_dtype);
    MPI_Request raccumulate(const void *origin_addr, int origin_count, 
        MPI_Datatype origin_dtype, 
        int target_rank, aint_t target_disp, int target_count, 
        MPI_Datatype target_dtype, MPI_Op op);
    MPI_Request rget_accumulate(const void *origin_addr, int origin_count, 
        MPI_Datatype origin_dtype, 
        void *result_addr, int result_count, MPI_Datatype result_dtype,
        int target_rank, aint_t target_disp, int target_count, 
        MPI_Datatype target_dtype, MPI_Op op);

    void fence(int assert);

    void start(MPI_Group group, int assert);
    void complete();
    void post(MPI_Group group, int assert);
    void wait();
    int test();

    void lock(int lock_type, int rank, int assert);
    void unlock(int rank);
    void lock_all(int assert);
    void unlock_all();

    void flush(int rank);
    void flush_all();
    void flush_local(int rank);
    void flush_local_all();

    void sync();
protected:
    mpi_t _val;
    int _state;
};



inline _Win::_Win(mpi_t val, int state) noexcept
    :_val(val), _state(state){ }
inline _Win::~_Win() noexcept{
    if( _state != 0 && ! is_null() ){
        if( MPI_Win_free(&_val) != MPI_SUCCESS )
            ErrMPI::abort(1, emFLPFB);
    }
}
inline _Win::_Win(_Win &&that) noexcept
:_val(that._val), _state(that._state)
{
    that._val = nullval();
    that._state = 0;
}
inline _Win & _Win::operator=(_Win &&that) noexcept{
    using std::swap;
    if( &that != this ){
        swap(_val, that._val);
        swap(_state, that._state);
    }
    return *this;
}
inline void _Win::free(mpi_t &win){
    ErrMPI::check( MPI_Win_free(&win), emFLPFB );
}
inline _Win::mpi_t _Win::raw() const noexcept { return _val; }
inline bool _Win::is_null() const noexcept{
    return _val == nullval();
}
inline void _Win::shared_query(int rank, aint_t *size, int *disp_unit, 
    void *base_ptr) const{
    ErrMPI::check(MPI_Win_shared_query(_val, rank, size, disp_unit, base_ptr), 
        emFLPFB, "  ... query rank=", rank, '\n');
}
inline bool _Win::get_attr(int keyval, void *attr_val) const{
    int flag;
    ErrMPI::check(MPI_Win_get_attr(_val, keyval, attr_val, &flag), emFLPFB, 
        "  ... keyval=", keyval, '\n');
    return flag;
}
inline MPI_Group _Win::get_group()const{
    MPI_Group group;
    ErrMPI::check(MPI_Win_get_group(_val, &group), emFLPFB);
    return group;
}
inline void _Win::set_info(MPI_Info info){
    ErrMPI::check(MPI_Win_set_info(_val, info), emFLPFB);
}
inline MPI_Info _Win::get_info() const{
    MPI_Info info_used;
    ErrMPI::check(MPI_Win_get_info(_val, &info_used), emFLPFB);
    return info_used;
}
inline _Win::mpi_t _Win::nullval() noexcept{
    return MPI_WIN_NULL;
}
inline _Win::mpi_t _Win::create(void *base, aint_t size, 
    int disp_unit, MPI_Info info, 
    MPI_Comm comm)
{
    mpi_t win;
    ErrMPI::check(MPI_Win_create(base, size, disp_unit, info, comm, &win),
        emFLPFB, "  ... base=", base, ", size=",size, 
        ", disp_unit=", disp_unit, '\n');
    return win;
}
inline _Win::mpi_t _Win::allocate(aint_t size, int disp_unit, MPI_Info info, 
    MPI_Comm comm, void *base_ptr){
    mpi_t win; 
    ErrMPI::check(MPI_Win_allocate(size, disp_unit, info, comm, base_ptr, &win), 
        emFLPFB, "  ... size=",size, "disp_unit=",disp_unit);
    return win;
}
inline _Win::mpi_t _Win::allocate_shared(aint_t size, 
    int disp_unit, MPI_Info info, 
    MPI_Comm comm, void *base_ptr){
    mpi_t win;
    ErrMPI::check(MPI_Win_allocate_shared(size, disp_unit, 
        info, comm, base_ptr, &win), emFLPFB, "  ... size=",size, 
        "disp_unit=", disp_unit);
    return win;
}
inline _Win::mpi_t _Win::create_dynamic(MPI_Info info, MPI_Comm comm){
    mpi_t win;
    ErrMPI::check(MPI_Win_create_dynamic(info, comm, &win), emFLPFB);
    return win;
}
inline void _Win::attach(void *base, aint_t size){
    ErrMPI::check(MPI_Win_attach(_val, base, size), emFLPFB, 
        "  .... attach ", base, "(size=", size, ")\n");
}
inline void _Win::detach(const void *base){
    ErrMPI::check(MPI_Win_detach(_val, base), emFLPFB, "  ... detach ", base);
}
inline void _Win::put(const void *origin_addr, int origin_count, 
    MPI_Datatype origin_dtype, 
    int target_rank, aint_t target_disp, int target_count, 
    MPI_Datatype target_dtype){
    ErrMPI::check(MPI_Put(origin_addr, origin_count, origin_dtype, target_rank, 
        target_disp, target_count, target_dtype, _val), emFLPFB, 
        "  ... put (", origin_addr, ", ", origin_count, 
        ") to rank ",  target_rank, " (", target_disp, ", ", target_count, 
        ")\n");
}
inline void _Win::get(void *origin_addr, int origin_count, 
    MPI_Datatype origin_dtype, 
    int target_rank, aint_t target_disp, int target_count, 
    MPI_Datatype target_dtype){
    ErrMPI::check(MPI_Get(origin_addr, origin_count, origin_dtype, target_rank, 
    target_disp, target_count, target_dtype, _val), 
        emFLPFB, "  ... get (", origin_addr, ", ", origin_count, ") from rank ", 
        target_rank, " (", target_disp, ", ", target_count, ")\n");
}
inline void _Win::accumulate(const void *origin_addr, int origin_count, 
    MPI_Datatype origin_dtype, 
    int target_rank, aint_t target_disp, int target_count, 
    MPI_Datatype target_dtype, MPI_Op op){
    ErrMPI::check(MPI_Accumulate(origin_addr, origin_count, origin_dtype, 
        target_rank, target_disp, target_count, target_dtype, op, _val), 
        emFLPFB, "accumulate (", origin_addr, ", ", origin_count, ") to rank ", 
        target_rank, " (", target_disp, ", ", target_count, ")\n");
}
inline void _Win::get_accumulate(const void *origin_addr, int origin_count, 
    MPI_Datatype origin_dtype, 
    void *result_addr, int result_count, MPI_Datatype result_dtype,
    int target_rank, aint_t target_disp, int target_count, 
    MPI_Datatype target_dtype, MPI_Op op){
    ErrMPI::check(MPI_Get_accumulate(origin_addr, origin_count, origin_dtype,
        result_addr, result_count, result_dtype, target_rank, target_disp, 
        target_count, target_dtype, op, _val), emFLPFB, 
        "   ... get to (", result_addr, ", ", result_count, 
        ") and accumulate from (", origin_addr, ", ", origin_count, 
        ") to rank ", target_rank, " (", target_disp, ", ", target_count, 
        ")\n");
}
inline void _Win::fetch_and_op(const void *origin_addr, void *result_addr,
    MPI_Datatype dtype, int target_rank, aint_t target_disp, 
    MPI_Op op){
    ErrMPI::check(MPI_Fetch_and_op(origin_addr, result_addr, dtype, 
        target_rank, target_disp, op, _val), emFLPFB, " ... fetch to ", 
        result_addr, " and op from ", origin_addr, " to rank ", target_rank, 
        " (", target_disp, ")\n");
}
inline void _Win::compare_and_swap(const void *origin_addr, 
    const void *compare_addr, 
    void *result_addr, MPI_Datatype dtype, 
    int target_rank, aint_t target_disp){
    ErrMPI::check(MPI_Compare_and_swap(origin_addr, compare_addr, 
        result_addr, dtype, target_rank, target_disp, _val), emFLPFB, 
        "  ... compare ", compare_addr, " and swap (src=", origin_addr, 
        ", result=", result_addr, ") to rank ", target_rank, "(", target_disp, 
        ")\n" );
}
inline MPI_Request _Win::rput(const void *origin_addr, int origin_count, 
    MPI_Datatype origin_dtype, 
    int target_rank, aint_t target_disp, int target_count, 
    MPI_Datatype target_dtype)
{
    MPI_Request req;
    ErrMPI::check(MPI_Rput(origin_addr, origin_count, origin_dtype, 
        target_rank, target_disp, target_count, target_dtype, _val, &req),
        emFLPFB, "  ... rput from (", origin_addr, ", ", origin_count, 
        ") to rank", target_rank, " (", target_count, ", ", target_disp, ")\n");
    return req;
}
inline MPI_Request _Win::rget(void *origin_addr, int origin_count, 
    MPI_Datatype origin_dtype, 
    int target_rank, aint_t target_disp, int target_count, 
    MPI_Datatype target_dtype)
{
    MPI_Request req;
    ErrMPI::check(MPI_Rget(origin_addr, origin_count, origin_dtype,
        target_rank, target_disp, target_count, target_dtype, _val, &req),
        emFLPFB, "  ... rget to (", origin_addr, ", ", origin_count, 
        ") from rank ", target_rank, " (", target_disp, ", ", target_count, 
        ")\n");
    return req;
}
inline MPI_Request _Win::raccumulate(const void *origin_addr, int origin_count, 
    MPI_Datatype origin_dtype, 
    int target_rank, aint_t target_disp, int target_count, 
    MPI_Datatype target_dtype, MPI_Op op)
{
    MPI_Request req;
    ErrMPI::check(MPI_Raccumulate(origin_addr, origin_count, origin_dtype, 
        target_rank, target_disp, target_count, target_dtype, op, _val, &req), 
        emFLPFB, "raccumulate (", origin_addr, ", ", origin_count, ") to rank ", 
        target_rank, " (", target_disp, ", ", target_count, ")\n");    
    return req;
}
inline MPI_Request _Win::rget_accumulate(const void *origin_addr, int origin_count, 
    MPI_Datatype origin_dtype, 
    void *result_addr, int result_count, MPI_Datatype result_dtype,
    int target_rank, aint_t target_disp, int target_count, 
    MPI_Datatype target_dtype, MPI_Op op)
{
    MPI_Request req;
    ErrMPI::check(MPI_Rget_accumulate(origin_addr, origin_count, origin_dtype,
        result_addr, result_count, result_dtype, target_rank, target_disp, 
        target_count, target_dtype, op, _val, &req), emFLPFB, 
        "   ... rget to (", result_addr, ", ", result_count, 
        ") and accumulate from (", origin_addr, ", ", origin_count, 
        ") to rank ", target_rank, " (", target_disp, ", ", target_count, 
        ")\n");
    return req;
}
inline void _Win::fence(int assert){
    ErrMPI::check(MPI_Win_fence(assert, _val), emFLPFB);
}
inline void _Win::start(MPI_Group group, int assert){
    ErrMPI::check(MPI_Win_start(group, assert, _val), emFLPFB);
}
inline void _Win::complete(){
    ErrMPI::check(MPI_Win_complete(_val), emFLPFB);
}
inline void _Win::post(MPI_Group group, int assert){
    ErrMPI::check(MPI_Win_post(group, assert, _val), emFLPFB);
}
inline void _Win::wait(){
    ErrMPI::check(MPI_Win_wait(_val), emFLPFB);
}
inline int _Win::test(){
    int flag;
    ErrMPI::check(MPI_Win_test(_val, &flag), emFLPFB);
    return flag;
}
inline void _Win::lock(int lock_type, int rank, int assert){
    ErrMPI::check(MPI_Win_lock(lock_type, rank, assert, _val), emFLPFB);
}
inline void _Win::unlock(int rank){
    ErrMPI::check(MPI_Win_unlock(rank, _val), emFLPFB);
}
inline void _Win::lock_all(int assert){
    ErrMPI::check(MPI_Win_lock_all(assert, _val), emFLPFB);
}
inline void _Win::unlock_all(){
    ErrMPI::check(MPI_Win_unlock_all(_val), emFLPFB);
}
inline void _Win::flush(int rank){
    ErrMPI::check(MPI_Win_flush(rank, _val), emFLPFB);
}
inline void _Win::flush_all(){
    ErrMPI::check(MPI_Win_flush_all(_val), emFLPFB);
}
inline void _Win::flush_local(int rank){
    ErrMPI::check(MPI_Win_flush_local(rank, _val), emFLPFB);
}
inline void _Win::flush_local_all(){
    ErrMPI::check(MPI_Win_flush_local_all(_val), emFLPFB);
}
inline void _Win::sync(){
    ErrMPI::check(MPI_Win_sync(_val), emFLPFB);
}

} // namespace MPI
} // namespace HIPP




#endif	//_HIPPMPI_MPI_RAW_WIN_H_