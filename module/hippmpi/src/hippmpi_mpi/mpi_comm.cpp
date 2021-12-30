#include <mpi_comm.h>

namespace HIPP{
namespace MPI{

const Comm::copy_attr_fn_t 
    Comm::NULL_COPY_FN = Comm::_null_copy_fn,
    Comm::DUP_FN = Comm::_dup_fn;
const Comm::del_attr_fn_t Comm::NULL_DEL_FN = Comm::_null_del_fn;

std::unordered_map<int, Comm::_attr_extra_state_t *> Comm::_attr_extra_state; 

ostream & Comm::info( ostream &os, int fmt_cntl ) const{
    PStream ps {os};
    bool null_comm = is_null();
    if( null_comm ) {
        if( fmt_cntl == 0 )
            ps << "Comm{Null}";
        else
            ps << HIPPCNTL_CLASS_INFO(HIPP::MPI::Comm), "  Null\n"; 
        return os;
    }

    const int _rank = rank(), _size = size();
    const bool inter_comm = is_inter();
    const int topo = topo_test();
    auto topos = _topostr(topo);
    if(fmt_cntl == 0) {
        ps << "Comm{size=", _size, ", rank=", _rank, ", topology=", topos;
        if( inter_comm )
            ps << ", remote size=", remote_size();
        ps << "}";
        return os;
    }

    ps << HIPPCNTL_CLASS_INFO(HIPP::MPI::Comm),
        "  Process group{size=", _size, ", rank=", _rank;
    if( inter_comm )
        ps << ", remote size=", remote_size();
    ps << "}\n",
        "  Topology{", topos;
    if( topo == CART ) {
        vector<int> dims, periods, coords;
        cart_get(dims, periods, coords);
        ps << ", ndims=", dims.size(), ", dims={", dims, 
            "}, periods={", periods, "}, coords={", coords, "}";
    }
    ps << "}\n";
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

int Comm::topo_test()const{
    return _obj_ptr->topo_test();
}

void Comm::dims_create(int nnodes, int ndims, int dims[]) {
    _obj_raw_t::dims_create(nnodes, ndims, dims);
}

void Comm::dims_create(int nnodes, int ndims, vector<int> &dims){
    if( nnodes <= 0 || ndims <= 0 )
        ErrLogic::throw_(ErrLogic::eDOMAIN, emFLPFB, 
            "  ... nnodes ", nnodes, " and ndims ", ndims, 
            " are invalid (must be positive)\n");
    dims.resize(ndims, 0);
    dims_create(nnodes, ndims, dims.data());
}

void Comm::dims_create(int nnodes, ContiguousBuffer<int> dims) {
    auto [p, n] = dims;
    int ndims = static_cast<int>( static_cast<int>(n) );
    dims_create(nnodes, ndims, p);
}

vector<int> Comm::dims_create(int nnodes, int ndims) {
    vector<int> dims(ndims);
    dims_create(nnodes, dims);
    return dims;
}

Comm Comm::cart_create(int ndims, const int dims[], const int periods[], 
    int reorder) const
{
    auto new_comm = obj_raw().cart_create(ndims, dims, periods, reorder);
    return _from_raw(new_comm, 1);
}

Comm Comm::cart_create(ContiguousBuffer<const int> dims, 
    ContiguousBuffer<const int> periods, int reorder) const
{
    auto [p_dims, n_dims] = dims;
    auto [p_periods, n_periods] = periods;
    if(n_dims != n_periods)
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... dims.size() (", n_dims, 
            ") != periods.size() (", n_periods, ")\n");
    return cart_create(static_cast<int>(n_dims), p_dims, p_periods, reorder);
}

Comm Comm::cart_sub(const int remain_dims[]) const {
    auto new_comm = obj_raw().cart_sub(remain_dims);
    return _from_raw(new_comm, 1);
}

Comm Comm::cart_sub(ContiguousBuffer<const int> remain_dims) const {
    return cart_sub(remain_dims.get_buff());
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

void Comm::cart_get(ContiguousBuffer<int> dims, ContiguousBuffer<int> periods, 
    ContiguousBuffer<int> coords) const
{
    auto [p_dims, n_dims] = dims;
    auto [p_periods, n_periods] = periods;
    auto [p_coords, n_coords] = coords;
    if( n_dims != n_periods || n_dims != n_coords )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... sizes of buffers do not match (dims->", n_dims, 
            ", periods->", n_periods, ", coords->", n_coords, ")\n");
    obj_raw().cart_get(static_cast<int>(n_dims), p_dims, p_periods, p_coords);
}

int Comm::cart_rank( const vector<int> &coords )const {
    return _obj_ptr->cart_rank( coords.data() );
}

int Comm::cart_rank(ContiguousBuffer<const int> coords) const {
    return obj_raw().cart_rank( coords.get_buff() );
}

vector<int> Comm::cart_coords( int rank )const{
    int ndims = cartdim_get();
    vector<int> coords( ndims );
    _obj_ptr->cart_coords( rank, ndims, coords.data() );
    return coords;
}

void Comm::cart_coords(int rank, ContiguousBuffer<int> coords) const {
    auto [p_coords, n_coords] = coords;
    obj_raw().cart_coords(rank, static_cast<int>(n_coords), p_coords);
}

void Comm::cart_shift( int direction, int disp, 
    int &rank_src, int &rank_dest )const{
    _obj_ptr->cart_shift( direction, disp, &rank_src, &rank_dest );
}

std::pair<int, int> Comm::cart_shift(int direction, int disp) const {
    int src, dest;
    cart_shift(direction, disp, src, dest);
    return {src, dest};
}

Comm Comm::graph_create(int nnodes, const int index[], const int edges[], 
    int reorder) const
{
    auto new_comm = obj_raw().graph_create(nnodes, index, edges, reorder);
    return _from_raw(new_comm, 1);
}

Comm Comm::graph_create(ContiguousBuffer<const int> index, 
    ContiguousBuffer<const int> edges, int reorder) const
{
    auto [p_index, n_index] = index;
    auto [p_edges, n_edges] = edges;
    return graph_create(static_cast<int>(n_index), p_index, p_edges, reorder);
}

std::pair<int, int> Comm::graphdims_get() const {
    int nnodes, nedges;
    obj_raw().graphdims_get(&nnodes, &nedges);
    return {nnodes, nedges};
}

void Comm::graph_get(int maxindex, int maxedges, int index[], 
    int edges[]) const
{
    obj_raw().graph_get(maxindex, maxedges, index, edges);
}

void Comm::graph_get(ContiguousBuffer<int> index, 
    ContiguousBuffer<int> edges) const
{
    auto [p_index, n_index] = index;
    auto [p_edges, n_edges] = edges;
    graph_get(static_cast<int>(n_index), static_cast<int>(n_edges), 
        p_index, p_edges);
}

std::pair<vector<int>, vector<int> > Comm::graph_get() const {
    auto [nnodes, nedges] = graphdims_get();
    vector<int> index(nnodes), edges(nedges);
    graph_get(index, edges);
    return { std::move(index), std::move(edges) };
}

int Comm::graph_neighbors_count(int rank) const {
    return obj_raw().graph_neighbors_count(rank);
}

void Comm::graph_neighbors(int rank, int maxneighbors, int neighbors[]) const {
    obj_raw().graph_neighbors(rank, maxneighbors, neighbors);
}

void Comm::graph_neighbors(int rank, ContiguousBuffer<int> neighbors) const {
    auto [p, n] = neighbors;
    graph_neighbors(rank, static_cast<int>(n), p);
}

vector<int> Comm::graph_neighbors(int rank) const {
    int n = graph_neighbors_count(rank);
    vector<int> neighbors(n);
    graph_neighbors(rank, neighbors);
    return neighbors;
}

Comm Comm::dist_graph_create_adjacent(int indegree, const int sources[], 
    const int sourceweights[], int outdegree, const int destinations[], 
    const int destweights[], const Info &info, int reorder) const
{
    auto new_comm = obj_raw().dist_graph_create_adjacent(indegree, sources, 
        sourceweights, outdegree, destinations, destweights, info.raw(), 
        reorder);
    return _from_raw(new_comm, 1);
}

Comm Comm::dist_graph_create_adjacent(
    ContiguousBuffer<const int> sources, 
    ContiguousBuffer<const int> sourceweights,
    ContiguousBuffer<const int> destinations,
    ContiguousBuffer<const int> destweights,
    const Info &info, int reorder) const
{
    auto [p_src, n_src] = sources;
    auto [p_dst, n_dst] = destinations;
    return dist_graph_create_adjacent(
        static_cast<int>(n_src), p_src, sourceweights.get_buff(), 
        static_cast<int>(n_dst), p_dst, destweights.get_buff(), info, reorder);
}

Comm Comm::dist_graph_create_adjacent(
    ContiguousBuffer<const int> sources, 
    ContiguousBuffer<const int> destinations,
    const Info &info, int reorder) const
{
    auto [p_src, n_src] = sources;
    auto [p_dst, n_dst] = destinations;
    return dist_graph_create_adjacent(
        static_cast<int>(n_src), p_src, UNWEIGHTED, 
        static_cast<int>(n_dst), p_dst, UNWEIGHTED, info, reorder);
}

Comm Comm::dist_graph_create(int n, const int sources[], const int degrees[], 
    const int destinations[], const int weights[], 
    const Info &info, int reorder) const
{
    auto new_comm = obj_raw().dist_graph_create(n, sources, degrees, 
        destinations, weights, info.raw(), reorder);
    return _from_raw(new_comm, 1);
}

Comm Comm::dist_graph_create(
    ContiguousBuffer<const int> sources, 
    ContiguousBuffer<const int> degrees,
    ContiguousBuffer<const int> destinations,
    ContiguousBuffer<const int> weights,
    const Info &info, int reorder) const
{
    auto [p_src, n_src] = sources;
    return dist_graph_create(static_cast<int>(n_src), p_src, 
        degrees.get_buff(), destinations.get_buff(), weights.get_buff(), 
        info, reorder);
}

Comm Comm::dist_graph_create(
    ContiguousBuffer<const int> sources, 
    ContiguousBuffer<const int> degrees,
    ContiguousBuffer<const int> destinations,
    const Info &info, int reorder) const
{
    auto [p_src, n_src] = sources;
    return dist_graph_create(static_cast<int>(n_src), p_src, 
        degrees.get_buff(), destinations.get_buff(), UNWEIGHTED, 
        info, reorder);
}

std::tuple<int, int, int> Comm::dist_graph_neighbors_count() const {
    int indeg, outdeg, weighted;
    obj_raw().dist_graph_neighbors_count(&indeg, &outdeg, &weighted);
    return {indeg, outdeg, weighted};
}

void Comm::dist_graph_neighbors(int maxindegree, int sources[],
    int sourceweights[], int maxoutdegree, int destinations[], 
    int destweights[]) const
{
    obj_raw().dist_graph_neighbors(maxindegree, sources, sourceweights,
        maxoutdegree, destinations, destweights);
}

void Comm::dist_graph_neighbors(ContiguousBuffer<int> sources,
    ContiguousBuffer<int> sourceweights, 
    ContiguousBuffer<int> destinations,
    ContiguousBuffer<int> destweights) const
{
    auto [p_src, n_src] = sources;
    auto [p_dst, n_dst] = destinations;
    dist_graph_neighbors(static_cast<int>(n_src), p_src, 
        sourceweights.get_buff(), static_cast<int>(n_dst), p_dst, 
        destweights.get_buff());
}

void Comm::dist_graph_neighbors(ContiguousBuffer<int> sources,
    ContiguousBuffer<int> destinations) const
{
    auto [p_src, n_src] = sources;
    auto [p_dst, n_dst] = destinations;
    dist_graph_neighbors(static_cast<int>(n_src), p_src, UNWEIGHTED, 
        static_cast<int>(n_dst), p_dst, UNWEIGHTED);
}

std::pair<vector<int>, vector<int> > Comm::dist_graph_neighbors() const {
    auto [indeg, outdeg, weighted] = dist_graph_neighbors_count();
    vector<int> srcs(indeg), dsts(outdeg);
    dist_graph_neighbors(srcs, dsts);
    return {std::move(srcs), std::move(dsts)};
}

int Comm::cart_map(int ndims, const int dims[], const int periods[]) const {
    int newrank;
    obj_raw().cart_map(ndims, dims, periods, &newrank);
    return newrank;
}

int Comm::cart_map(ContiguousBuffer<int> dims, 
    ContiguousBuffer<int> periods) const 
{
    auto [p_dims, n_dims] = dims;
    return cart_map(static_cast<int>(n_dims), p_dims, periods.get_buff());
}

int Comm::graph_map(int nnodes, const int index[], const int edges[]) const {
    int newrank;
    obj_raw().graph_map(nnodes, index, edges, &newrank);
    return newrank;
}

int Comm::graph_map(ContiguousBuffer<int> index, 
    ContiguousBuffer<int> edges) const
{
    auto [p_index, nnodes] = index;
    return graph_map(nnodes, p_index, edges.get_buff());
}

void Comm::neighbor_allgather(const void* sendbuf, int sendcount, 
    const Datatype &sendtype, void* recvbuf, int recvcount, 
    const Datatype &recvtype) const
{
    obj_raw().neighbor_allgather(sendbuf, sendcount, sendtype.raw(), recvbuf, 
        recvcount, recvtype.raw());
}

void Comm::neighbor_allgather(const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype) const
{
    neighbor_allgather(sendbuf, count, dtype, recvbuf, count, dtype);
}

void Comm::neighbor_allgather(const ConstDatapacket &send_dpacket,
    void *recvbuf) const
{
    const auto &[p,n,dt] = send_dpacket;
    neighbor_allgather(p, recvbuf, n, dt);
}

void Comm::neighbor_allgather(const ConstDatapacket &send_dpacket,
    const Datapacket &recv_dpacket) const
{
    neighbor_allgather(send_dpacket, recv_dpacket.get_buff());
}

void Comm::neighbor_allgatherv(
    const void* sendbuf, int sendcount, const Datatype &sendtype, 
    void* recvbuf, const int recvcounts[], const int displs[], 
    const Datatype &recvtype) const
{
    obj_raw().neighbor_allgatherv(sendbuf, sendcount, sendtype.raw(),
        recvbuf, recvcounts, displs, recvtype.raw());
}

void Comm::neighbor_allgatherv(
    const ConstDatapacket &send_dpacket, void *recvbuf, 
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs,
    const Datatype &recvtype) const
{
    auto [_recvcounts, n_cs] = recvcounts;
    auto [_displs, n_ds] = displs;
    if( n_cs != n_ds )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... recvcounts", recvcounts, 
            " does not match displs ", displs, '\n');
    const auto &[p,n,dt] = send_dpacket;
    neighbor_allgatherv(p, n, dt, recvbuf, _recvcounts, _displs, recvtype);
}

void Comm::neighbor_allgatherv(
    const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket,
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs) const
{
    const auto &[p,n,dt] = recv_dpacket;
    neighbor_allgatherv(send_dpacket, p, recvcounts, displs, dt);
}

void Comm::neighbor_alltoall(
    const void* sendbuf, int sendcount, const Datatype &sendtype, 
    void* recvbuf, int recvcount, const Datatype &recvtype) const
{
    obj_raw().neighbor_alltoall(sendbuf, sendcount, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw());
}

void Comm::neighbor_alltoall(
    const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype) const
{
    neighbor_alltoall(sendbuf, count, dtype, recvbuf, count, dtype);
}


void Comm::neighbor_alltoallv(const void* sendbuf, const int sendcounts[], 
    const int sdispls[], const Datatype &sendtype, void* recvbuf, 
    const int recvcounts[], const int rdispls[], 
    const Datatype &recvtype) const
{
    obj_raw().neighbor_alltoallv(sendbuf, sendcounts, sdispls, sendtype.raw(),
        recvbuf, recvcounts, rdispls, recvtype.raw());
}

void Comm::neighbor_alltoallw(
    const void* sendbuf, const int sendcounts[], const aint_t sdispls[], 
    const Datatype::mpi_t sendtypes[], 
    void* recvbuf, const int recvcounts[], const aint_t rdispls[], 
    const Datatype::mpi_t recvtypes[]) const
{
    obj_raw().neighbor_alltoallw(sendbuf, sendcounts, sdispls, sendtypes, 
        recvbuf, recvcounts, rdispls, recvtypes);
}

Requests Comm::ineighbor_allgather(const void* sendbuf, int sendcount, 
    const Datatype &sendtype, void* recvbuf, 
    int recvcount, const Datatype &recvtype) const
{
    auto req = obj_raw().ineighbor_allgather(
        sendbuf, sendcount, sendtype.raw(), recvbuf, 
        recvcount, recvtype.raw());
    return Requests::_from_raw_bare(req);
}

Requests Comm::ineighbor_allgather(
    const void *sendbuf, void *recvbuf, int count, const Datatype &dtype) const
{
    return ineighbor_allgather(sendbuf, count, dtype, recvbuf, count, dtype);
}

Requests Comm::ineighbor_allgather(const ConstDatapacket &send_dpacket, 
    void *recvbuf) const
{
    const auto &[p,n,dt] = send_dpacket;
    return ineighbor_allgather(p, recvbuf, n, dt);
}

Requests Comm::ineighbor_allgather(const ConstDatapacket &send_dpacket, 
    const Datapacket &recv_dpacket) const
{
    return ineighbor_allgather(send_dpacket, recv_dpacket.get_buff());
}

Requests Comm::ineighbor_allgatherv(const void* sendbuf, int sendcount, 
    const Datatype &sendtype, void* recvbuf, const int recvcounts[], 
    const int displs[], const Datatype &recvtype) const
{
    auto req = obj_raw().ineighbor_allgatherv(sendbuf, sendcount, 
        sendtype.raw(), recvbuf, recvcounts, displs, recvtype.raw());
    return Requests::_from_raw_bare(req);
}

Requests Comm::ineighbor_allgatherv(
    const ConstDatapacket &send_dpacket,
    void *recvbuf, ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs,
    const Datatype &recvtype ) const
{
    auto [_recvcounts, n_cs] = recvcounts;
    auto [_displs, n_ds] = displs;
    if( n_cs != n_ds )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... recvcounts", recvcounts, 
            " does not match displs ", displs, '\n');
    const auto &[p,n,dt] = send_dpacket;
    return ineighbor_allgatherv(p, n, dt, recvbuf, _recvcounts, _displs, 
        recvtype);
}

Requests Comm::ineighbor_allgatherv(
    const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket, 
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs) const
{
    const auto &[p,n,dt] = recv_dpacket;
    return ineighbor_allgatherv(send_dpacket, p, recvcounts, displs, dt);
}


Requests Comm::ineighbor_alltoall(const void* sendbuf, int sendcount,
    const Datatype &sendtype, void* recvbuf, int recvcount,
    const Datatype &recvtype) const
{
    auto req = obj_raw().ineighbor_alltoall(sendbuf, sendcount, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw());
    return Requests::_from_raw_bare(req);
}

Requests Comm::ineighbor_alltoall(const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype) const
{
    return ineighbor_alltoall(sendbuf, count, dtype, recvbuf, count, dtype);
}


Requests Comm::ineighbor_alltoallv(const void* sendbuf, const int sendcounts[], 
    const int sdispls[], const Datatype &sendtype, void* recvbuf, 
    const int recvcounts[], const int rdispls[], 
    const Datatype &recvtype) const
{
    auto req = obj_raw().ineighbor_alltoallv(sendbuf, sendcounts, sdispls, 
        sendtype.raw(),
        recvbuf, recvcounts, rdispls, recvtype.raw());
    return Requests::_from_raw_bare(req);
}

Requests Comm::ineighbor_alltoallw(const void* sendbuf, const int sendcounts[],
    const aint_t sdispls[], const Datatype::mpi_t sendtypes[],
    void* recvbuf, const int recvcounts[],
    const aint_t rdispls[], const Datatype::mpi_t recvtypes[]) const
{
    auto req = obj_raw().ineighbor_alltoallw(sendbuf, sendcounts, sdispls, 
        sendtypes, recvbuf, recvcounts, rdispls, recvtypes);
    return Requests::_from_raw_bare(req);
}


Win Comm::win_create(void *base, aint_t size, int disp_unit, 
const Info &info) const {
    auto win = Win::_obj_raw_t::create(base, size, disp_unit, 
        info.raw(), raw());
    return Win::_from_raw(win, Win::_obj_raw_t::stFREE);
}

Win Comm::win_create_dynamic(const Info &info) const {
    auto win = Win::_obj_raw_t::create_dynamic(info.raw(), raw());
    return Win::_from_raw(win, Win::_obj_raw_t::stFREE);
}

Win Comm::win_allocate(void *&base_ptr, aint_t size, int disp_unit, 
    const Info &info ) const {
    auto win = Win::_obj_raw_t::allocate(size, disp_unit, info.raw(), raw(), 
        &base_ptr);
    return Win::_from_raw(win, Win::_obj_raw_t::stFREE);
}

Win Comm::win_allocate_shared(void *&base_ptr, 
    aint_t size, int disp_unit, const Info &info) const
{
    auto win = Win::_obj_raw_t::allocate_shared(size, disp_unit, info.raw(),
        raw(), &base_ptr);
    return Win::_from_raw(win, Win::_obj_raw_t::stFREE);
}

Status Comm::sendrecv(const ConstDatapacket &send_dpacket, int dest, int sendtag, 
    const Datapacket &recv_dpacket, int src, int recvtag)
{
    const auto &[p_s,n_s,dt_s] = send_dpacket;
    const auto &[p_r,n_r,dt_r] = recv_dpacket;
    return _obj_ptr->sendrecv(
        p_s, n_s, dt_s.raw(), dest, sendtag, 
        p_r, n_r, dt_r.raw(), src, recvtag);
}

Status Comm::sendrecv(const ConstDatapacket &send_dpacket, int dest, int sendtag, 
    void *recvbuf, int src, int recvtag)
{
    const auto [p,n,dt] = send_dpacket;
    return _obj_ptr->sendrecv(
        p, n, dt.raw(), dest, sendtag,
        recvbuf, n, dt.raw(), src, recvtag);
}

Status Comm::sendrecv_replace(const Datapacket &dpacket, int dest, int sendtag, 
    int src, int recvtag)
{
    const auto &[p, n, dt] = dpacket;
    return _obj_ptr->sendrecv_replace(p, n, dt.raw(), 
        dest, sendtag, src, recvtag);
}

Status Comm::probe(int src, int tag) const{
    return _obj_ptr->probe(src, tag);
}

Status Comm::iprobe(int src, int tag, int &flag) const{
    return _obj_ptr->iprobe(src, tag, flag);
}

std::pair<Status, Message> Comm::mprobe(int src, int tag) const{
    Message::mpi_t msg;
    Status st = _obj_ptr->mprobe(src, tag, msg);
    return {st, Message(msg)};
}

std::pair<Status, Message> Comm::improbe(int src, int tag, int &flag) const{
    Message::mpi_t msg;
    Status st = _obj_ptr->improbe(src, tag, flag, msg);
    return {st, Message(msg)};
}

void Comm::barrier() const{
    _obj_ptr->barrier();
}

void Comm::bcast( void *buf, int count, const Datatype &dtype, int root) const{
    _obj_ptr->bcast(buf, count, dtype.raw(), root);
}

void Comm::bcast(const Datapacket &dpacket, int root) const {
    auto & [p, n, dt] = dpacket;
    bcast(p, n, dt, root);
}

void Comm::gather( const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, int recvcount, const Datatype &recvtype, int root) const {
    _obj_ptr->gather(sendbuf, sendcount, sendtype.raw(), recvbuf,
        recvcount, recvtype.raw(), root);
}

void Comm::gather(const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, int root) const {
    gather(sendbuf, count, dtype, recvbuf, count, dtype, root);
}

void Comm::gather(const ConstDatapacket &send_dpacket, void *recvbuf, 
int root) const 
{
    auto & [p,n,dt] = send_dpacket;
    gather(p, recvbuf, n, dt, root);
}

void Comm::gather(const ConstDatapacket &send_dpacket, 
    const Datapacket &recv_dpacket, int root) const
{
    gather(send_dpacket, recv_dpacket.get_buff(), root);
}

void Comm::gatherv(const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    const Datatype &recvtype, int root ) const 
{
    _obj_ptr->gatherv( sendbuf, sendcount, sendtype.raw(), 
        recvbuf, recvcounts, displs, recvtype.raw(), root );
}

void Comm::gatherv(const ConstDatapacket &send_dpacket, void *recvbuf, 
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs,
    const Datatype &recvtype, int root) const
{
    const auto &[_recvcounts, n_cs] = recvcounts;
    const auto &[_displs, n_ds] = displs;
    if( n_cs != n_ds ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... recvcounts", recvcounts, 
            " does not match displs ", displs, '\n');
    auto & [p,n,dt] = send_dpacket;
    gatherv(p, n, dt, recvbuf, _recvcounts, _displs, recvtype, root);
}

void Comm::gatherv(const ConstDatapacket &send_dpacket, 
    const Datapacket &recv_dpacket, 
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs, int root) const
{
    auto & [p,n,dt] = recv_dpacket;
    gatherv(send_dpacket, p, recvcounts, displs, dt, root);
}

void Comm::scatter(
    const void *sendbuf, int sendcount, const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype, int root )const{
    _obj_ptr->scatter( sendbuf, sendcount, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw(), root );
}

void Comm::scatter(const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, int root) const {
    scatter(sendbuf, count, dtype, recvbuf, count, dtype, root);
}

void Comm::scatter(const void *sendbuf, 
    const Datapacket &recv_dpacket, int root) const 
{
    auto & [p, n, dt] = recv_dpacket;
    scatter(sendbuf, p, n, dt, root);
}

void Comm::scatter(const ConstDatapacket &send_dpacket,
    const Datapacket &recv_dpacket, int root) const
{
    auto & [p,n,dt] = send_dpacket;
    scatter(p, recv_dpacket, root);
}

void Comm::scatterv(const void *sendbuf, const int sendcounts[], 
    const int displs[], const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype, int root) const
{
    _obj_ptr->scatterv(sendbuf, sendcounts, displs, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw(), root);
}

void Comm::scatterv(const void *sendbuf, ContiguousBuffer<const int> sendcounts, 
    ContiguousBuffer<const int> displs, const Datatype &sendtype,
    const Datapacket &recv_dpacket, int root) const
{
    auto [_sendcounts, n_cs] = sendcounts;
    auto [_displs, n_ds] = displs;
    if( n_cs != n_ds )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... sendcounts", sendcounts, 
            " does not match displs ", displs, '\n');
    auto [p, n, dt] = recv_dpacket;
    scatterv(sendbuf, _sendcounts, _displs, sendtype, 
        p, n, dt, root);
}

void Comm::scatterv(const ConstDatapacket &send_dpacket, 
    ContiguousBuffer<const int> sendcounts, 
    ContiguousBuffer<const int> displs,
    const Datapacket &recv_dpacket, int root) const
{
    auto &[p,n,dt] = send_dpacket;
    scatterv(p, sendcounts, displs, dt, recv_dpacket, root);
}

void Comm::allgather( const void *sendbuf, int sendcount, 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype ) const{
    _obj_ptr->allgather( sendbuf, sendcount, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw() );
}

void Comm::allgather(const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype) const 
{
    allgather(sendbuf, count, dtype, recvbuf, count, dtype);
}

void Comm::allgather(const ConstDatapacket &send_dpacket,
    void *recvbuf) const 
{
    const auto &[p,n,dt] = send_dpacket;
    allgather(p, recvbuf, n, dt);
}

void Comm::allgather(const ConstDatapacket &send_dpacket,
    const Datapacket &recv_dpacket) const 
{
    allgather(send_dpacket, recv_dpacket.get_buff());
}

void Comm::allgatherv(
    const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    const Datatype &recvtype ) const{
    _obj_ptr->allgatherv(sendbuf, sendcount, sendtype.raw(),
        recvbuf, recvcounts, displs, recvtype.raw());
}

void Comm::allgatherv(
    const ConstDatapacket &send_dpacket, void *recvbuf, 
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs,
    const Datatype &recvtype) const
{
    auto [_recvcounts, n_cs] = recvcounts;
    auto [_displs, n_ds] = displs;
    if( n_cs != n_ds )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... recvcounts", recvcounts, 
            " does not match displs ", displs, '\n');
    const auto &[p,n,dt] = send_dpacket;
    allgatherv(p, n, dt, recvbuf, _recvcounts, _displs, recvtype);
}

