/**
create: Yangyao CHEN, 2020/01/21
    [write   ] _Comm - the intermediate-level MPI communicator interface.
*/

#ifndef _HIPPMPI_MPI_RAW_COMM_H_
#define _HIPPMPI_MPI_RAW_COMM_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{

/**
The intermediate-level MPI communicator interface.

_Comm should not be constructed by users directly. The life time of it is 
handled by the high-level interfaces.
*/
class _Comm {
public:
    typedef MPI_Comm mpi_t;
    typedef MPI_Comm_copy_attr_function copy_attr_fn_t;
    typedef MPI_Comm_delete_attr_function del_attr_fn_t;

    inline static copy_attr_fn_t 
        *const NULL_COPY_FN = MPI_NULL_COPY_FN, 
        *const DUP_FN = MPI_COMM_DUP_FN;
    inline static del_attr_fn_t 
        *const NULL_DEL_FN = MPI_NULL_DELETE_FN;

    /**
    Intermediate-level communicator constructors.
    @val:    the actual original MPI_Comm variable.
    @state:  indicates how the life time of communicator is controlled.
             If the life time is system-managed, that is, not handled by
             this instance, set state to 0.
             Otherwise set state to 1, and the communicator is freed when 
             deconstructing.
    For the null-communicator, it is safe to set state 0 or 1.
    */
    _Comm( mpi_t val, int state ) noexcept;
    ~_Comm() noexcept;
    static void free( mpi_t &comm );

    int size() const;
    int rank() const;
    mpi_t raw() const noexcept;
    bool is_null() const noexcept;
    bool is_inter() const;
    int remote_size() const;

    /**
    Attribute caching.
    */
    static int create_keyval( copy_attr_fn_t *copy_attr_fn, 
        del_attr_fn_t *delete_attr_fn, void *extra_state );
    static void free_keyval( int *comm_keyval );
    void set_attr( int keyval, void *attr_val );
    bool get_attr( int keyval, void *attr_val ) const;
    void del_attr( int keyval );
    
    /**
    New communicator constructors.
    */
    mpi_t split( int color, int key = 0 )const;
    mpi_t dup( )const;
    static mpi_t create( mpi_t comm, MPI_Group group );
    static mpi_t nullval() noexcept;
    static mpi_t selfval() noexcept;
    static mpi_t world() noexcept;
    static mpi_t create_inter( mpi_t local_comm, int local_leader, 
        mpi_t peer_comm, int remote_leader, int tag );
    static mpi_t merge_inter( mpi_t intercomm, int high );
    
    /**
    Access the group content.
    */
    MPI_Group group() const;
    MPI_Group remote_group() const;
    
    /**
    Cartesian topology creation method.
    */
    static void dims_create(int nnodes, int ndims, int dims[]);
    mpi_t cart_create(int ndims, const int dims[], const int periods[], 
        int reorder) const;
    mpi_t cart_sub(const int remain_dims[])const;

    /**
    Cartesian topology meta-info inquiry methods.
    */
    int cartdim_get()const;
    void cart_get(int maxdims, int dims[], int periods[], int coords[])const;
    int cart_rank(const int coords[])const;
    void cart_coords(int rank, int maxdims, int coords[])const;
    void cart_shift(int direction, int disp, int *rank_src, 
        int *rank_dest )const;

    /**
    Graph topology creation function.

    graph_create(): create a graph topology and return the new communicator with
    topology information attached.
    All processes in the current communicator must call this method with the 
    same arguments.
    The node number is in the range [0, nnodes). Node may linked to itself. 
    Multiple links between a pair of processes are allowed. Adjacent matrix 
    is not required to be symmetry. The link indicates no communication 
    direction.
    
    @nnodes: number of nodes in the graph. If smaller than the communicator 
    size, null communicator is returned in some processes.
    @index: specify the node degrees. index[i] is the total degrees of all nodes
    with node number <= i.
    @edges: flatten-joined edges (i.e., node numbers of neighbors) linked by 
    all nodes.
    @reorder: Boolean. If true, allow reordering the processes and then making 
    topology assignment.
    */
    mpi_t graph_create(int nnodes, const int index[], const int edges[], 
        int reorder) const;
     
    /**
    Graph topology meta-info inquiry methods.
    
    graphdims_get(): return the number of nodes and edges which are correct 
    input to graph_get().

    graph_get(): returns index and edges. ``maxindex`` and ``maxedges`` are 
    maximal capabilities of the two arrays.

    graph_neighbors_count(): return the number of neighbors near the process
    with given ``rank``.

    graph_neighbors(): returns the ranks of neighbors near the process with 
    given ``rank``. ``maxneighbors`` indicates the maximal capability of array
    ``neighbors``.
    */
    void graphdims_get(int *nnodes, int *nedges) const;
    void graph_get(int maxindex, int maxedges, int index[], int edges[]) const;
    int graph_neighbors_count(int rank) const;
    void graph_neighbors(int rank, int maxneighbors, int neighbors[]) const;

    /**
    Distributed graph topology creation function.
    
    Each process specifies only a subset of all edges. All processes must have 
    the same ``reorder`` and ``info`` arguments. The new communicator has the 
    same size with the old.

    ``info`` may (or may not) be used by the implementation to optimize the 
    process mapping (e.g., interpretation of weights, reordering quality, and 
    time limit on the graph management). _Info::nullval() is always a valid 
    argument.
    
    The weights of edges do not have standard semantics, but should be 
    non-negative and usually indicate the communication intensity. 
    Edge multiplicity may also hint the communication intensity. 

    Isolated process is allowed. A edge may has multiplicity > 1 with arbitrary
    order of weights.

    ``MPI_UNWEIGHTED`` may be used in all processes at the same time, indicating 
    no weighting. If a degree is 0, the weights array is not modified (may use 
    ``MPI_WEIGHTS_EMPTY``, but should not use NULL because MPI_UNWEIGHTED may be 
    ``NULL``).
    
    If ``reorder = true``, allow reordering the processes and then making 
    topology assignment.

    dist_graph_create_adjacent(): each process specifies all its incoming and
    outgoing edges. The overhead of this call is smaller than 
    dist_graph_create(). sources and destinations must be consistent in any 
    linked pair, with the same weights. 

    dist_graph_create(): each process indicates possible directed edges between 
    process pairs in the desired graph. This call is more flexible but usually
    has more overhead. 
    ``n`` is the number of source nodes. ``sources`` are the ranks of source 
    processes and ``degrees`` are their numbers of destinations. 
    ``destinations`` are flatten-joined destination ranks of all source 
    processes.
    ``sources`` and ``destinations`` in any process may have repeated items. The
    ordering does not matter.
    */
    mpi_t dist_graph_create_adjacent(int indegree, const int sources[], 
        const int sourceweights[], int outdegree, const int destinations[], 
        const int destweights[], MPI_Info info, int reorder) const;

