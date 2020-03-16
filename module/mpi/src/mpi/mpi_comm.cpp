#include "mpi_comm.h"

namespace HIPP{
namespace MPI{

const Comm::copy_attr_fn_t 
    Comm::NULL_COPY_FN = Comm::_null_copy_fn,
    Comm::DUP_FN = Comm::_dup_fn;
const Comm::del_attr_fn_t Comm::NULL_DEL_FN = Comm::_null_del_fn;

std::unordered_map<int, Comm::_attr_extra_state_t *> Comm::_attr_extra_state; 

ostream & Comm::info( ostream &os, int fmt_cntl ) const{
    if( fmt_cntl == 0 ){
        prt(os, HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI::Comm));
        if( is_null() ) prt(os, "Null");
        else{
            prt(os, "size: ", size(), ", rank: ", rank());
            if( is_inter() ) prt(os, ", remote size: ", remote_size());
            prt(os, ", topology: ", _topostr( topo_test() ));
        } 
    }
    if( fmt_cntl >= 1 ){
        prt(os, HIPPCNTL_CLASS_INFO(HIPP::MPI::Comm));
        if( is_null() ) prt(os, "  Null") << endl;
        else{
            prt(os, "  Process group",
                "\n    rank/size:        ", rank(), '/', size());
            if( is_inter() )
                prt(os, "\n    remote size:      ", remote_size());
            prt(os, "\n  Topology: ", _topostr( topo_test() ));
            int topo = topo_test();
            if( topo == CART ){
                vector<int> dims, periods, coords;
                cart_get( dims, periods, coords );
                prt(os, "\n    ndims:            ", dims.size());
                if( dims.size() > 0 ){
                    prt(os, "\n    dims:             (");
                    prt_a(os, dims) << ')';
                    prt(os,  "\n    periods:          (");
                    prt_a(os, periods) << ')';
                    prt(os,  "\n    coords:           (");
                    prt_a(os, coords) << ')';
                }
            }
            os << endl;
        } 
    }
    return os;
}
void Comm::free() noexcept {
    *this = nullval();
}
int Comm::size() const { return _obj_ptr->size(); }
int Comm::rank() const { return _obj_ptr->rank(); }
bool Comm::is_null() const { return _obj_ptr->is_null(); }
bool Comm::is_inter() const{
    return _obj_ptr->is_inter();
}
int Comm::remote_size() const{
    return _obj_ptr->remote_size();
}
int Comm::create_keyval( copy_attr_fn_t copy_attr_fn,
    del_attr_fn_t del_attr_fn, void *extra_state){
    typedef MemObj<_attr_extra_state_t> mem_t;
    auto *ptr = mem_t::alloc( 1, emFLPFB );
    mem_t().construct( ptr, copy_attr_fn, del_attr_fn, extra_state );
    int keyval = KEYVAL_INVALID;
    try{
        keyval = 
            _obj_raw_t::create_keyval( &_copy_attr_fn, &_del_attr_fn, ptr );
        _attr_extra_state.emplace( keyval, ptr );
    }catch( const ErrMPI &e ){ 
        mem_t::dealloc( ptr ); throw; 
    }catch( ... ){
        ErrMPI::abort( 1, emFLPFB, "  ... run out of memory\n" );
    }
    return keyval;
}
void Comm::free_keyval( int &keyval ){
    auto it = _attr_extra_state.find( keyval );
    if( it == _attr_extra_state.end() )
        ErrLogic::throw_( ErrLogic::eDOMAIN, emFLPFB, 
            "  ... key value ", keyval, " dose not exist\n" );
    auto *ptr = it->second;
    MemObj<_attr_extra_state_t>::dealloc( ptr );
    _attr_extra_state.erase(it);
    _obj_raw_t::free_keyval(&keyval);
}
Comm & Comm::set_attr( int keyval, void *attr_val ){
    _obj_ptr->set_attr( keyval, attr_val );
    return *this;
}
bool Comm::get_attr( int keyval, void * &attr_val ) const{
    return _obj_ptr->get_attr(keyval, &attr_val);
}
Comm & Comm::del_attr( int keyval ){
    _obj_ptr->del_attr(keyval);
    return *this;
}

Comm Comm::split( int color, int key )const{
    mpi_t newcomm = _obj_ptr->split(color, key);
    int state = (newcomm == _obj_raw_t::nullval())?0:1;
    return _from_raw(newcomm, state );
}
Comm Comm::dup() const{
    mpi_t newcomm = _obj_ptr->dup();
    int state = (newcomm == _obj_raw_t::nullval())?0:1;
    return _from_raw( newcomm, state );
}
Comm Comm::create( const Group &group ) const{
    mpi_t newcomm = _obj_raw_t::create( raw(), group.raw() );
    int state = (newcomm == _obj_raw_t::nullval())?0:1;
    return _from_raw(newcomm, state);
}
Comm Comm::world() noexcept{
    return _from_raw( _obj_raw_t::world(), 0 );
}
Comm Comm::selfval() noexcept{
    return _from_raw( _obj_raw_t::selfval(), 0 );
}
Comm Comm::nullval() noexcept{
    return _from_raw( _obj_raw_t::nullval(), 0 );
}
Comm Comm::create_inter( int local_leader, const Comm &peer_comm, 
    int remote_leader, int tag ){
    return _from_raw(
        _obj_raw_t::create_inter( raw(), local_leader, 
            peer_comm.raw(), remote_leader, tag ), 1
    );
}
Comm Comm::merge_inter( int high ){
    return _from_raw( _obj_raw_t::merge_inter( raw(), high ), 1 );
}
const Group Comm::group() const{
    return const_cast<Comm*>(this)->group();
}
Group Comm::group(){
    auto obj = _obj_ptr->group();
    return Group( std::make_shared<Group::_obj_raw_t>(obj, 1) );
}
const Group Comm::remote_group() const{
    return const_cast<Comm*>(this)->remote_group();
}
Group Comm::remote_group(){
    auto obj = _obj_ptr->remote_group();
    return Group( std::make_shared<Group::_obj_raw_t>(obj, 1) );
}

Comm Comm::cart_create( const vector<int> &dims, 
    const vector<int> &periods, int reorder )const{
    return _from_raw( _obj_ptr->cart_create( dims.size(), 
        dims.data(), periods.data(), reorder ), 1 );
}
void Comm::dims_create( int nnodes, int ndims, vector<int> &dims ){
    _obj_raw_t::dims_create(nnodes, ndims, dims.data());
}
int Comm::topo_test()const{
    return _obj_ptr->topo_test();
}
int Comm::cartdim_get()const{
    return _obj_ptr->cartdim_get();
}
void Comm::cart_get( vector<int> &dims, vector<int> &periods, 
    vector<int> &coords )const{
    int ndims = cartdim_get();
    dims.resize( ndims ); periods.resize( ndims ); coords.resize( ndims );
    _obj_ptr->cart_get( ndims, dims.data(), periods.data(), coords.data() );
}
int Comm::cart_rank( const vector<int> &coords )const{
    return _obj_ptr->cart_rank( coords.data() );
}
vector<int> Comm::cart_coords( int rank )const{
    int ndims = cartdim_get();
    vector<int> coords( ndims );
    _obj_ptr->cart_coords( rank, ndims, coords.data() );
    return coords;
}
void Comm::cart_shift( int direction, int disp, 
    int &rank_src, int &rank_dest )const{
    _obj_ptr->cart_shift( direction, disp, &rank_src, &rank_dest );
}
Comm Comm::cart_sub( const vector<int> &remain_dims ){
    return _from_raw( _obj_ptr->cart_sub( remain_dims.data() ), 1 );
}

void Comm::barrier() const{
    _obj_ptr->barrier();
}
void Comm::bcast( void *buf, int count, const Datatype &dtype, int root) const{
    _obj_ptr->bcast(buf, count, dtype.raw(), root);
}
void Comm::gather( const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, int recvcount, const Datatype &recvtype, int root) const{
    _obj_ptr->gather(sendbuf, sendcount, sendtype.raw(), recvbuf,
        recvcount, recvtype.raw(), root);
}
void Comm::gatherv(
    const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    const Datatype &recvtype, int root ) const{
    _obj_ptr->gatherv( sendbuf, sendcount, sendtype.raw(), 
        recvbuf, recvcounts, displs, recvtype.raw(), root );
}
void Comm::scatter(
    const void *sendbuf, int sendcount, const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype, int root )const{
    _obj_ptr->scatter( sendbuf, sendcount, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw(), root );
}
void Comm::scatterv(
    const void *sendbuf, const int sendcounts[], const int displs[], 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype, int root) const{
    _obj_ptr->scatterv(sendbuf, sendcounts, displs, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw(), root);
}
void Comm::allgather( const void *sendbuf, int sendcount, 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype ) const{
    _obj_ptr->allgather( sendbuf, sendcount, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw() );
}
void Comm::allgatherv(
    const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    const Datatype &recvtype ) const{
    _obj_ptr->allgatherv(sendbuf, sendcount, sendtype.raw(),
        recvbuf, recvcounts, displs, recvtype.raw());
}
void Comm::alltoall( const void *sendbuf, int sendcount, 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype ) const{
    _obj_ptr->alltoall( sendbuf, sendcount, sendtype.raw(),
        recvbuf, recvcount, recvtype.raw() );
}
void Comm::alltoallv( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], const Datatype &sendtype,
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const Datatype &recvtype ) const{
    _obj_ptr->alltoallv(sendbuf, sendcounts, senddispls, sendtype.raw(),
    recvbuf, recvcounts, recvdispls, recvtype.raw());
}
void Comm::alltoallw( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], const Datatype::mpi_t sendtypes[],
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const Datatype::mpi_t recvtypes[] ) const{
    _obj_ptr->alltoallw(sendbuf, sendcounts, senddispls, sendtypes, 
        recvbuf, recvcounts, recvdispls, recvtypes);
}
void Comm::reduce( const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype, const Oppacket &op, int root ) const{
    _obj_ptr->reduce( sendbuf, recvbuf, count, 
        dtype.raw(), op._op.raw(), root );
}
void Comm::allreduce( const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype, const Oppacket &op ) const{
    _obj_ptr->allreduce( sendbuf, recvbuf, count, dtype.raw(), op._op.raw() );
}
void Comm::reduce_local( const void *inbuf, void *inoutbuf, int count, 
    const Datatype &dtype, const Oppacket &op ){
    _obj_raw_t::reduce_local( 
        inbuf, inoutbuf, count, dtype.raw(), op._op.raw() );
}
void Comm::reduce_scatter_block( const void *sendbuf, void *recvbuf, 
    int recvcount, const Datatype &dtype, const Oppacket &op ) const{
    _obj_ptr->reduce_scatter_block( sendbuf, recvbuf, recvcount, 
        dtype.raw(), op._op.raw() );
}
void Comm::reduce_scatter( const void *sendbuf, void *recvbuf, 
    const int recvcounts[], const Datatype &dtype, 
    const Oppacket &op )const{
    _obj_ptr->reduce_scatter( sendbuf, recvbuf, recvcounts, 
        dtype.raw(), op._op.raw() );
}
void Comm::scan( const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, const Oppacket &op ) const{
    _obj_ptr->scan( sendbuf, recvbuf, count, dtype.raw(), op._op.raw() );
}
void Comm::exscan( const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, const Oppacket &op ) const{
    _obj_ptr->exscan(sendbuf, recvbuf, count, dtype.raw(), op._op.raw());
}

