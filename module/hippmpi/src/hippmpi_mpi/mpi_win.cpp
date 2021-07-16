#include <mpi_win.h>

namespace HIPP{
namespace MPI{
    
namespace _mpi_win_helper{
SyncGuard::SyncGuard(Win &win, int sync_type, int rank, int assert)
:_win(&win), _sync_type(sync_type), _rank(rank), _assert(assert)
{}
SyncGuard::SyncGuard(SyncGuard &&that) noexcept
:SyncGuard(*that._win, that._sync_type, that._rank, that._assert)
{
    that._sync_type = syncOVER;
}
SyncGuard & SyncGuard::operator=(SyncGuard &&that)noexcept{
    using std::swap;
    if(this != &that){
        swap(_win, that._win);
        swap(_sync_type, that._sync_type);
        swap(_rank, that._rank);
        swap(_assert, that._assert);
    }
    return *this;
}
SyncGuard::~SyncGuard(){
    if( _sync_type != syncOVER )
        release();
}
void SyncGuard::release(){
    switch (_sync_type){
    case syncFENCE:
        _win->fence(_assert);
        break;
    case syncSTART:
        _win->complete();
        break;
    case syncPOST:
        _win->wait();
        break;
    case syncLOCK:
        _win->unlock(_rank);
        break;
    case syncLOCKALL:
        _win->unlock_all();
        break;
    default:
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... already release");
    }
    _sync_type = syncOVER;
}
} // namespace _mpi_win_helper

ostream & Win::info(ostream &os, int fmt_cntl)const{
    string model, flavor;
    if( !is_null() ){
        int _model = get_model(), _flavor = get_create_flavor();
        if( _model == UNIFIED ) model = "UNIFIED";
        else if( _model == SEPARATE ) model = "SEPARATE";
        else model = "OTHER";
        
        if(_flavor==FLAVOR_CREATE) flavor="CREATE";
        else if(_flavor==FLAVOR_SHARED) flavor="SHARED";
        else if(_flavor==FLAVOR_DYNAMIC) flavor="DYNAMIC";
        else if(_flavor==FLAVOR_ALLOCATE) flavor="ALLOCATE";
        else flavor="OTHER";
    }
    if(fmt_cntl == 0){
        prt(os, HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI::Win));
        if( is_null() ) prt(os, "Null");
        else{
            prt(os, "base: ", get_base(), ", size: ", get_size(), 
                ", disp_unit: ", get_disp_unit(), ", model: ", model, 
                ", create flavor: ", flavor);
        }
    }else if(fmt_cntl >=1){
        prt(os, HIPPCNTL_CLASS_INFO(HIPP::MPI::Win));
        if( is_null() ) prt(os, " Null") << endl;
        else{
            prt(os, "  Process group\n    ");
            get_group().info(os, 0) << '\n';
            prt(os, "  Memory",
                "\n    base:          ", get_base(),
                "\n    size:          ", get_size(), 
                "\n    disp_unit:     ", get_disp_unit(), 
                "\n    model:         ", model, 
                "\n    create flavor: ", flavor) << endl;
        }
    }
    return os;
}
void Win::free() noexcept{
    *this = nullval();
}
bool Win::is_null() const{
    return _obj_ptr->is_null();
}
void * Win::shared_query(int rank, aint_t &size, int &disp_unit) const{
    void *base_ptr;
    _obj_ptr->shared_query(rank, &size, &disp_unit, &base_ptr);
    return base_ptr;
}
bool Win::get_attr(int keyval, void * &attr_val) const{
    return _obj_ptr->get_attr(keyval, &attr_val);
}
Group Win::get_group() const{
    auto obj = _obj_ptr->get_group();
    return Group( std::make_shared<Group::_obj_raw_t>(obj, 1) );
}
void * Win::get_base() const{
    void *base = NULL;
    get_attr(BASE, base);
    return base;
}
aint_t Win::get_size() const{
    void *psize=NULL;
    if( !get_attr(SIZE, psize) || !psize )
        ErrRuntime::throw_(ErrRuntime::eDEFAULT, emFLPFB, 
        "  ... size attribute is not set");
    return *reinterpret_cast<aint_t *>(psize);
}
int Win::get_disp_unit() const{
    void *pdisp_unit;
    if( !get_attr(DISP_UNIT, pdisp_unit) || !pdisp_unit )
        ErrRuntime::throw_(ErrRuntime::eDEFAULT, emFLPFB, 
        "  ... disp_unit attribute is not set");
    return *reinterpret_cast<int *>(pdisp_unit);
}
int Win::get_create_flavor() const{
    void *pflavor;
    if( !get_attr(CREATE_FLAVOR, pflavor) || !pflavor )
        ErrRuntime::throw_(ErrRuntime::eDEFAULT, emFLPFB, 
        "  ... create_flavor attribute is not set");
    return *reinterpret_cast<int *>(pflavor);
}
int Win::get_model() const{
    void *pmodel;
    if( !get_attr(MODEL, pmodel) || !pmodel )
        ErrRuntime::throw_(ErrRuntime::eDEFAULT, emFLPFB, 
        "  ... model attribute is not set");
    return *reinterpret_cast<int *>(pmodel);
}
void Win::set_info(const Info &info){
    _obj_ptr->set_info(info.raw());
}
Info Win::get_info(){
    auto info = _obj_ptr->get_info();
    return Info::_from_raw(info, 1);
}
void Win::attach(void *base, aint_t size){
    _obj_ptr->attach(base, size);
}
void Win::detach(const void *base){
    _obj_ptr->detach(base);
}
Win Win::nullval() noexcept{
    return _from_raw(_obj_raw_t::nullval(), 0);
}
void Win::put(int target_rank, const Datapacket &origin_dpacket, 
    const Datapacket &target_dpacket){
    auto &o = origin_dpacket;
    auto &t = target_dpacket;
    _obj_ptr->put(o._buff, o._size, o._dtype.raw(), target_rank, 
        (aint_t)t._buff, t._size, t._dtype.raw());
}
void Win::put(int target_rank, const Datapacket &origin_dpacket, 
    aint_t target_disp){
    auto &o = origin_dpacket;
    auto dtype = o._dtype.raw();
    _obj_ptr->put(o._buff, o._size, dtype, target_rank, target_disp, 
        o._size, dtype);
}
void Win::get(int target_rank, const Datapacket &origin_dpacket, 
    const Datapacket &target_dpacket){
    auto &o = origin_dpacket;
    auto &t = target_dpacket;
    _obj_ptr->get(o._buff, o._size, o._dtype.raw(), target_rank, 
        (aint_t)t._buff, t._size, t._dtype.raw());
}
void Win::get(int target_rank, const Datapacket &origin_dpacket, 
    aint_t target_disp){
    auto &o = origin_dpacket;
    auto dtype = o._dtype.raw();
    _obj_ptr->get(o._buff, o._size, dtype, target_rank, 
        target_disp, o._size, dtype);
}
void Win::accumulate(int target_rank, const Oppacket &op, 
    const Datapacket &origin_dpacket, const Datapacket &target_dpacket){
    auto &o = origin_dpacket;
    auto &t = target_dpacket;
    _obj_ptr->accumulate(o._buff, o._size, o._dtype.raw(), target_rank, 
        (aint_t)t._buff, t._size, t._dtype.raw(), op._op.raw());
}
void Win::accumulate(int target_rank, const Oppacket &op, 
    const Datapacket &origin_dpacket, aint_t target_disp){
    auto &o = origin_dpacket;
    auto dtype = o._dtype.raw();
    _obj_ptr->accumulate(o._buff, o._size, dtype, target_rank, 
        target_disp, o._size, dtype, op._op.raw());
}
void Win::get_accumulate(int target_rank, const Oppacket &op, 
    const Datapacket &result_dpacket, 
    const Datapacket &origin_dpacket, 
    const Datapacket &target_dpacket){
    auto &r = result_dpacket;
    auto &o = origin_dpacket;
    auto &t = target_dpacket;
    _obj_ptr->get_accumulate(
        o._buff, o._size, o._dtype.raw(), 
        r._buff, r._size, r._dtype.raw(),
        target_rank,
        (aint_t)t._buff, t._size, t._dtype.raw(), op._op.raw());
}
void Win::get_accumulate(int target_rank, const Oppacket &op, 
    const Datapacket &result_dpacket, const void *origin_addr, 
    aint_t target_disp){
    auto &r = result_dpacket;
    auto dtype = r._dtype.raw();
    _obj_ptr->get_accumulate(
        origin_addr, r._size, dtype, 
        r._buff, r._size, dtype,
        target_rank,
        target_disp, r._size, dtype, op._op.raw());
}
void Win::fetch_and_op(int target_rank, const Oppacket &op, 
    const Datatype &dtype, 
    void *result_addr, const void *origin_addr, aint_t target_disp){
    _obj_ptr->fetch_and_op(origin_addr, result_addr, dtype.raw(), target_rank, 
        target_disp, op._op.raw());
}
void Win::compare_and_swap(int target_rank, const Datatype &dtype, 
    void *result_addr, const void *compare_addr, 
    const void *origin_addr, aint_t target_disp){
    _obj_ptr->compare_and_swap(origin_addr, compare_addr, result_addr, 
        dtype.raw(), target_rank, target_disp);
}
Requests Win::rput(int target_rank, const Datapacket &origin_dpacket, 
    const Datapacket &target_dpacket){
    auto &o = origin_dpacket;
    auto &t = target_dpacket;
    auto rq = _obj_ptr->rput(o._buff, o._size, o._dtype.raw(), target_rank, 
        (aint_t)t._buff, t._size, t._dtype.raw());
    return Requests::_from_raw(rq, 0);
}
Requests Win::rput(int target_rank, const Datapacket &origin_dpacket, 
    aint_t target_disp){
    auto &o = origin_dpacket;
    auto dtype = o._dtype.raw();
    auto rq = _obj_ptr->rput(o._buff, o._size, dtype, target_rank, target_disp, 
        o._size, dtype);
    return Requests::_from_raw(rq, 0);
}
Requests Win::rget(int target_rank, const Datapacket &origin_dpacket, 
    const Datapacket &target_dpacket){
    auto &o = origin_dpacket;
    auto &t = target_dpacket;
    auto rq = _obj_ptr->rget(o._buff, o._size, o._dtype.raw(), target_rank, 
        (aint_t)t._buff, t._size, t._dtype.raw());
    return Requests::_from_raw(rq, 0);
}
Requests Win::rget(int target_rank, const Datapacket &origin_dpacket, 
    aint_t target_disp){
        auto &o = origin_dpacket;
    auto dtype = o._dtype.raw();
    auto rq = _obj_ptr->rget(o._buff, o._size, dtype, target_rank, 
        target_disp, o._size, dtype);
    return Requests::_from_raw(rq, 0);
}
Requests Win::raccumulate(int target_rank, const Oppacket &op, 
    const Datapacket &origin_dpacket, const Datapacket &target_dpacket){
        auto &o = origin_dpacket;
    auto &t = target_dpacket;
    auto rq = _obj_ptr->raccumulate(o._buff, o._size, o._dtype.raw(), target_rank, 
        (aint_t)t._buff, t._size, t._dtype.raw(), op._op.raw());
    return Requests::_from_raw(rq, 0);
}
Requests Win::raccumulate(int target_rank, const Oppacket &op, 
    const Datapacket &origin_dpacket, aint_t target_disp){
    auto &o = origin_dpacket;
    auto dtype = o._dtype.raw();
    auto rq = _obj_ptr->raccumulate(o._buff, o._size, dtype, target_rank, 
        target_disp, o._size, dtype, op._op.raw());
    return Requests::_from_raw(rq, 0);
}
Requests Win::rget_accumulate(int target_rank, const Oppacket &op, 
    const Datapacket &result_dpacket, 
    const Datapacket &origin_dpacket, 
    const Datapacket &target_dpacket){
    auto &r = result_dpacket;
    auto &o = origin_dpacket;
    auto &t = target_dpacket;
    auto rq = _obj_ptr->rget_accumulate(
        o._buff, o._size, o._dtype.raw(), 
        r._buff, r._size, r._dtype.raw(),
        target_rank,
        (aint_t)t._buff, t._size, t._dtype.raw(), op._op.raw());
    return Requests::_from_raw(rq, 0);
}
Requests Win::rget_accumulate(int target_rank, const Oppacket &op, 
    const Datapacket &result_dpacket, const void *origin_addr, 
    aint_t target_disp){
    auto &r = result_dpacket;
    auto dtype = r._dtype.raw();
    auto rq = _obj_ptr->rget_accumulate(
        origin_addr, r._size, dtype, 
        r._buff, r._size, dtype,
        target_rank,
        target_disp, r._size, dtype, op._op.raw());    
    return Requests::_from_raw(rq, 0);
}
void Win::fence(int assert){
    _obj_ptr->fence(assert);
}
void Win::start(const Group &group, int assert){
    _obj_ptr->start(group.raw(), assert);
}
void Win::complete(){
    _obj_ptr->complete();
}
void Win::post(const Group &group, int assert){
    _obj_ptr->post(group.raw(), assert);
}
void Win::wait(){
    _obj_ptr->wait();
}
bool Win::test(){
    return _obj_ptr->test();
}
void Win::lock(int lock_type, int rank, int assert){
    _obj_ptr->lock(lock_type, rank, assert);
}
void Win::unlock(int rank){
    _obj_ptr->unlock(rank);
}
void Win::lock_all(int assert){
    _obj_ptr->lock_all(assert);
}
void Win::unlock_all(){
    _obj_ptr->unlock_all();
}
void Win::flush(int rank){
    _obj_ptr->flush(rank);
}
void Win::flush_all(){
    _obj_ptr->flush_all();
}
void Win::flush_local(int rank){
    _obj_ptr->flush_local(rank);
}
void Win::flush_local_all(){
    _obj_ptr->flush_local_all();
}
void Win::sync(){
    _obj_ptr->sync();
}
Win::sync_guard_t Win::fence_g(int begin_assert, int end_assert){
    fence(begin_assert);
    return {*this, sync_guard_t::syncFENCE, -1, end_assert};
}
Win::sync_guard_t Win::start_g(const Group &group, int assert){
    start(group, assert);
    return {*this, sync_guard_t::syncSTART, -1, 0};
}
Win::sync_guard_t Win::post_g(const Group &group, int assert){
    post(group, assert);
    return {*this, sync_guard_t::syncPOST, -1, 0};
}
Win::sync_guard_t Win::lock_g(int lock_type, int rank, int assert){
    lock(lock_type, rank, assert);
    return {*this, sync_guard_t::syncLOCK, rank, 0};
}
Win::sync_guard_t Win::lock_all_g(int assert){
    lock_all(assert);
    return {*this, sync_guard_t::syncLOCKALL, -1, 0};
}

} // namespace MPI
} // namespace HIPP