    mpi_t dist_graph_create(int n, const int sources[], const int degrees[], 
        const int destinations[], const int weights[], MPI_Info info, 
        int reorder) const;

    /**
    Distributed graph topology meta-info inquiry methods.

    dist_graph_neighbors_count(): get the in and out degrees of the caller 
    process. The degrees take into account the edge specifications in all 
    processes.
    If ``MPI_UNWEIGHTED`` is used during the creation, return false in 
    ``weighted``. Otherwise return true.

    dist_graph_neighbors(): get neighbors. An edge with multiplicity > 1 
    results multiple items to the arguments with defining ordering.

    If the distributed graph is created with ``dist_graph_create_adjacent()``
    the returned arrays follow the same order passed to the creation. 
    Otherwise the only guarantee is that multiple calls of this method on 
    the same communicator return the same results.

    Each of the weight arguments may be ``MPI_UNWEIGHTED``. If 
    ``MPI_UNWEIGHTED`` is used during topology creation, the weight arguments 
    are not modified.
    ``maxindegree`` or ``maxoutdegree`` may be less than the actual degree. Then
    only a part of the neighbors are returned into the arguments.
    */
    void dist_graph_neighbors_count(int *indegree, int *outdegree, 
        int *weighted) const;
	
    void dist_graph_neighbors(int maxindegree, int sources[],
	    int sourceweights[], int maxoutdegree, int destinations[], 
        int destweights[]) const;

    /**
    Low-level topology methods. With communicator manipulation methods they 
    can be used to create any desired topology.

    These two methods reorder the ranks of processes in the current communicator 
    and return the new rank of the caller process, for CART and GRAPH 
    topologies, respectively.

    The methods cart_create() and graph_create() may be implemented by first 
    calling cart_map() and graph_map(), respectively, and then using split()
    to separate not-in-topology processes from in-topology processes.
    */
    void cart_map(int ndims, const int dims[], const int periods[], 
        int *newrank) const;
    void graph_map(int nnodes, const int index[], const int edges[], 
        int *newrank) const;

    /**
    Return the topology type of the communicator. Possible values are 
    GRAPH, CART, DIST_GRAPH, or UNDEFINED (for no topology).
    */
    int topo_test()const;

    /**
    Neighbor collective communications on virtual topologies. These calls are 
    collective over the entire communicator. 
    
    Each process communicates with and only with the nearest neighbors 
    (i.e., direct neighbors) on the topology. For Cartesian topology, they 
    are the disp = -1 and +1 processes, for every dimension. If non-periodic,
    some neighbors may be PROC_NULL and then the data buffers must exist but are 
    not touched. For graph topology, the adjacent matrix must be symmetric. 
    The order of neighbors is consistent with ``graph_neighbors()``. 
    For distributed graph, the order of neighbors is consistent with 
    ``dist_graph_neighbors()``.

    The communication pattern is sparse. If the topology graph is densely
    connected, these calls are equivalent to ordinary collective calls.

    neighbor_allgather(): each process i sends to all its neighbors j the same 
    data in ``sendbuf``, where (i,j) is an edge in the adjacent matrix. 
    Each process i receives from all its neighbors j and contiguously store the 
    data into ``recvbuf``, where (j, i) is an edge in the adjacent matrix.
    For distributed graph, it is as if each process sends to its outgoing 
    neighbors and receives from its incoming neighbors. The sending type 
    signature at each process must be consistent with receiving type 
    signatures at all other processes.

    neighbor_allgatherv(): extends ``neighbor_allgather()`` by allowing each
    process receiving different number of data items from its neighbors.

    neighbor_alltoall(): extends ``neighbor_allgather()`` by allowing each 
    process sending different data content to its neighbors.

    neighbor_alltoallv(): extents ``neighbor_allgatherv()`` by allowing each 
    process sending different number of data items from its neighbors.

    neighbor_alltoallw(): extents ``neighbor_alltoallv()`` by allowing the 
    datatypes are different among neighbors.

    The methods prefixed with ``i`` are nonblocking variants of the 
    corresponding blocking calls.
    */
    void neighbor_allgather(const void* sendbuf, int sendcount, 
	    MPI_Datatype sendtype, void* recvbuf, int recvcount, 
	    MPI_Datatype recvtype) const;
    void neighbor_allgatherv(const void* sendbuf, int sendcount, 
        MPI_Datatype sendtype, void* recvbuf, const int recvcounts[], 
        const int displs[], MPI_Datatype recvtype) const;
    void neighbor_alltoall(const void* sendbuf, int sendcount, 
        MPI_Datatype sendtype, void* recvbuf, int recvcount, 
        MPI_Datatype recvtype) const;
    void neighbor_alltoallv(const void* sendbuf, const int sendcounts[], 
        const int sdispls[], MPI_Datatype sendtype, void* recvbuf, 
        const int recvcounts[], const int rdispls[], 
        MPI_Datatype recvtype) const;
    void neighbor_alltoallw(const void* sendbuf, const int sendcounts[], 
        const MPI_Aint sdispls[], const MPI_Datatype sendtypes[], void* recvbuf, 
        const int recvcounts[], const MPI_Aint rdispls[], 
        const MPI_Datatype recvtypes[]) const;
    MPI_Request ineighbor_allgather(const void* sendbuf, int sendcount, 
        MPI_Datatype sendtype, void* recvbuf, 
        int recvcount, MPI_Datatype recvtype) const;
    MPI_Request ineighbor_allgatherv(const void* sendbuf, int sendcount, 
        MPI_Datatype sendtype, void* recvbuf, const int recvcounts[], 
        const int displs[], MPI_Datatype recvtype) const;
    MPI_Request ineighbor_alltoall(const void* sendbuf, int sendcount,
        MPI_Datatype sendtype, void* recvbuf, int recvcount,
        MPI_Datatype recvtype) const;
    MPI_Request ineighbor_alltoallv(const void* sendbuf, const int sendcounts[], 
        const int sdispls[], MPI_Datatype sendtype, void* recvbuf, 
        const int recvcounts[], const int rdispls[], 
        MPI_Datatype recvtype) const;
    MPI_Request ineighbor_alltoallw(const void* sendbuf, const int sendcounts[],
        const MPI_Aint sdispls[], const MPI_Datatype sendtypes[],
        void* recvbuf, const int recvcounts[],
        const MPI_Aint rdispls[], const MPI_Datatype recvtypes[]) const;