Requests Comm::ibarrier() const{
    return Requests::_from_raw( _obj_ptr->ibarrier(), 0);
}
Requests Comm::ibcast( void *buf, int count, const Datatype &dtype, int root) const{
    return Requests::_from_raw( _obj_ptr->ibcast(buf, count, dtype.raw(), root), 0);
}
Requests Comm::igather( const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, int recvcount, const Datatype &recvtype, int root) const{
    return Requests::_from_raw( _obj_ptr->igather(sendbuf, sendcount, 
        sendtype.raw(), recvbuf,
        recvcount, recvtype.raw(), root), 0);
}
Requests Comm::igatherv(
    const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    const Datatype &recvtype, int root ) const{
    return Requests::_from_raw( _obj_ptr->igatherv( sendbuf, sendcount, 
        sendtype.raw(), 
        recvbuf, recvcounts, displs, recvtype.raw(), root ), 0);
}
Requests Comm::iscatter(
    const void *sendbuf, int sendcount, const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype, int root )const{
    return Requests::_from_raw( _obj_ptr->iscatter( sendbuf, sendcount, 
        sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw(), root ), 0);
}
Requests Comm::iscatterv(
    const void *sendbuf, const int sendcounts[], const int displs[], 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype, int root) const{
    return Requests::_from_raw( _obj_ptr->iscatterv(sendbuf, sendcounts, 
        displs, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw(), root), 0);
}
Requests Comm::iallgather( const void *sendbuf, int sendcount, 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype ) const{
    return Requests::_from_raw( _obj_ptr->iallgather( sendbuf, sendcount, 
        sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw() ), 0);
}
Requests Comm::iallgatherv(
    const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    const Datatype &recvtype ) const{
    return Requests::_from_raw( _obj_ptr->iallgatherv(sendbuf, sendcount, 
        sendtype.raw(),
        recvbuf, recvcounts, displs, recvtype.raw()), 0);
}
Requests Comm::ialltoall( const void *sendbuf, int sendcount, 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype ) const{
    return Requests::_from_raw( _obj_ptr->ialltoall( sendbuf, sendcount, 
        sendtype.raw(),
        recvbuf, recvcount, recvtype.raw() ), 0);
}
Requests Comm::ialltoallv( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], const Datatype &sendtype,
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const Datatype &recvtype ) const{
    return Requests::_from_raw( _obj_ptr->ialltoallv(sendbuf, sendcounts, 
        senddispls, sendtype.raw(),
        recvbuf, recvcounts, recvdispls, recvtype.raw()), 0);
}
Requests Comm::ialltoallw( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], const Datatype::mpi_t sendtypes[],
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const Datatype::mpi_t recvtypes[] ) const{
    return Requests::_from_raw( _obj_ptr->ialltoallw(sendbuf, sendcounts, 
        senddispls, sendtypes, 
        recvbuf, recvcounts, recvdispls, recvtypes), 0);
}
Requests Comm::ireduce( const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype, const Oppacket &op, int root ) const{
    return Requests::_from_raw( _obj_ptr->ireduce( sendbuf, recvbuf, count, 
        dtype.raw(), op._op.raw(), root ), 0);
}
Requests Comm::iallreduce( const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype, const Oppacket &op ) const{
    return Requests::_from_raw( _obj_ptr->iallreduce( sendbuf, recvbuf, count, 
        dtype.raw(), op._op.raw() ), 0);
}
Requests Comm::ireduce_scatter_block( const void *sendbuf, void *recvbuf, 
    int recvcount, const Datatype &dtype, const Oppacket &op ) const{
    return Requests::_from_raw( _obj_ptr->ireduce_scatter_block( 
        sendbuf, recvbuf, recvcount, 
        dtype.raw(), op._op.raw() ), 0);
}
Requests Comm::ireduce_scatter( const void *sendbuf, void *recvbuf, 
    const int recvcounts[], const Datatype &dtype, 
    const Oppacket &op )const{
    return Requests::_from_raw( _obj_ptr->ireduce_scatter( 
        sendbuf, recvbuf, recvcounts, 
        dtype.raw(), op._op.raw() ), 0);
}
Requests Comm::iscan( const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, const Oppacket &op ) const{
    return Requests::_from_raw( _obj_ptr->iscan( 
        sendbuf, recvbuf, count, dtype.raw(), op._op.raw() ), 0);
}
Requests Comm::iexscan( const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, const Oppacket &op ) const{
    return Requests::_from_raw( _obj_ptr->iexscan(
        sendbuf, recvbuf, count, dtype.raw(), op._op.raw()), 0);
}