void Comm::allgatherv(
    const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket,
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs) const
{
    const auto &[p, n, dt] = recv_dpacket;
    allgatherv(send_dpacket, p, recvcounts, displs, dt);
}

void Comm::alltoall( const void *sendbuf, int sendcount, 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype ) const{
    _obj_ptr->alltoall( sendbuf, sendcount, sendtype.raw(),
        recvbuf, recvcount, recvtype.raw() );
}

void Comm::alltoall(const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype) const
{
    alltoall(sendbuf, count, dtype, recvbuf, count, dtype);
}

void Comm::alltoallv( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], const Datatype &sendtype,
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const Datatype &recvtype ) const
{
    _obj_ptr->alltoallv(sendbuf, sendcounts, senddispls, sendtype.raw(),
    recvbuf, recvcounts, recvdispls, recvtype.raw());
}

void Comm::alltoallw( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], const Datatype::mpi_t sendtypes[],
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const Datatype::mpi_t recvtypes[] ) const
{
    _obj_ptr->alltoallw(sendbuf, sendcounts, senddispls, sendtypes, 
        recvbuf, recvcounts, recvdispls, recvtypes);
}

void Comm::reduce( const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype, const Oppacket &op, int root ) const
{
    _obj_ptr->reduce( sendbuf, recvbuf, count, 
        dtype.raw(), op._op.raw(), root );
}