    /**
    Wrappers of MPI point-to-point communication.
    */
    void send( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const;
    void bsend(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const;
    void ssend(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const;
    void rsend(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const;
    MPI_Status recv( void *buff, int count, MPI_Datatype dtype, 
        int src, int tag ) const;
    
    MPI_Request isend( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const;
    MPI_Request ibsend( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const;
    MPI_Request issend( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const;
    MPI_Request irsend( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const;
    MPI_Request irecv( void *buff, int count, MPI_Datatype dtype, 
        int src, int tag ) const;

    MPI_Request send_init(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag) const;
    MPI_Request bsend_init(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag) const;
    MPI_Request ssend_init(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag) const;
    MPI_Request rsend_init(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag) const;
    MPI_Request recv_init(void *buff, int count, MPI_Datatype dtype, 
        int src, int tag) const;

    MPI_Status sendrecv(const void *sendbuff, int sendcount, 
        MPI_Datatype sendtype, int dest, int sendtag, void *recvbuff, 
        int recvcount, MPI_Datatype recvtype, int src, int recvtag);
    MPI_Status sendrecv_replace(void *buff, int count, MPI_Datatype dtype, 
        int dest, int sendtag, int src, int recvtag);

    MPI_Status iprobe(int src, int tag, int &flag) const;
    MPI_Status probe(int src, int tag) const;
    MPI_Status improbe(int src, int tag, int &flag, MPI_Message &message) const;
    MPI_Status mprobe(int src, int tag, MPI_Message &message) const;
    static MPI_Status mrecv( void *buff, int count, MPI_Datatype dtype, 
        MPI_Message &message);
    static MPI_Request imrecv(void *buff, int count, MPI_Datatype dtype, 
        MPI_Message &message);

    /**
    Wrappers of MPI original blocking/non-blocking collective communication
    (data transfer and computation).
    */
    void barrier() const;
    void bcast( void *buf, int count, MPI_Datatype dtype, int root ) const;
    void gather( const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const;
    void gatherv(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        MPI_Datatype recvtype, int root ) const;
    void scatter(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const;
    void scatterv(
        const void *sendbuf, const int sendcounts[], const int displs[], 
        MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root) const;
    void allgather( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype ) const;
    void allgatherv(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        MPI_Datatype recvtype ) const;
    void alltoall( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype ) const;
    void alltoallv( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], MPI_Datatype sendtype,
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        MPI_Datatype recvtype ) const;
    void alltoallw( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], const MPI_Datatype sendtypes[],
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        const MPI_Datatype recvtypes[] ) const;
    void reduce( const void *sendbuf, void *recvbuf, int count, 
        MPI_Datatype dtype, MPI_Op op, int root ) const;
    void allreduce( const void *sendbuf, void *recvbuf, int count, 
        MPI_Datatype dtype, MPI_Op op ) const;
    static void reduce_local( const void *inbuf, void *inoutbuf, int count, 
        MPI_Datatype dtype, MPI_Op op );
    void reduce_scatter_block( const void *sendbuf, void *recvbuf, 
        int recvcount, MPI_Datatype dtype, MPI_Op op ) const;
    void reduce_scatter( const void *sendbuf, void *recvbuf, 
        const int recvcounts[], MPI_Datatype dtype, MPI_Op op ) const;
    void scan( const void *sendbuf, void *recvbuf, 
        int count, MPI_Datatype dtype, MPI_Op op ) const;
    void exscan( const void *sendbuf, void *recvbuf, 
        int count, MPI_Datatype dtype, MPI_Op op ) const;
    MPI_Request ibarrier() const;
    MPI_Request ibcast( 
        void *buf, int count, MPI_Datatype dtype, int root ) const;
    MPI_Request igather( 
        const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const;
    MPI_Request igatherv(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        MPI_Datatype recvtype, int root ) const;
    MPI_Request iscatter(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const;
    MPI_Request iscatterv(
        const void *sendbuf, const int sendcounts[], const int displs[], 
        MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root) const;
    MPI_Request iallgather( 
        const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype ) const;
    MPI_Request iallgatherv(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        MPI_Datatype recvtype ) const;
    MPI_Request ialltoall( 
        const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype ) const;
    MPI_Request ialltoallv( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], MPI_Datatype sendtype,
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        MPI_Datatype recvtype ) const;
    MPI_Request ialltoallw( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], const MPI_Datatype sendtypes[],
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        const MPI_Datatype recvtypes[] ) const;
    MPI_Request ireduce( const void *sendbuf, void *recvbuf, int count, 
        MPI_Datatype dtype, MPI_Op op, int root ) const;
    MPI_Request iallreduce( const void *sendbuf, void *recvbuf, int count, 
        MPI_Datatype dtype, MPI_Op op ) const;
    MPI_Request ireduce_scatter_block( const void *sendbuf, void *recvbuf, 
        int recvcount, MPI_Datatype dtype, MPI_Op op ) const;
    MPI_Request ireduce_scatter( const void *sendbuf, void *recvbuf, 
        const int recvcounts[], MPI_Datatype dtype, MPI_Op op ) const;
    MPI_Request iscan( const void *sendbuf, void *recvbuf, 
        int count, MPI_Datatype dtype, MPI_Op op ) const;
    MPI_Request iexscan( const void *sendbuf, void *recvbuf, 
        int count, MPI_Datatype dtype, MPI_Op op ) const;
protected:
    mpi_t _val;
    int _state;
};

inline 
_Comm::_Comm( mpi_t val, int state ) noexcept: _val(val), _state( state ){ }

inline 
_Comm::~_Comm() noexcept {
    if( _state != 0 && ! is_null() ){
        if( MPI_Comm_free( &_val ) != MPI_SUCCESS )
            ErrMPI::abort(1, emFLPFB);
    }
}

inline 
void _Comm::free( mpi_t &comm ){
    ErrMPI::check(  MPI_Comm_free(&comm), emFLPFB );
}

inline 
int _Comm::size() const {
    int _size;
    ErrMPI::check( MPI_Comm_size(_val, &_size), emFLPFB);
    return _size;
}

inline 
int _Comm::rank() const {
    int _rank;
    ErrMPI::check( MPI_Comm_rank(_val, &_rank), emFLPFB );
    return _rank;
}

inline 
auto _Comm::raw() const noexcept -> mpi_t { 
    return _val; 
}

inline 
bool _Comm::is_null() const noexcept { return _val == nullval(); }

inline 
bool _Comm::is_inter() const {
    int _flag;
    ErrMPI::check( MPI_Comm_test_inter(_val, &_flag), emFLPFB );
    return bool(_flag);
}

inline 
int _Comm::remote_size() const {
    int _size;
    ErrMPI::check( MPI_Comm_remote_size(_val, &_size), emFLPFB );
    return _size;
}

inline 
int _Comm::create_keyval( copy_attr_fn_t *copy_attr_fn, 
    del_attr_fn_t *delete_attr_fn, void *extra_state ){
    int keyval;
    ErrMPI::check( 
        MPI_Comm_create_keyval( 
            copy_attr_fn, delete_attr_fn, &keyval, extra_state ), emFLPFB );
    return keyval;
}

inline 
void _Comm::free_keyval( int *comm_keyval ) {
    ErrMPI::check( MPI_Comm_free_keyval( comm_keyval ), emFLPFB );
}

inline 
void _Comm::set_attr( int keyval, void *attr_val ) {
    ErrMPI::check( MPI_Comm_set_attr(_val, keyval, attr_val), emFLPFB );
}

inline 
bool _Comm::get_attr( int keyval, void *attr_val ) const {
    int flag;
    ErrMPI::check( 
        MPI_Comm_get_attr(_val, keyval, attr_val, &flag), emFLPFB );
    return bool(flag);
}

inline 
void _Comm::del_attr( int keyval ) {
    ErrMPI::check( MPI_Comm_delete_attr(_val, keyval), emFLPFB, 
        "  ... keyval=", keyval, '\n' );
}

inline 
auto _Comm::split( int color, int key) const -> mpi_t {
    mpi_t newcomm;
    ErrMPI::check( 
        MPI_Comm_split( _val, color, key, &newcomm ), emFLPFB );
    return newcomm;
}

inline 
auto _Comm::dup( )const -> mpi_t {
    mpi_t newcomm;
    ErrMPI::check( MPI_Comm_dup( _val, &newcomm ), emFLPFB );
    return newcomm;
}

inline 
auto _Comm::create( mpi_t comm, MPI_Group group ) -> mpi_t {
    mpi_t newcomm;
    ErrMPI::check( MPI_Comm_create(comm, group, &newcomm), emFLPFB );
    return newcomm;
}

inline 
auto _Comm::nullval() noexcept -> mpi_t { return MPI_COMM_NULL; }

inline 
auto _Comm::selfval() noexcept -> mpi_t { return MPI_COMM_SELF; }

inline 
auto _Comm::world() noexcept -> mpi_t { return MPI_COMM_WORLD; }

inline 
auto _Comm::create_inter( mpi_t local_comm, int local_leader, 
    mpi_t peer_comm, int remote_leader, int tag ) -> mpi_t
{
    mpi_t newcomm;
    ErrMPI::check( MPI_Intercomm_create( local_comm, local_leader, 
        peer_comm, remote_leader, tag, &newcomm), emFLPFB );
    return newcomm;
}

inline 
auto _Comm::merge_inter( mpi_t intercomm, int high ) -> mpi_t {
    mpi_t newcomm;
    ErrMPI::check( MPI_Intercomm_merge( intercomm, high, &newcomm ),
        emFLPFB );
    return newcomm;
}

inline 
MPI_Group _Comm::group() const{
    MPI_Group obj;
    ErrMPI::check( MPI_Comm_group(_val, &obj), emFLPFB );
    return obj;
}

inline 
MPI_Group _Comm::remote_group() const{
    MPI_Group obj;
    ErrMPI::check( MPI_Comm_remote_group(_val, &obj), emFLPFB );
    return obj;
}

inline 
void _Comm::dims_create( int nnodes, int ndims, int dims[] ){
    ErrMPI::check(
        MPI_Dims_create(nnodes, ndims, dims), emFLPFB, 
            "  ... nnodes=", nnodes, ", ndims=", ndims, '\n');
}

inline 
auto _Comm::cart_create( int ndims, const int dims[], const int periods[], 
    int reorder )const -> mpi_t 
{
    mpi_t comm_cart;
    ErrMPI::check( MPI_Cart_create( _val, ndims, dims, periods, reorder, 
        &comm_cart ), emFLPFB, "  ... ndims=", ndims, '\n' );
    return comm_cart;
}

inline 
auto _Comm::cart_sub( const int remain_dims[] )const -> mpi_t {
    mpi_t newcomm;
    ErrMPI::check(
        MPI_Cart_sub( _val, remain_dims, &newcomm ), emFLPFB);
    return newcomm;
}

inline 
int _Comm::cartdim_get()const{
    int ndims;
    ErrMPI::check( MPI_Cartdim_get(_val,&ndims), emFLPFB );
    return ndims;
}

inline 
void _Comm::cart_get( int maxdims, int dims[], int periods[], int coords[] )const{
    ErrMPI::check(
        MPI_Cart_get(_val, maxdims, dims, periods, coords), emFLPFB, 
        "  ... maxdims=", maxdims, '\n');
}

inline 
int _Comm::cart_rank( const int coords[] )const{
    int rank;
    ErrMPI::check(
        MPI_Cart_rank(_val, coords, &rank), emFLPFB);
    return rank;
}

inline 
void _Comm::cart_coords( int rank, int maxdims, int coords[] )const{
    ErrMPI::check( MPI_Cart_coords(_val, rank, maxdims, coords), 
        emFLPFB, "  ... rank=", rank, ", maxdims=", maxdims );
}

inline 
void _Comm::cart_shift( int direction, int disp, int *rank_src, 
    int *rank_dest )const{
    ErrMPI::check( MPI_Cart_shift(_val, direction, disp, 
        rank_src, rank_dest), emFLPFB, "  ... direction=", direction,
        ", disp=", disp, '\n' );
}

inline 
_Comm::mpi_t _Comm::graph_create(int nnodes, const int index[], 
    const int edges[], int reorder) const 
{
    mpi_t new_comm;
    ErrMPI::check(MPI_Graph_create(raw(), nnodes, index, edges, reorder, 
        &new_comm), emFLPFB, "  ... creation of graph (nnodes=", nnodes, 
        ") failed\n");
    return new_comm;
}

inline 
void _Comm::graphdims_get(int *nnodes, int *nedges) const {
    ErrMPI::check(MPI_Graphdims_get(raw(), nnodes, nedges), 
        emFLPFB, "  ... get graph dims failed\n");
}

inline 
void _Comm::graph_get(int maxindex, int maxedges, int index[], 
    int edges[]) const 
{
    ErrMPI::check(MPI_Graph_get(raw(), maxindex, maxedges, index, edges), 
        emFLPFB, "  ... get graph index and edges failed\n");
}

inline 
int _Comm::graph_neighbors_count(int rank) const {
    int cnt;
    ErrMPI::check(MPI_Graph_neighbors_count(raw(), rank, &cnt), 
        emFLPFB, "  ... get count failed (rank=", rank, ")\n");
    return cnt;
}

inline 
void _Comm::graph_neighbors(int rank, int maxneighbors, int neighbors[]) const {
    ErrMPI::check(MPI_Graph_neighbors(raw(), rank, maxneighbors, neighbors), 
        emFLPFB, "  ... get graph neighbors failed (rank=", rank, ")\n");
}

inline 
_Comm::mpi_t _Comm::dist_graph_create_adjacent(int indegree, 
    const int sources[], const int sourceweights[], int outdegree, 
    const int destinations[], const int destweights[], MPI_Info info, 
    int reorder) const
{
    mpi_t new_comm;
    ErrMPI::check(MPI_Dist_graph_create_adjacent(raw(), indegree, sources, 
        sourceweights, outdegree, destinations, destweights,
        info, reorder, &new_comm), emFLPFB, 
        "  ... creation failed (indegree=", 
        indegree, ", outdegree=", outdegree, ")\n");
    return new_comm;
}

inline 
_Comm::mpi_t _Comm::dist_graph_create(int n, const int sources[], 
    const int degrees[], const int destinations[], const int weights[], 
    MPI_Info info, int reorder) const
{
    mpi_t new_comm;
    ErrMPI::check(MPI_Dist_graph_create(raw(), n, sources, degrees, 
        destinations, weights, info, reorder, &new_comm), emFLPFB, 
        "  ... creation failed (n=", n, ")\n");
    return new_comm;
}

inline
void _Comm::dist_graph_neighbors_count(int *indegree, int *outdegree, 
    int *weighted) const
{
    ErrMPI::check(MPI_Dist_graph_neighbors_count(raw(), indegree, 
        outdegree, weighted), emFLPFB, "  ... getting neighbors count "
        "failed\n");
}

inline
void _Comm::dist_graph_neighbors(int maxindegree, int sources[],
    int sourceweights[], int maxoutdegree, int destinations[], 
    int destweights[]) const
{
    ErrMPI::check(MPI_Dist_graph_neighbors(raw(), maxindegree, sources, 
        sourceweights, maxoutdegree, destinations, destweights), emFLPFB, 
        "  ... getting neighbors failed (maxindegree=", maxindegree, 
        ", maxoutdegree=", maxoutdegree, ")\n");
}

inline 
void _Comm::cart_map(int ndims, const int dims[], const int periods[], 
    int *newrank) const
{
    ErrMPI::check( MPI_Cart_map(raw(), ndims, dims, periods, newrank), 
        emFLPFB, "  ... reordering of processes failed (ndims=", ndims, ")\n");
}

inline 
void _Comm::graph_map(int nnodes, const int index[], 
    const int edges[], int *newrank) const
{
    ErrMPI::check( MPI_Graph_map(nnodes, nnodes, index, edges, newrank), 
        emFLPFB, "  ... reordering of processes failed (nnodes=", nnodes, 
        ")\n" );
}

inline 
int _Comm::topo_test()const{
    int status;
    ErrMPI::check( MPI_Topo_test(_val, &status), emFLPFB );
    return status;
}

inline
void _Comm::neighbor_allgather(const void* sendbuf, int sendcount, 
    MPI_Datatype sendtype, void* recvbuf, int recvcount, 
    MPI_Datatype recvtype) const
{
    ErrMPI::check(MPI_Neighbor_allgather(sendbuf, sendcount, sendtype,
        recvbuf, recvcount, recvtype, raw()), emFLPFB, 
        "  ... allgather failed (sendcount=", sendcount, 
        ", recvcount=", recvcount, ")\n");
}

inline
void _Comm::neighbor_allgatherv(const void* sendbuf, int sendcount, 
    MPI_Datatype sendtype, void* recvbuf, const int recvcounts[], 
    const int displs[], MPI_Datatype recvtype) const
{
    ErrMPI::check(MPI_Neighbor_allgatherv(sendbuf, sendcount, sendtype,
        recvbuf, recvcounts, displs, recvtype, raw()), emFLPFB, 
        "  ... allgatherv failed (sendcount=", sendcount, ")\n");
}
    
inline
void _Comm::neighbor_alltoall(const void* sendbuf, int sendcount, 
    MPI_Datatype sendtype, void* recvbuf, int recvcount, 
    MPI_Datatype recvtype) const
{
    ErrMPI::check(MPI_Neighbor_alltoall(sendbuf, sendcount, sendtype,
        recvbuf, recvcount, recvtype, raw()), emFLPFB, 
        "  ... alltoall failed (sendcount=", sendcount, 
        ", recvcount=", recvcount, ")\n");
}
    
inline
void _Comm::neighbor_alltoallv(const void* sendbuf, const int sendcounts[], 
    const int sdispls[], MPI_Datatype sendtype, void* recvbuf, 
    const int recvcounts[], const int rdispls[], 
    MPI_Datatype recvtype) const
{
    ErrMPI::check(MPI_Neighbor_alltoallv(sendbuf, sendcounts, sdispls, 
        sendtype, recvbuf, recvcounts, rdispls, recvtype, raw()), 
        emFLPFB,  "  ... alltoallv failed\n");
}
    
inline
void _Comm::neighbor_alltoallw(const void* sendbuf, const int sendcounts[], 
    const MPI_Aint sdispls[], const MPI_Datatype sendtypes[], void* recvbuf, 
    const int recvcounts[], const MPI_Aint rdispls[], 
    const MPI_Datatype recvtypes[]) const
{
    ErrMPI::check(MPI_Neighbor_alltoallw(sendbuf, sendcounts, sdispls, 
        sendtypes, recvbuf, recvcounts, rdispls, recvtypes, raw()), emFLPFB, 
        "  ... alltoallw failed\n");
}
    
inline
MPI_Request _Comm::ineighbor_allgather(const void* sendbuf, int sendcount, 
    MPI_Datatype sendtype, void* recvbuf, 
    int recvcount, MPI_Datatype recvtype) const
{
    MPI_Request req;
    ErrMPI::check(MPI_Ineighbor_allgather(sendbuf, sendcount, sendtype,
        recvbuf, recvcount, recvtype, raw(), &req), emFLPFB, 
        "  ... iallgather failed (sendcount=", sendcount, 
        ", recvcount=", recvcount, ")\n");
    return req;
}

inline
MPI_Request _Comm::ineighbor_allgatherv(const void* sendbuf, int sendcount, 
    MPI_Datatype sendtype, void* recvbuf, const int recvcounts[], 
    const int displs[], MPI_Datatype recvtype) const
{
    MPI_Request req;
    ErrMPI::check(MPI_Ineighbor_allgatherv(sendbuf, sendcount, sendtype,
        recvbuf, recvcounts, displs, recvtype, raw(), &req), emFLPFB, 
        "  ... iallgatherv failed (sendcount=", sendcount, ")\n");
    return req;
}

inline
MPI_Request _Comm::ineighbor_alltoall(const void* sendbuf, int sendcount,
    MPI_Datatype sendtype, void* recvbuf, int recvcount,
    MPI_Datatype recvtype) const
{
    MPI_Request req;
    ErrMPI::check(MPI_Ineighbor_alltoall(sendbuf, sendcount, sendtype,
        recvbuf, recvcount, recvtype, raw(), &req), emFLPFB, 
        "  ... ialltoall failed (sendcount=", sendcount, 
        ", recvcount=", recvcount, ")\n");
    return req;
}

inline
MPI_Request _Comm::ineighbor_alltoallv(const void* sendbuf, 
    const int sendcounts[], 
    const int sdispls[], MPI_Datatype sendtype, void* recvbuf, 
    const int recvcounts[], const int rdispls[], 
    MPI_Datatype recvtype) const
{
    MPI_Request req;
    ErrMPI::check(MPI_Ineighbor_alltoallv(sendbuf, sendcounts, sdispls, 
        sendtype, recvbuf, recvcounts, rdispls, recvtype, raw(), &req), 
        emFLPFB,  "  ... ialltoallv failed\n");
    return req;
}

inline
MPI_Request _Comm::ineighbor_alltoallw(const void* sendbuf, 
    const int sendcounts[],
    const MPI_Aint sdispls[], const MPI_Datatype sendtypes[],
    void* recvbuf, const int recvcounts[],
    const MPI_Aint rdispls[], const MPI_Datatype recvtypes[]) const
{
    MPI_Request req;
    ErrMPI::check(MPI_Ineighbor_alltoallw(sendbuf, sendcounts, sdispls, 
        sendtypes, recvbuf, recvcounts, rdispls, recvtypes, raw(), &req), 
        emFLPFB, "  ... alltoallw failed\n");
    return req;
}

inline 
void _Comm::send( const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag ) const{
    ErrMPI::check( 
        MPI_Send(buff, count, dtype, dest, tag, _val), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
}

inline 
void _Comm::bsend(const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag ) const {
    ErrMPI::check( MPI_Bsend(buff, count, dtype, dest, tag, _val),
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
}

inline 
void _Comm::ssend(const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag ) const {
    ErrMPI::check( MPI_Ssend(buff, count, dtype, dest, tag, _val),
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
}

inline 
void _Comm::rsend(const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag ) const {
    ErrMPI::check( MPI_Rsend(buff, count, dtype, dest, tag, _val),
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
}

inline 
MPI_Status _Comm::recv( void *buff, int count, MPI_Datatype dtype, 
    int src, int tag ) const{
    MPI_Status st;
    ErrMPI::check( MPI_Recv(buff, count, dtype, src, tag, _val, &st), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", src=", src, ", tag=", tag, "\n");
    return st;
}

inline 
MPI_Request _Comm::isend( const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag ) const{
    MPI_Request rq;
    ErrMPI::check( MPI_Isend(buff, count, dtype, dest, tag, _val, &rq), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
    return rq;
}

inline 
MPI_Request _Comm::ibsend( const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag ) const{
    MPI_Request rq;
    ErrMPI::check( MPI_Ibsend(buff, count, dtype, dest, tag, _val, &rq), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
    return rq;
}

inline 
MPI_Request _Comm::issend( const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag ) const{
    MPI_Request rq;
    ErrMPI::check( MPI_Issend(buff, count, dtype, dest, tag, _val, &rq), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
    return rq;
}

inline 
MPI_Request _Comm::irsend( const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag ) const{
    MPI_Request rq;
    ErrMPI::check( MPI_Irsend(buff, count, dtype, dest, tag, _val, &rq), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
    return rq;
}

inline 
MPI_Request _Comm::irecv( void *buff, int count, MPI_Datatype dtype, 
    int src, int tag ) const
{
    MPI_Request rq;
    ErrMPI::check(MPI_Irecv(buff, count, dtype, src, tag, _val, &rq),
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", src=", src, ", tag=", tag, "\n");
    return rq;
}

inline
MPI_Request _Comm::send_init(const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag) const
{
    MPI_Request rq;
    ErrMPI::check(
        MPI_Send_init(buff, count, dtype, dest, tag, _val, &rq), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
    return rq;
}

inline
MPI_Request _Comm::bsend_init(const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag) const
{
    MPI_Request rq;
    ErrMPI::check(
        MPI_Bsend_init(buff, count, dtype, dest, tag, _val, &rq), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
    return rq;
}

inline
MPI_Request _Comm::ssend_init(const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag) const
{
    MPI_Request rq;
    ErrMPI::check(
        MPI_Ssend_init(buff, count, dtype, dest, tag, _val, &rq), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
    return rq;
}

inline
MPI_Request _Comm::rsend_init(const void *buff, int count, MPI_Datatype dtype, 
    int dest, int tag) const
{
    MPI_Request rq;
    ErrMPI::check(
        MPI_Rsend_init(buff, count, dtype, dest, tag, _val, &rq), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", dst=", dest, ", tag=", tag, "\n");
    return rq;
}

inline
MPI_Request _Comm::recv_init(void *buff, int count, MPI_Datatype dtype, 
    int src, int tag) const
{
    MPI_Request rq;
    ErrMPI::check(
        MPI_Recv_init(buff, count, dtype, src, tag, _val, &rq), 
        emFLPFB, "  ... buff=", buff, ", count=", count, 
        ", src=", src, ", tag=", tag, "\n");
    return rq;
}

inline 
MPI_Status _Comm::sendrecv(const void *sendbuff, int sendcount, 
    MPI_Datatype sendtype, int dest, int sendtag, void *recvbuff, 
    int recvcount, MPI_Datatype recvtype, int src, int recvtag) 
{
    MPI_Status st;
    ErrMPI::check( MPI_Sendrecv(sendbuff, sendcount, sendtype, 
        dest, sendtag, recvbuff, recvcount, recvtype, 
        src, recvtag, _val, &st), emFLPFB);
    return st;
}

inline 
MPI_Status _Comm::sendrecv_replace(void *buff, int count, MPI_Datatype dtype, 
    int dest, int sendtag, int src, int recvtag) {
    MPI_Status st;
    ErrMPI::check( MPI_Sendrecv_replace(buff, count, dtype, dest, 
        sendtag, src, recvtag, _val, &st), emFLPFB );
    return st;
}

inline MPI_Status _Comm::iprobe(int src, int tag, int &flag) const {
    MPI_Status st;
    ErrMPI::check(
        MPI_Iprobe(src, tag, _val, &flag, &st), 
            emFLPFB, "  ... src=", src, ", tag=", tag, '\n');
    return st;
}

inline MPI_Status _Comm::probe(int src, int tag) const 
{
    MPI_Status st;
    ErrMPI::check(MPI_Probe(src, tag, _val, &st), 
        emFLPFB, "  ... src=", src, ", tag=", tag, '\n');
    return st;
}

inline MPI_Status _Comm::improbe(
    int src, int tag, int &flag, MPI_Message &message) const 
{
    MPI_Status st;
    ErrMPI::check(
        MPI_Improbe(src, tag, _val, &flag, &message, &st), 
            emFLPFB, "  ... src=", src, ", tag=", tag, '\n');
    return st;
}

inline MPI_Status _Comm::mprobe(int src, int tag, MPI_Message &message) const {
    MPI_Status st;
    ErrMPI::check(MPI_Mprobe(src, tag, _val, &message, &st), 
        emFLPFB, "  ... src=", src, ", tag=", tag, '\n');
    return st;
}

inline MPI_Status _Comm::mrecv( void *buff, int count, MPI_Datatype dtype, 
    MPI_Message &message)
{
    MPI_Status st;
    ErrMPI::check(MPI_Mrecv(buff, count, dtype, &message, &st), emFLPFB, 
        "  ... buff=", buff, ", count=", count, '\n');
    return st;
}

inline MPI_Request _Comm::imrecv(void *buff, int count, MPI_Datatype dtype, 
    MPI_Message &message)
{
    MPI_Request req;
    ErrMPI::check(MPI_Imrecv(buff, count, dtype, &message, &req), emFLPFB, 
        "  ... buff=", buff, ", count=", count, '\n');
    return req;
}


inline 
void _Comm::barrier() const{
    ErrMPI::check( 
        MPI_Barrier(_val), emFLPFB );
}

inline 
void _Comm::bcast( void *buf, int count, MPI_Datatype dtype, int root ) const{
    ErrMPI::check( 
        MPI_Bcast(buf, count, dtype, root, _val), emFLPFB );
}

inline 
void _Comm::gather( const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
    void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const{
    ErrMPI::check( 
        MPI_Gather(sendbuf, sendcount, sendtype, 
            recvbuf, recvcount, recvtype, root, _val), emFLPFB );
}

inline 
void _Comm::gatherv(
    const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    MPI_Datatype recvtype, int root ) const{
    ErrMPI::check( 
        MPI_Gatherv(sendbuf, sendcount, sendtype, 
            recvbuf, recvcounts, displs, recvtype, root, _val), emFLPFB );
}

inline 
void _Comm::scatter(
    const void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const{
    ErrMPI::check( 
        MPI_Scatter(sendbuf, sendcount, sendtype, 
            recvbuf, recvcount, recvtype, root, _val), emFLPFB );
}

inline 
void _Comm::scatterv(
    const void *sendbuf, const int sendcounts[], const int displs[], 
    MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype, int root) const{
    ErrMPI::check( 
        MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, 
            recvbuf, recvcount, recvtype, root, _val), emFLPFB );
}

inline 
void _Comm::allgather( const void *sendbuf, int sendcount, 
    MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype ) const
{
    ErrMPI::check( 
    MPI_Allgather(sendbuf, sendcount, sendtype, 
        recvbuf, recvcount, recvtype, _val), emFLPFB );
}

inline 
void _Comm::allgatherv(
    const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    MPI_Datatype recvtype ) const{
    ErrMPI::check( 
        MPI_Allgatherv(sendbuf, sendcount, sendtype, 
            recvbuf, recvcounts, displs, recvtype, _val), emFLPFB );
}

inline 
void _Comm::alltoall( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype ) const{
    ErrMPI::check( 
        MPI_Alltoall(sendbuf, sendcount, sendtype, 
            recvbuf, recvcount, recvtype,_val), emFLPFB );
}

inline 
void _Comm::alltoallv( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], MPI_Datatype sendtype,
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    MPI_Datatype recvtype ) const{
    ErrMPI::check( 
        MPI_Alltoallv(sendbuf, sendcounts, senddispls, sendtype, 
            recvbuf, recvcounts, recvdispls, recvtype, _val), emFLPFB );
}

inline 
void _Comm::alltoallw( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], const MPI_Datatype sendtypes[],
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const MPI_Datatype recvtypes[] ) const{
    ErrMPI::check( 
        MPI_Alltoallw(sendbuf, sendcounts, senddispls, sendtypes, 
            recvbuf, recvcounts, recvdispls, recvtypes, _val), emFLPFB );
}

inline 
void _Comm::reduce( const void *sendbuf, void *recvbuf, int count, 
    MPI_Datatype dtype, MPI_Op op, int root ) const{
    ErrMPI::check(
        MPI_Reduce( sendbuf, recvbuf, count, dtype, 
            op, root, _val ), emFLPFB);
}

inline 
void _Comm::allreduce( const void *sendbuf, void *recvbuf, int count, 
    MPI_Datatype dtype, MPI_Op op ) const{
    ErrMPI::check(
        MPI_Allreduce( sendbuf, recvbuf, count, dtype, 
            op, _val ), emFLPFB);
}

inline 
void _Comm::reduce_local( const void *inbuf, void *inoutbuf, int count, 
    MPI_Datatype dtype, MPI_Op op ){
    ErrMPI::check(
        MPI_Reduce_local( inbuf, inoutbuf, count, dtype, op ), emFLPFB);
}

inline 
void _Comm::reduce_scatter_block( const void *sendbuf, void *recvbuf, 
    int recvcount, MPI_Datatype dtype, MPI_Op op ) const{
    ErrMPI::check(
        MPI_Reduce_scatter_block( sendbuf, recvbuf, recvcount, dtype, 
            op, _val ), emFLPFB);
}

inline 
void _Comm::reduce_scatter( const void *sendbuf, void *recvbuf, 
    const int recvcounts[], MPI_Datatype dtype, MPI_Op op ) const{
    ErrMPI::check(
        MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts,
            dtype, op, _val), emFLPFB);
}

inline 
void _Comm::scan( const void *sendbuf, void *recvbuf, 
    int count, MPI_Datatype dtype, MPI_Op op ) const{
    ErrMPI::check(
        MPI_Scan(sendbuf, recvbuf, count, dtype, op, _val), emFLPFB );
}

inline 
void _Comm::exscan( const void *sendbuf, void *recvbuf, 
    int count, MPI_Datatype dtype, MPI_Op op ) const{
    ErrMPI::check(
        MPI_Exscan( sendbuf, recvbuf, count, dtype, op, _val ), emFLPFB );
}

inline 
MPI_Request _Comm::ibarrier() const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Ibarrier(_val, &req), emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::ibcast( 
    void *buf, int count, MPI_Datatype dtype, int root ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Ibcast( buf, count, dtype, root, _val, &req ), emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::igather( 
    const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
    void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Igather( sendbuf, sendcount, sendtype, 
            recvbuf, recvcount, recvtype, root, _val, &req ), emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::igatherv(
    const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    MPI_Datatype recvtype, int root ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Igatherv( sendbuf, sendcount, sendtype, 
            recvbuf, recvcounts, displs, recvtype, root, 
            _val, &req ), emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::iscatter(
    const void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Iscatter( sendbuf, sendcount, sendtype, 
        recvbuf, recvcount, recvtype, root, _val, &req ), emFLPFB
    );
    return req;
}

inline 
MPI_Request _Comm::iscatterv(
    const void *sendbuf, const int sendcounts[], const int displs[], 
    MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype, int root) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Iscatterv( sendbuf, sendcounts, displs, sendtype, 
        recvbuf, recvcount, recvtype, root, _val, &req ), emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::iallgather( 
    const void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Iallgather( sendbuf, sendcount, sendtype,
            recvbuf, recvcount, recvtype, _val, &req ), emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::iallgatherv(
    const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
    void *recvbuf, const int recvcounts[], const int displs[],
    MPI_Datatype recvtype ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Iallgatherv( sendbuf, sendcount, sendtype,
            recvbuf, recvcounts, displs, recvtype, _val, &req ), emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::ialltoall( 
    const void *sendbuf, int sendcount, MPI_Datatype sendtype,
    void *recvbuf, int recvcount, MPI_Datatype recvtype ) const {
    MPI_Request req;
    ErrMPI::check(
        MPI_Ialltoall( sendbuf, sendcount, sendtype,
            recvbuf, recvcount, recvtype, _val, &req ), emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::ialltoallv( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], MPI_Datatype sendtype,
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    MPI_Datatype recvtype ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Ialltoallv( sendbuf, sendcounts, senddispls, sendtype,
            recvbuf, recvcounts, recvdispls, recvtype, _val, &req ), 
            emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::ialltoallw( const void *sendbuf, const int sendcounts[], 
    const int senddispls[], const MPI_Datatype sendtypes[],
    void *recvbuf, const int recvcounts[], const int recvdispls[], 
    const MPI_Datatype recvtypes[] ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Ialltoallw( sendbuf, sendcounts, senddispls, sendtypes,
            recvbuf, recvcounts, recvdispls, recvtypes, _val, &req ), 
            emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::ireduce( const void *sendbuf, void *recvbuf, int count, 
    MPI_Datatype dtype, MPI_Op op, int root ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Ireduce( sendbuf, recvbuf, count, dtype, op, root, _val, &req ), 
            emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::iallreduce( const void *sendbuf, void *recvbuf, int count, 
    MPI_Datatype dtype, MPI_Op op ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Iallreduce( sendbuf, recvbuf, count, dtype, op, _val, &req ), 
            emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::ireduce_scatter_block( const void *sendbuf, void *recvbuf, 
    int recvcount, MPI_Datatype dtype, MPI_Op op ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Ireduce_scatter_block( 
            sendbuf, recvbuf, recvcount, dtype, op, _val, &req ), 
            emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::ireduce_scatter( const void *sendbuf, void *recvbuf, 
    const int recvcounts[], MPI_Datatype dtype, MPI_Op op ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Ireduce_scatter( 
            sendbuf, recvbuf, recvcounts, dtype, op, _val, &req ), 
            emFLPFB );
    return req;   
}

inline 
MPI_Request _Comm::iscan( const void *sendbuf, void *recvbuf, 
    int count, MPI_Datatype dtype, MPI_Op op ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Iscan( 
            sendbuf, recvbuf, count, dtype, op, _val, &req ), 
            emFLPFB );
    return req;
}

inline 
MPI_Request _Comm::iexscan( const void *sendbuf, void *recvbuf, 
    int count, MPI_Datatype dtype, MPI_Op op ) const{
    MPI_Request req;
    ErrMPI::check(
        MPI_Iexscan( 
            sendbuf, recvbuf, count, dtype, op, _val, &req ), 
            emFLPFB );
    return req;
}

} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_RAW_COMM_H_