string Comm::_topostr( int topo ){
    string ret;
    switch (topo){
    case UNDEFINED:
        ret = "undefined"; break;
    case CART:
        ret = "cartesian"; break;
    case GRAPH:
        ret = "graph"; break;
    case DIST_GRAPH:
        ret = "distributed graph"; break;
    default:
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, "  ... topology ", topo, 
            " wrong. Possible values are ", UNDEFINED, ", ", CART, ", ", GRAPH,
            ", ", DIST_GRAPH, '\n');
        break;
    }
    return ret;
}

bool Comm::_null_copy_fn( Comm &oldcomm, int keyval, 
    void *extra_state, void *attr_val, void *&attr_val_out) noexcept{
    return false;
}

bool Comm::_dup_fn( Comm &oldcomm, int keyval, 
    void *extra_state, void *attr_val, void *&attr_val_out) noexcept{
    attr_val_out = attr_val;
    return true;
}

void Comm::_null_del_fn( Comm &comm, int keyval, 
    void *attr_val, void *extra_state ) noexcept{}

int Comm::_copy_attr_fn( mpi_t oldcomm, int keyval, void *extra_state, 
    void *attr_val, void *attr_val_out, int *flag ){
    int ret = MPI_SUCCESS;
    try{
        auto &_es = *(_attr_extra_state_t *) extra_state;
        Comm tempcomm = _from_raw( oldcomm, 0 );
        *flag = _es.copy_attr_fn( tempcomm, keyval, _es.extra_state, attr_val,
            *(void **)attr_val_out );
    }catch( const ErrMPI &e ){
        *flag = 0; 
        *(void **)attr_val_out = nullptr;
        ret = e.get_errno();
    }
    return ret;
}

int Comm::_del_attr_fn( mpi_t comm, int keyval,
    void *attr_val, void *extra_state ){
    int ret = MPI_SUCCESS;
    try{
        auto &_es = *(_attr_extra_state_t *) extra_state;
        Comm tempcomm = _from_raw( comm, 0 );
        _es.del_attr_fn( tempcomm, keyval, attr_val, _es.extra_state );
    }catch( const ErrMPI &e ){
        ret = e.get_errno();
    }
    return ret;
}

} // namespace MPI
} // namespace HIPP