void Comm::reduce( const ConstDatapacket &send_dpacket, void *recvbuf,
    const Oppacket &op, int root ) const {
    const auto &[p,n,dt] = send_dpacket;
    reduce(p, recvbuf, n, dt, op, root);
}

void Comm::reduce(const void *sendbuf, const Datapacket &recv_dpacket,
    const Oppacket &op, int root) const
{
    auto &[p,n,dt] = recv_dpacket;
    reduce(sendbuf, p, n, dt, op, root);
}

void Comm::reduce(const ConstDatapacket &send_dpacket, 
    const Datapacket &recv_dpacket, const Oppacket &op, int root) const
{
    reduce(send_dpacket, recv_dpacket.get_buff(), op, root);
}

void Comm::allreduce( const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype, const Oppacket &op ) const{
    _obj_ptr->allreduce( sendbuf, recvbuf, count, dtype.raw(), op._op.raw() );
}

void Comm::allreduce( const ConstDatapacket &send_dpacket, void *recvbuf, 
    const Oppacket &op ) const {
    auto &[p, n, dt] = send_dpacket;
    allreduce(p, recvbuf, n, dt, op);
}

void Comm::allreduce(const void *sendbuf, const Datapacket &recv_dpacket,
    const Oppacket &op ) const
{
    auto &[p,n,dt] = recv_dpacket;
    allreduce(sendbuf, p, n, dt, op);
}

