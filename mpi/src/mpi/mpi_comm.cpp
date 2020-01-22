#include "mpi_comm.h"

namespace HIPP{
namespace MPI{
ostream & Comm::info( ostream &os, int fmt_cntl ) const{
    if( fmt_cntl == 0 ){
        prt(os, HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI:Comm));
        if( is_null() ) prt(os, "Null");
        else prt(os, "size: ", size(), ", rank: ", rank());
    }
    if( fmt_cntl >= 1 ){
        prt(os, HIPPCNTL_CLASS_INFO(HIPP::MPI::Comm));
        if( is_null() ) prt(os, "Null") << endl;
        else 
            prt(os, "  Size info (size=", size(), 
                ", rank=", rank(), ")") << endl;
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
Comm Comm::split( int color, int key )const{
    mpi_t newcomm = _obj_ptr->split(color, key);
    int state = (newcomm == _obj_raw_t::nullval())?0:1;
    return _from_raw(newcomm, state );
}
Comm Comm::dup(){
    mpi_t newcomm = _obj_ptr->dup();
    int state = (newcomm == _obj_raw_t::nullval())?0:1;
    return _from_raw( newcomm, state );
}
Comm Comm::create( const Group &group ){
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
    const int senddispls[], const Datatype sendtypes[],
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const Datatype recvtypes[] ) const{
    int commsize = is_inter() ? remote_size() : size();
    vector<Datatype::mpi_t> _st(commsize), _rt(commsize);
    for(int i=0; i<commsize; ++i){
        _st[i] = sendtypes[i].raw();
        _rt[i] = recvtypes[i].raw();
    }
    _obj_ptr->alltoallw(sendbuf, sendcounts, senddispls, _st.data(), 
        recvbuf, recvcounts, recvdispls, _rt.data());
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
    const int senddispls[], const Datatype sendtypes[],
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const Datatype recvtypes[] ) const{
    int commsize = is_inter() ? remote_size() : size();
    vector<Datatype::mpi_t> _st(commsize), _rt(commsize);
    for(int i=0; i<commsize; ++i){
        _st[i] = sendtypes[i].raw();
        _rt[i] = recvtypes[i].raw();
    }
    return Requests::_from_raw( _obj_ptr->ialltoallw(sendbuf, sendcounts, 
        senddispls, _st.data(), 
        recvbuf, recvcounts, recvdispls, _rt.data()), 0);
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

} // namespace MPI
} // namespace HIPP