void Comm::allreduce(const ConstDatapacket &send_dpacket, 
    const Datapacket &recv_dpacket, const Oppacket &op) const
{
    allreduce(send_dpacket, recv_dpacket.get_buff(), op);
}

void Comm::reduce_local(const void *inbuf, void *inoutbuf, int count, 
    const Datatype &dtype, const Oppacket &op ){
    _obj_raw_t::reduce_local( 
        inbuf, inoutbuf, count, dtype.raw(), op._op.raw() );
}

void Comm::reduce_local(const ConstDatapacket &in_dpacket, void *inoutbuf, 
    const Oppacket &op)
{
    auto &[p, n, dt] = in_dpacket;
    reduce_local(p, inoutbuf, n, dt, op);
}

void Comm::reduce_local(const ConstDatapacket &in_dpacket, 
    const Datapacket &inout_dpacket, const Oppacket &op)
{
    reduce_local(in_dpacket, inout_dpacket.get_buff(), op);
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
Requests Comm::ibcast( void *buf, int count, const Datatype &dtype, 
    int root) const
{
    return Requests::_from_raw( 
        _obj_ptr->ibcast(buf, count, dtype.raw(), root), 0);
}
Requests Comm::ibcast(const Datapacket &dpacket, int root) const {
    const auto &[p,n,dt] = dpacket;
    return ibcast(p, n, dt, root);
}
Requests Comm::igather( 
    const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, int recvcount, const Datatype &recvtype, int root) const{
    return Requests::_from_raw( _obj_ptr->igather(sendbuf, sendcount, 
        sendtype.raw(), recvbuf,
        recvcount, recvtype.raw(), root), 0);
}

Requests Comm::igather(const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, int root) const {
    return igather(sendbuf, count, dtype, recvbuf, count, dtype, root);
}

Requests Comm::igather(const ConstDatapacket &send_dpacket, 
    void *recvbuf, int root) const {
    auto &[p, n, dt] = send_dpacket;
    return igather(p, recvbuf, n, dt, root);
}
Requests Comm::igather(const ConstDatapacket &send_dpacket, 
    const Datapacket &recv_dpacket, int root) const
{
    return igather(send_dpacket, recv_dpacket.get_buff(), root);
}

Requests Comm::igatherv(
    const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    const Datatype &recvtype, int root ) const{
    return Requests::_from_raw( _obj_ptr->igatherv( sendbuf, sendcount, 
        sendtype.raw(), 
        recvbuf, recvcounts, displs, recvtype.raw(), root ), 0);
}

Requests Comm::igatherv(
    const ConstDatapacket &send_dpacket, void *recvbuf, 
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs, 
    const Datatype &recvtype, int root ) const
{
    auto [_recvcounts, n_cs] = recvcounts;
    auto [_displs, n_ds] = displs;
    if( n_cs != n_ds )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... recvcounts", recvcounts, 
            " does not match displs ", displs, '\n');
    auto &[p,n,dt] = send_dpacket;
    return igatherv(p, n, dt, recvbuf, _recvcounts, _displs, recvtype, root);
}

Requests Comm::igatherv(
    const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket,
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs, int root) const
{
    auto &[p,n,dt] = recv_dpacket;
    return igatherv(send_dpacket, p, recvcounts, displs, dt, root);
}

Requests Comm::iscatter(
    const void *sendbuf, int sendcount, const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype, int root )const{
    return Requests::_from_raw( _obj_ptr->iscatter( sendbuf, sendcount, 
        sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw(), root ), 0);
}

Requests Comm::iscatter(const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, int root) const {
    return iscatter(sendbuf, count, dtype, recvbuf, count, dtype, root);
}

Requests Comm::iscatter(const void *sendbuf, 
    const Datapacket &recv_dpacket, int root) const {
    const auto &[p,n,dt] = recv_dpacket;
    return iscatter(sendbuf, p, n, dt, root);
}

Requests Comm::iscatter(const ConstDatapacket &send_dpacket,
    const Datapacket &recv_dpacket, int root) const
{
    return iscatter(send_dpacket.get_buff(), recv_dpacket, root);
}

Requests Comm::iscatterv(
    const void *sendbuf, const int sendcounts[], const int displs[], 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype, int root) const{
    return Requests::_from_raw( _obj_ptr->iscatterv(sendbuf, sendcounts, 
        displs, sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw(), root), 0);
}

Requests Comm::iscatterv(
    const void *sendbuf, ContiguousBuffer<const int> sendcounts, 
    ContiguousBuffer<const int> displs, const Datatype &sendtype,
    const Datapacket &recv_dpacket, int root) const
{
    auto [_sendcounts, n_cs] = sendcounts;
    auto [_displs, n_ds] = displs;
    if( n_cs != n_ds )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... sendcounts", sendcounts, 
            " does not match displs ", displs, '\n');
    auto [p,n,dt] = recv_dpacket;
    return iscatterv(p, _sendcounts, _displs, sendtype, p, n, dt, root);
}

Requests Comm::iscatterv(
    const ConstDatapacket send_dpacket, ContiguousBuffer<const int> sendcounts, 
    ContiguousBuffer<const int> displs,
    const Datapacket &recv_dpacket, int root) const
{
    auto [p,n,dt] = send_dpacket;
    return iscatterv(p, sendcounts, displs, dt, recv_dpacket, root);
}

Requests Comm::iallgather( const void *sendbuf, int sendcount, 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype ) const{
    return Requests::_from_raw( _obj_ptr->iallgather( sendbuf, sendcount, 
        sendtype.raw(), 
        recvbuf, recvcount, recvtype.raw() ), 0);
}

Requests Comm::iallgather(const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype) const
{
    return iallgather(sendbuf, count, dtype, recvbuf, count, dtype);
}

Requests Comm::iallgather(const ConstDatapacket &send_dpacket, 
    void *recvbuf) const 
{
    auto &[p,n,dt] = send_dpacket;
    return iallgather(p, recvbuf, n, dt);
}

Requests Comm::iallgather(const ConstDatapacket &send_dpacket, 
    const Datapacket &recv_dpacket) const
{
    return iallgather(send_dpacket, recv_dpacket.get_buff());
}

Requests Comm::iallgatherv(
    const void *sendbuf, int sendcount, const Datatype &sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    const Datatype &recvtype ) const{
    return Requests::_from_raw( _obj_ptr->iallgatherv(sendbuf, sendcount, 
        sendtype.raw(),
        recvbuf, recvcounts, displs, recvtype.raw()), 0);
}

Requests Comm::iallgatherv(
    const ConstDatapacket &send_dpacket,
    void *recvbuf, ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs,
    const Datatype &recvtype ) const
{
    auto [_recvcounts, n_cs] = recvcounts;
    auto [_displs, n_ds] = displs;
    if( n_cs != n_ds )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... recvcounts", recvcounts, 
            " does not match displs ", displs, '\n');
    auto &[p,n,dt] = send_dpacket;
    return iallgatherv(p,n,dt,recvbuf,_recvcounts,_displs,recvtype);
}

Requests Comm::iallgatherv(
    const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket, 
    ContiguousBuffer<const int> recvcounts, 
    ContiguousBuffer<const int> displs) const
{
    auto &[p,n,dt] = recv_dpacket;
    return iallgatherv(send_dpacket, p, recvcounts, displs, dt);
}

Requests Comm::ialltoall( const void *sendbuf, int sendcount, 
    const Datatype &sendtype,
    void *recvbuf, int recvcount, const Datatype &recvtype ) const{
    return Requests::_from_raw( _obj_ptr->ialltoall( sendbuf, sendcount, 
        sendtype.raw(),
        recvbuf, recvcount, recvtype.raw() ), 0);
}

Requests Comm::ialltoall(const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype) const
{
    return ialltoall(sendbuf, count, dtype, recvbuf, count, dtype);
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

Requests Comm::ireduce( const ConstDatapacket &send_dpacket, void *recvbuf,
    const Oppacket &op, int root ) const 
{
    auto &[p,n,dt] = send_dpacket;
    return ireduce(p, recvbuf, n, dt, op, root);
}

Requests Comm::ireduce(const void *sendbuf, const Datapacket &recv_dpacket,
    const Oppacket &op, int root ) const
{
    auto &[p,n,dt] = recv_dpacket;
    return ireduce(sendbuf, p, n, dt, op, root);
}

Requests Comm::ireduce(const ConstDatapacket &send_dpacket, 
    const Datapacket &recv_dpacket, const Oppacket &op, int root ) const
{
    return ireduce(send_dpacket, recv_dpacket.get_buff(), op, root);
}

Requests Comm::iallreduce( const void *sendbuf, void *recvbuf, int count, 
    const Datatype &dtype, const Oppacket &op ) const{
    return Requests::_from_raw( _obj_ptr->iallreduce( sendbuf, recvbuf, count, 
        dtype.raw(), op._op.raw() ), 0);
}

Requests Comm::iallreduce( const ConstDatapacket &send_dpacket, void *recvbuf, 
    const Oppacket &op ) const 
{
    auto &[p,n,dt] = send_dpacket;
    return iallreduce(p, recvbuf, n, dt, op);
}

Requests Comm::iallreduce(const void *sendbuf, const Datapacket &recv_dpacket,
    const Oppacket &op ) const
{
    auto &[p,n,dt] = recv_dpacket;
    return iallreduce(sendbuf, p, n, dt, op);
}

Requests Comm::iallreduce(const ConstDatapacket &send_dpacket, 
    const Datapacket &recv_dpacket,
    const Oppacket &op ) const
{
    return iallreduce(send_dpacket, recv_dpacket.get_buff(), op);
}

Requests Comm::ireduce_scatter_block( const void *sendbuf, void *recvbuf, 
    int recvcount, const Datatype &dtype, const Oppacket &op ) const{
    return Requests::_from_raw( _obj_ptr->ireduce_scatter_block( 
        sendbuf, recvbuf, recvcount, 
        dtype.raw(), op._op.raw() ), 0);
}

Requests Comm::ireduce_scatter( const void *sendbuf, void *recvbuf, 
    const int recvcounts[], const Datatype &dtype, 
    const Oppacket &op )const
{
    return Requests::_from_raw( _obj_ptr->ireduce_scatter( 
        sendbuf, recvbuf, recvcounts, 
        dtype.raw(), op._op.raw() ), 0);
}

Requests Comm::iscan( const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, const Oppacket &op ) const
{
    return Requests::_from_raw( _obj_ptr->iscan( 
        sendbuf, recvbuf, count, dtype.raw(), op._op.raw() ), 0);
}

Requests Comm::iexscan( const void *sendbuf, void *recvbuf, 
    int count, const Datatype &dtype, const Oppacket &op ) const
{
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
            " invalid. Possible values are ", 
            UNDEFINED, ", ", CART, ", ", GRAPH, ", ", DIST_GRAPH, '\n');
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

