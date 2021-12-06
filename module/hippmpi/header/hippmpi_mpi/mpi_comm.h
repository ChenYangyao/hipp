/**
create: Yangyao CHEN, 2020/01/18
    [write   ] Comm - the high-level communicator interface for MPI system.
*/ 
#ifndef _HIPPMPI_MPI_COMM_H_
#define _HIPPMPI_MPI_COMM_H_

#include "mpi_obj_base.h"
#include "mpi_raw_comm.h"
#include "mpi_group.h"
#include "mpi_datapacket.h"
#include "mpi_op.h"
#include "mpi_status.h"
#include "mpi_message.h"
#include "mpi_requests.h"
#include "mpi_win.h"

namespace HIPP::MPI {

class Comm: public MPIObj<_Comm> {
public:
    typedef MPIObj<_Comm> _obj_base_t;
    using _obj_base_t::_obj_base_t;
    typedef std::function< bool(Comm &oldcomm, 
        int keyval, void *extra_state, void *attr_val, void *&attr_val_out) > 
        copy_attr_fn_t;
    typedef std::function< void(Comm &comm, 
        int keyval, void *attr_val, void *extra_state)>
        del_attr_fn_t;
    static const copy_attr_fn_t NULL_COPY_FN, DUP_FN;
    static const del_attr_fn_t NULL_DEL_FN;

    /**
    ``info()`` prints a short (``fmt_cntl=1``) or verbose (``fmt_cntl=1``)
    description about the instance to the stream ``os``.

    Operator ``<<`` is equivalent to ``info()`` with ``fmt_cntl=0``.

    The stream ``os`` itself is returned.
    */
    ostream &info( ostream &os = cout, int fmt_cntl = 1) const;
    friend ostream & operator<< (ostream &os, const Comm &comm);

    /** 
    Free the communicator and set the instance to the null value 
    (i.e., is_null()->true). 
    ``free()`` may be called on any object (even on a null value or other 
    predefined communicators) at any time.
    */
    void free() noexcept;

    /**
    Return the basic properties of the communicator.
    @size: number of processes in the process group of the communicator.
    @rank: the identifier of the calling process among all processes. It must
    be in the range ``[0, size)``.
    @is_null: whether or not the communicator is a null value 
    (internally, ``MPI_PROC_NULL``).
    @is_inter: whether or not the communicator is an inter-communicator.
    @remote_size: number of processes in the remote group for an 
    inter-communicator.
    For an inter-communicator, size and rank are the values of the local group.
    */
    int size() const;
    int rank() const;
    bool is_null() const;
    bool is_inter() const;
    int remote_size() const;

    /**
     * attribute caching
     */
    static int create_keyval( copy_attr_fn_t copy_attr_fn = NULL_COPY_FN,
        del_attr_fn_t del_attr_fn = NULL_DEL_FN, void *extra_state = nullptr );
    static void free_keyval( int &keyval );
    Comm & set_attr( int keyval, void *attr_val );
    bool get_attr( int keyval, void * &attr_val ) const;
    Comm & del_attr( int keyval );

    template<typename AttrT>
    static int create_keyval();
    template<typename AttrT>
    bool get_attr( int keyval, AttrT * &attr_val ) const;

    /**
     * communicator management
     */
    Comm split( int color, int key = 0 )const;
    Comm dup() const;
    Comm create( const Group &group) const;
    static Comm world() noexcept;
    static Comm selfval() noexcept;
    static Comm nullval() noexcept;
    Comm create_inter( int local_leader, const Comm &peer_comm, 
        int remote_leader, int tag );
    Comm merge_inter( int high );

    /**
    Access the group of processes in the communicator.
    */
    const Group group() const;
    Group group();
    const Group remote_group() const;
    Group remote_group();

    /**
    Return the topology type of the communicator. Possible values are 
    GRAPH, CART, DIST_GRAPH, or UNDEFINED (for no topology).
    */
    int topo_test() const;

    /**
    Cartesian topology creation method.
    
    ``dims_create()`` is a helpful function to determine the number of processes 
    at each dimension from the number of process available in total, ``nnodes``, 
    and disired number of dimensions, ``ndims``. 
    ``dims`` is an in-and-out argument. Positive ``dims[i]`` will not 
    changed on exit; zero ``dims[i]`` will be changed to a suitable value. 
    Changed elements in ``dims`` will be in an non-increasing order, and they 
    are as close as possible. It is erroneous that ``nnodes`` is not multiple of 
    prod(dims[i]) (for all dims[i] != 0).
    It is valid to pass a `dims` with length not equal to ``ndims``. ``dims`` is 
    resized to ``ndims``, padded with 0 if necessary.

    **Example:** calls of ``Comm::cart_create(nnodes, ndims, dims)`` give the 
    following results.

    .. table::
        :class: tight-table

        ================= ==========================
        nnodes and ndims   input and output dims  
        ================= ==========================
        6, 2              (0,0) -> (3,2)
        7, 2              (0,0) -> (7,1)
        6, 3              (0,3,0) -> (2,3,1)
        7, 3              (0,3,0) -> erroneous call 
        ================= ==========================

    ``cart_create()`` creates a new communicator with number of processes at 
    each dimension specified by ``dims``, whether periodic at each dimension 
    specified by ``periods``. If ``reorder`` is not zero then the implementation 
    is allowed to reorder the ranks of the processes and then put then on 
    cartesian grids. Processes are put by row-major order according to their 
    ranks. If size of the origin communicator is larger than needed, processes 
    that is not put on the grids get null communicater as returned by 
    :expr:`Comm::nullval()`.

    ``cart_sub()`` decomposes the original cartesian topology into several 
    sub-cartesian communicators.
    The remaining dimensions are passed as ``remain_dims``. Decomposition is 
    performed at all non-remaining direction.
    */
    static void dims_create(int nnodes, int ndims, vector<int> &dims);
    Comm cart_create(const vector<int> &dims, 
        const vector<int> &periods, int reorder = 1) const;
    Comm cart_sub(const vector<int> &remain_dims) const;

    /**
    Cartesian topology meta-info inquiry methods. 
    
    The meta-info of a communicator with Cartesian topology may be inquired with
    the following methods.

    ``cartdim_get()`` returns the number of dimensions. 
    
    ``cart_get()`` returns number of processes at each dimension, whether each 
    dimension is periodic, and the coordinates of the calling 
    process, into args ``dims``, ``periods`` and ``coords``, respectively. The 
    arguments are auto-resized to ``cartdim_get()``.
    The second overload fills the results into contiguous buffers.

    ``cart_rank()`` accepts coordinates ``coords`` in the topology and return 
    its ``rank`` in the communicator. For periodic dimension, ``coords[i]`` is 
    shifted to valid range, otherwise an out-of-range ``coords[i]`` is 
    erroneous. For zero-dimensional topology, ``coords`` is not significant and 
    the call returns 0.
    The second overload take a contiguous buffer as input.

    ``cart_coords()`` converts the ``rank`` in the communicator into the 
    coordinates on the process grid.
    The second overload fills the result into the contiguous buffer.

    ``cart_shift()`` find the neighbor ranks of the calling process at dimension 
    specified by ``direction`` and displacement specified by ``disp``. 
    Return the ranks of the processes offset by ``-disp`` and ``disp`` at this 
    dimension into ``rank_src``, ``rank_dest``, respectively.
    The second overload returns the ranks as ``std::pair``.
    */
    int cartdim_get() const;
    void cart_get(vector<int> &dims, vector<int> &periods, 
        vector<int> &coords) const;
    void cart_get(ContiguousBuffer<int> dims, ContiguousBuffer<int> periods, 
        ContiguousBuffer<int> coords) const;

    int cart_rank(const vector<int> &coords) const;
    int cart_rank(ContiguousBuffer<const int> coords) const;

    vector<int> cart_coords(int rank) const;
    void cart_coords(int rank, ContiguousBuffer<int> coords) const;

    void cart_shift(int direction, int disp, int &rank_src, 
        int &rank_dest) const;
    std::pair<int, int> cart_shift(int direction, int disp = 1) const;

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

    The second overload assumes that ``nnodes`` is taken from ``index``.
    */
    Comm graph_create(int nnodes, const int index[], const int edges[], 
        int reorder = 1) const;
    Comm graph_create(ContiguousBuffer<const int> index, 
        ContiguousBuffer<const int> edges, int reorder = 1) const;
     
    /**
    Graph topology meta-info inquiry methods.
    
    graphdims_get(): return the number of nodes and edges which are correct 
    input to graph_get().

    graph_get(): returns index and edges. Overloads are:
    (1): ``maxindex`` and ``maxedges`` are maximal capabilities of the two 
    arrays.
    (2): the same as (1) but ``maxindex`` and ``maxedges`` are taken from the 
    buffers themselves.
    (3): return the index and edges.

    graph_neighbors_count(): return the number of neighbors near the process
    with given ``rank``.

    graph_neighbors(): returns the ranks of neighbors near the process with 
    given ``rank``. Overloads are:
    (1): ``maxneighbors`` indicates the maximal capability of array 
    ``neighbors``.
    (2): the same as (1) but ``maxneighbors`` is taken from the buffer itself.
    (3): return the ranks of neighbors.
    */
    std::pair<int, int> graphdims_get() const;

    void graph_get(int maxindex, int maxedges, int index[], int edges[]) const;
    void graph_get(ContiguousBuffer<int> index, 
        ContiguousBuffer<int> edges) const;
    std::pair<vector<int>, vector<int> > graph_get() const;
    
    int graph_neighbors_count(int rank) const;
    
    void graph_neighbors(int rank, int maxneighbors, int neighbors[]) const;
    void graph_neighbors(int rank, ContiguousBuffer<int> neighbors) const;
    vector<int> graph_neighbors(int rank) const;

    /**
    Distributed graph topology creation function.
    
    Each process specifies only a subset of all edges. All processes must have 
    the same ``reorder`` and ``info`` arguments. The new communicator has the 
    same size with the old.

    ``info`` may (or may not) be used by the implementation to optimize the 
    process mapping (e.g., interpretation of weights, reordering quality, and 
    time limit on the graph management). Info::nullval() is always a valid 
    argument.
    
    The weights of edges do not have standard semantics, but should be 
    non-negative and usually indicate the communication intensity. 
    Edge multiplicity may also hint the communication intensity. 

    Isolated process is allowed. A edge may has multiplicity > 1 with arbitrary
    order of weights.

    ``UNWEIGHTED`` may be used in all processes at the same time, indicating 
    no weighting. If a degree is 0, the weights array is not modified (may use 
    ``WEIGHTS_EMPTY``, but should not use NULL because UNWEIGHTED may be 
    ``NULL``).
    
    If ``reorder = true``, allow reordering the processes and then making 
    topology assignment.

    dist_graph_create_adjacent(): each process specifies all its incoming and
    outgoing edges. The overhead of this call is smaller than 
    dist_graph_create(). sources and destinations must be consistent in any 
    linked pair, with the same weights. Overloads are:
    (1): the standard call.
    (2): degrees are taken from ``sources`` and ``destinations``, respectively.
    (3): the same as (2) but take weights to be ``WEIGHTS_EMPTY``.

    dist_graph_create(): each process indicates possible directed edges between 
    process pairs in the desired graph. This call is more flexible but usually
    has more overhead. 
    ``n`` is the number of source nodes. ``sources`` are the ranks of source 
    processes and ``degrees`` are their numbers of destinations. 
    ``destinations`` are flatten-joined destination ranks of all source 
    processes.
    ``sources`` and ``destinations`` in any process may have repeated items. The
    ordering does not matter. Overloads are:
    (1): the standard call.
    (2): the same as (1) but ``n`` is taken from ``sources``.
    (3): the same as (2) but assume weights to be ``WEIGHTS_EMPTY``.
    */
    Comm dist_graph_create_adjacent(int indegree, const int sources[], 
        const int sourceweights[], int outdegree, const int destinations[], 
        const int destweights[], const Info &info = Info::nullval(), 
        int reorder = 1) const;
    Comm dist_graph_create_adjacent(
        ContiguousBuffer<const int> sources, 
        ContiguousBuffer<const int> sourceweights,
        ContiguousBuffer<const int> destinations,
        ContiguousBuffer<const int> destweights,
        const Info &info = Info::nullval(), 
        int reorder = 1) const;
    Comm dist_graph_create_adjacent(
        ContiguousBuffer<const int> sources, 
        ContiguousBuffer<const int> destinations,
        const Info &info = Info::nullval(), 
        int reorder = 1) const;

    Comm dist_graph_create(int n, const int sources[], const int degrees[], 
        const int destinations[], const int weights[], 
        const Info &info=Info::nullval(), int reorder = 1) const;
    Comm dist_graph_create(
        ContiguousBuffer<const int> sources, 
        ContiguousBuffer<const int> degrees,
        ContiguousBuffer<const int> destinations,
        ContiguousBuffer<const int> weights,
        const Info &info=Info::nullval(), int reorder = 1) const;
    Comm dist_graph_create(
        ContiguousBuffer<const int> sources, 
        ContiguousBuffer<const int> degrees,
        ContiguousBuffer<const int> destinations,
        const Info &info=Info::nullval(), int reorder = 1) const;

    /**
    Distributed graph topology meta-info inquiry methods.

    dist_graph_neighbors_count(): get the in and out degrees, and whether the
    graph is weighted, of the caller process. 
    The degrees take into account the edge specifications in all 
    processes.
    If ``UNWEIGHTED`` is used during the creation, the graph is unweighted.

    dist_graph_neighbors(): get neighbors. An edge with multiplicity > 1 
    results multiple items to the arguments with defining ordering.

    If the distributed graph is created with ``dist_graph_create_adjacent()``
    the returned arrays follow the same order passed to the creation. 
    Otherwise the only guarantee is that multiple calls of this method on 
    the same communicator return the same results.

    Each of the weight arguments may be ``UNWEIGHTED``. If 
    ``UNWEIGHTED`` is used during topology creation, the weight arguments 
    are not modified.
    ``maxindegree`` or ``maxoutdegree`` may be less than the actual degree. Then
    only a part of the neighbors are returned into the arguments.

    Overloads are: 
    (1): standard calls.
    (2): the same as (1) but the degrees are taken from ``sources`` and 
    ``destinations``, respectively.
    (3): the same as (2) but weights are not returned.
    (4): the same as (3) but sources and destinations are returned as a pair.
    */
    std::tuple<int, int, int> dist_graph_neighbors_count() const;
    void dist_graph_neighbors(int maxindegree, int sources[],
	    int sourceweights[], int maxoutdegree, int destinations[], 
        int destweights[]) const;
    void dist_graph_neighbors(ContiguousBuffer<int> sources,
        ContiguousBuffer<int> sourceweights, 
        ContiguousBuffer<int> destinations,
        ContiguousBuffer<int> destweights) const;
    void dist_graph_neighbors(ContiguousBuffer<int> sources,
        ContiguousBuffer<int> destinations) const;
    std::pair<vector<int>, vector<int> > dist_graph_neighbors() const;

    /**
    Low-level topology methods. With communicator manipulation methods they 
    can be used to create any desired topology.

    These two methods reorder the ranks of processes in the current communicator 
    and return the new rank of the caller process, for CART and GRAPH 
    topologies, respectively. The contiguous buffer overloads take ``ndims``
    and ``nnodes`` from ``dims`` and ``index``, respectively.

    The methods cart_create() and graph_create() may be implemented by first 
    calling cart_map() and graph_map(), respectively, and then using split()
    to separate not-in-topology processes from in-topology processes.
    */
    int cart_map(int ndims, const int dims[], const int periods[]) const;
    int cart_map(ContiguousBuffer<int> dims, 
        ContiguousBuffer<int> periods) const;

    int graph_map(int nnodes, const int index[], const int edges[]) const;
    int graph_map(ContiguousBuffer<int> index, 
        ContiguousBuffer<int> edges) const;

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

    The variants are consistent with ``allgather()``, ``allgatherv()``, 
    ``alltoall()``, ``alltoallv()``, ``alltoallw()``, and their nonblocking 
    versions.
    */
    void neighbor_allgather(const void* sendbuf, int sendcount, 
	    const Datatype &sendtype, void* recvbuf, int recvcount, 
	    const Datatype &recvtype) const;
    void neighbor_allgather(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype) const;
    void neighbor_allgather(const ConstDatapacket &send_dpacket,
        void *recvbuf) const;
    void neighbor_allgather(const ConstDatapacket &send_dpacket,
        const Datapacket &recv_dpacket) const;

    void neighbor_allgatherv(
        const void* sendbuf, int sendcount, const Datatype &sendtype, 
        void* recvbuf, const int recvcounts[], const int displs[], 
        const Datatype &recvtype) const;
    void neighbor_allgatherv(
        const ConstDatapacket &send_dpacket, void *recvbuf, 
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs,
        const Datatype &recvtype) const;
    void neighbor_allgatherv(
        const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket,
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs) const;

    void neighbor_alltoall(
        const void* sendbuf, int sendcount, const Datatype &sendtype, 
        void* recvbuf, int recvcount, const Datatype &recvtype) const;
    void neighbor_alltoall(
        const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype) const;

    void neighbor_alltoallv(const void* sendbuf, const int sendcounts[], 
        const int sdispls[], const Datatype &sendtype, void* recvbuf, 
        const int recvcounts[], const int rdispls[], 
        const Datatype &recvtype) const;

    void neighbor_alltoallw(
        const void* sendbuf, const int sendcounts[], const aint_t sdispls[], 
        const Datatype::mpi_t sendtypes[], 
        void* recvbuf, const int recvcounts[], const aint_t rdispls[], 
        const Datatype::mpi_t recvtypes[]) const;

    Requests ineighbor_allgather(const void* sendbuf, int sendcount, 
        const Datatype &sendtype, void* recvbuf, 
        int recvcount, const Datatype &recvtype) const;
    Requests ineighbor_allgather(const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype) const;
    Requests ineighbor_allgather(const ConstDatapacket &send_dpacket, 
        void *recvbuf) const;
    Requests ineighbor_allgather(const ConstDatapacket &send_dpacket, 
        const Datapacket &recv_dpacket) const;

    Requests ineighbor_allgatherv(const void* sendbuf, int sendcount, 
        const Datatype &sendtype, void* recvbuf, const int recvcounts[], 
        const int displs[], const Datatype &recvtype) const;
    Requests ineighbor_allgatherv(
        const ConstDatapacket &send_dpacket,
        void *recvbuf, ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs,
        const Datatype &recvtype ) const;
    Requests ineighbor_allgatherv(
        const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket, 
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs) const;

    Requests ineighbor_alltoall(const void* sendbuf, int sendcount,
        const Datatype &sendtype, void* recvbuf, int recvcount,
        const Datatype &recvtype) const;
    Requests ineighbor_alltoall(const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype) const;

    Requests ineighbor_alltoallv(const void* sendbuf, const int sendcounts[], 
        const int sdispls[], const Datatype &sendtype, void* recvbuf, 
        const int recvcounts[], const int rdispls[], 
        const Datatype &recvtype) const;
    Requests ineighbor_alltoallw(const void* sendbuf, const int sendcounts[],
        const aint_t sdispls[], const Datatype::mpi_t sendtypes[],
        void* recvbuf, const int recvcounts[],
        const aint_t rdispls[], const Datatype::mpi_t recvtypes[]) const;

    /**
     * remote memory access subroutines
     */
    Win win_create(void *base, aint_t size, int disp_unit, 
        const Info &info=Info::nullval()) const;
    Win win_create_dynamic(const Info &info=Info::nullval()) const ;
    
    Win win_allocate(void *&base_ptr, 
        aint_t size, int disp_unit, const Info &info=Info::nullval()) const;
    template<typename T>
    std::pair<Win, T*> win_allocate(size_t n, int disp_unit=sizeof(T), 
        const Info &info=Info::nullval()) const;
    
    Win win_allocate_shared(void *&base_ptr, 
        aint_t size, int disp_unit, const Info &info=Info::nullval()) const;
    template<typename T>
    std::pair<Win, T*> win_allocate_shared(size_t n, int disp_unit=sizeof(T), 
        const Info &info=Info::nullval()) const;

    /**
    Point-to-point communication calls - the core functions of MPI.

    ``send()``, ``bsend()``, ``ssend()``, ``rsend()``: blocking sendings in 
    standard mode, buffered mode, synchronous mode and ready mode. 
    Refer to the MPI standard for their semantics. The standard mode is usually 
    the first choice.
    
    ``recv()``: blocking receiving.
    
    Methods with prefix "i" are the corresponding non-blocking calls. They 
    return a ``Requests`` object for later test, cancellation, or completion.

    ``recv()`` returns a ``Status`` object containing the meta-infomation of 
    the received message. 

    Any data buffer passed to these calls must not be pr-value - its life time 
    must last at least to the return of blocking calls or the finish of 
    non-blocking calls.
    
    Arguments are:
    @dest, src: rank of target process in the group that forming the 
        communicator. Receiving calls may use ``ANY_SOURCE`` for the matching
        of any source rank. All calls may use ``PROC_NULL`` so that the
        method returns immediately and has no effect.
    @tag: matching tag. Receiving calls may use wildcard tag ``ANY_TAG`` for
        the matching of arbitrary tags.
    @args: data to be sent/received. ``args`` are perfect-forwarded to 
        constructing a ``ConstDatapacket`` object (for sending calls) or 
        ``Datapacket`` object (for receiving calls) from which the buffer 
        specification is extracted and passed to the underlying communication 
        calls.
    */
    template<typename ...Args>
    void send( int dest, int tag, Args && ...args ) const;
    template<typename ...Args>
    void bsend( int dest, int tag, Args && ...args ) const;
    template<typename ...Args>
    void ssend( int dest, int tag, Args && ...args ) const;
    template<typename ...Args>
    void rsend( int dest, int tag, Args && ...args ) const;
    template<typename ...Args>
    Status recv( int src, int tag, Args && ...args ) const;

    template<typename ...Args>
    Requests isend( int dest, int tag, Args && ...args ) const;
    template<typename ...Args>
    Requests ibsend( int dest, int tag, Args && ...args ) const;
    template<typename ...Args>
    Requests issend( int dest, int tag, Args && ...args ) const;
    template<typename ...Args>
    Requests irsend( int dest, int tag, Args && ...args ) const;
    template<typename ...Args>
    Requests irecv( int src, int tag, Args && ...args ) const;

    /** 
    Perform one send and one receive in a single call.

    The second overload assumes the receiving buffer size and datatype are the 
    same with those of the sending buffer.

    The third overload uses a single buffer, i.e., received data replace the 
    sending data. Note that the underlying implementation may use additional 
    buffer.
    */
    Status sendrecv(const ConstDatapacket &send_dpacket, int dest, int sendtag, 
        const Datapacket &recv_dpacket, int src, int recvtag);
    Status sendrecv(const ConstDatapacket &send_dpacket, int dest, int sendtag, 
        void *recvbuf, int src, int recvtag);
    Status sendrecv_replace(const Datapacket &dpacket, int dest, int sendtag, 
        int src, int recvtag);

    Status probe(int src, int tag) const;
    Status iprobe(int src, int tag, int &flag) const;
    std::pair<Status, Message> mprobe(int src, int tag) const;
    std::pair<Status, Message> improbe(int src, int tag, int &flag) const;

    /**
    Collective communication - barrier of all processes.
    */
    void barrier() const;
    
    /**
    Broadcast calls.
    MPI standard-compliant.
    Use Datapacket as buffer specification.
    */
    void bcast(void *buf, int count, const Datatype &dtype, int root) const;
    void bcast(const Datapacket &dpacket, int root) const;

    /** 
    Gather calls.
    (1): MPI standard-compliant.
    (2)：Send and recv share the same datatype and count.
    (3,4): Same as (2) but the datatype and count are taken from 
    ``send_dpacket``.
    
    recvbuf, recvcount, recvtype - only significant at root. The send buffer 
    signature must match the recv buffer signature at root.

    For intra-communicator, ``sendbuf = IN_PLACE`` at root means in-place 
    sending. Then the sendcount and sendtype are ignored.

    For inter-communicator, in group A, root process passes ``root=ROOT``, other
    processes pass ``root=PROC_NULL``. In group B, all pass ``root`` eq to the 
    rank of root in A.
    */
    void gather(const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    void gather(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, int root) const;
    void gather(const ConstDatapacket &send_dpacket, 
        void *recvbuf, int root) const;
    void gather(const ConstDatapacket &send_dpacket, 
        const Datapacket &recv_dpacket, 
        int root) const;
    
    /**
    Variant of gather, allowing processes sending different number of items.
    (1): MPI standard-compliant.
    (2): Use abstract concept arguments - Datapacket and ContiguousBuffer.
    (3): Same as (2) but recv datatype is inferred from the datapacket.

    @recvbuf, recvcounts, displs, recvtype:  place to put the received data - 
    significant only at root.
    ``sendbuf = IN_PLACE`` is still available.
    */
    void gatherv(const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype, int root) const;
    void gatherv(const ConstDatapacket &send_dpacket, void *recvbuf, 
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs,
        const Datatype &recvtype, int root) const;
    void gatherv(const ConstDatapacket &send_dpacket, 
        const Datapacket &recv_dpacket, 
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs, int root) const;

    /**
    Scatter calls
    (1): MPI standard-compliant.
    (2): Send and recv share the same datatype and count.
    (3,4): Same as (2) but datatype and count are taken from ``recv_dpacket``.
    
    sendbuf, sendcount, sendtype - only significant at root. The send buffer
    signature at root must match each recv buffer signature. Every 
    location in the send buffer cannot be read more than once.

    For intra-communicator, ``recvbuf = IN_PLACE`` at root mean in-place 
    sending. Then recvbuf and recvcount are ignored.

    For inter-communicator, in group A, root process passes ``root=ROOT``, other
    processes pass ``root=PROC_NULL``. In group B, all pass ``root`` eq to the 
    rank of root in A.
    */
    void scatter(
        const void *sendbuf, int sendcount, const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root )const;
    void scatter(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, int root) const; 
    void scatter(const void *sendbuf, 
        const Datapacket &recv_dpacket, int root) const;
    void scatter(const ConstDatapacket &send_dpacket,
        const Datapacket &recv_dpacket, int root) const;

    /**
    Variants of scatter, allowing sending to processes different number of 
    items.
    (1): MPI standard-compliant.
    (2): Use abstract concept arguments - Datapacket and ContiguousBuffer.
    (3): Same as (2) but send datatype is inferred from the datapacket.

    The send buffer arguments are ignored at all processes except the root.
    ``recvbuf = IN_PLACE`` is still available.
    */
    void scatterv(const void *sendbuf, const int sendcounts[], 
        const int displs[], const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    void scatterv(const void *sendbuf, ContiguousBuffer<const int> sendcounts, 
        ContiguousBuffer<const int> displs, const Datatype &sendtype,
        const Datapacket &recv_dpacket, int root) const;
    void scatterv(const ConstDatapacket &send_dpacket, 
        ContiguousBuffer<const int> sendcounts, 
        ContiguousBuffer<const int> displs,
        const Datapacket &recv_dpacket, int root) const;

    /**
    All-variant to the gather/gatherv call - all processes received the data.
    */
    void allgather(
        const void *sendbuf, int sendcount, const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype) const;
    void allgather(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype) const;
    void allgather(const ConstDatapacket &send_dpacket,
        void *recvbuf) const;
    void allgather(const ConstDatapacket &send_dpacket,
        const Datapacket &recv_dpacket) const;
    
    void allgatherv(
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype ) const;
    void allgatherv(
        const ConstDatapacket &send_dpacket, void *recvbuf, 
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs,
        const Datatype &recvtype) const;
    void allgatherv(
        const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket,
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs) const;

    /**
    All-to-all calls.
    (1): MPI standard-compliant.
    (2): send buffer and recv buffer share the same count and datatype.

    If send buffer is ``IN_PLACE``, the data are taken from and replace the recv
    buffer.
    */
    void alltoall(const void *sendbuf, int sendcount, const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype ) const;
    void alltoall(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype) const;

    void alltoallv( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], const Datatype &sendtype,
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        const Datatype &recvtype ) const;
    void alltoallw( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], const Datatype::mpi_t sendtypes[],
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        const Datatype::mpi_t recvtypes[] ) const;
    /**
    Reduce calls.
    (1): MPI standard-compliant.
    (2,4): count and datatype are taken from ``send_dpacket``.
    (3): count and datatype are taken from ``recv_dpacket``.

    The same applies to ``allreduce()``.

    Set ``sendbuf=IN_PLACE`` at root implies in-place reduction, i.e., at root,
    data are taken from recv buffer and the results overwrite it.
    */
    void reduce(const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op, int root) const;
    void reduce(const ConstDatapacket &send_dpacket, void *recvbuf,
        const Oppacket &op, int root) const;
    void reduce(const void *sendbuf, const Datapacket &recv_dpacket,
        const Oppacket &op, int root) const;
    void reduce(const ConstDatapacket &send_dpacket, 
        const Datapacket &recv_dpacket,
        const Oppacket &op, int root) const;

    void allreduce(const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op ) const;
    void allreduce(const ConstDatapacket &send_dpacket, void *recvbuf, 
        const Oppacket &op ) const;
    void allreduce(const void *sendbuf, const Datapacket &recv_dpacket,
        const Oppacket &op ) const;
    void allreduce(const ConstDatapacket &send_dpacket, 
        const Datapacket &recv_dpacket, const Oppacket &op) const;
    
    /**
    Reduce_local calls.
    (1): MPI standard-compliant.
    (2,3): count and datatype are taken from ``in_dpacket``.
    */
    static void reduce_local(const void *inbuf, void *inoutbuf, int count, 
        const Datatype &dtype, const Oppacket &op);
    static void reduce_local(const ConstDatapacket &in_dpacket, void *inoutbuf, 
        const Oppacket &op);
    static void reduce_local(const ConstDatapacket &in_dpacket, 
        const Datapacket &inout_dpacket, const Oppacket &op);
    
    void reduce_scatter_block(const void *sendbuf, void *recvbuf, 
        int recvcount, const Datatype &dtype, const Oppacket &op ) const;
    
    void reduce_scatter( const void *sendbuf, void *recvbuf, 
        const int recvcounts[], const Datatype &dtype, 
        const Oppacket &op )const;
    
    void scan( const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, const Oppacket &op ) const;
    void exscan( const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, const Oppacket &op ) const;

    Requests ibarrier() const;
    Requests ibcast( 
        void *buf, int count, const Datatype &dtype, int root) const;
    Requests ibcast(const Datapacket &dpacket, int root) const;

    Requests igather( 
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    Requests igather(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, int root) const;
    Requests igather(const ConstDatapacket &send_dpacket, 
        void *recvbuf, int root) const;
    Requests igather(const ConstDatapacket &send_dpacket, 
        const Datapacket &recv_dpacket, int root) const;

    Requests igatherv(
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype, int root ) const;
    Requests igatherv(
        const ConstDatapacket &send_dpacket, void *recvbuf, 
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs, 
        const Datatype &recvtype, int root ) const;
    Requests igatherv(
        const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket,
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs, int root) const;
    
    Requests iscatter(
        const void *sendbuf, int sendcount, const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root )const;
    Requests iscatter(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, int root) const; 
    Requests iscatter(const void *sendbuf, 
        const Datapacket &recv_dpacket, int root) const;
    Requests iscatter(const ConstDatapacket &send_dpacket,
        const Datapacket &recv_dpacket, int root) const;

    Requests iscatterv(
        const void *sendbuf, const int sendcounts[], const int displs[], 
        const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    Requests iscatterv(
        const void *sendbuf, ContiguousBuffer<const int> sendcounts, 
        ContiguousBuffer<const int> displs, const Datatype &sendtype,
        const Datapacket &recv_dpacket, int root) const;
    Requests iscatterv(
        const ConstDatapacket send_dpacket, 
        ContiguousBuffer<const int> sendcounts, 
        ContiguousBuffer<const int> displs,
        const Datapacket &recv_dpacket, int root) const;

    Requests iallgather(const void *sendbuf, int sendcount, 
        const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype ) const;
    Requests iallgather(const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype) const;
    Requests iallgather(const ConstDatapacket &send_dpacket, 
        void *recvbuf) const;
    Requests iallgather(const ConstDatapacket &send_dpacket, 
        const Datapacket &recv_dpacket) const;

    Requests iallgatherv(
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype ) const;
    Requests iallgatherv(
        const ConstDatapacket &send_dpacket,
        void *recvbuf, ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs,
        const Datatype &recvtype ) const;
    Requests iallgatherv(
        const ConstDatapacket &send_dpacket, const Datapacket &recv_dpacket, 
        ContiguousBuffer<const int> recvcounts, 
        ContiguousBuffer<const int> displs) const;
    
    Requests ialltoall(const void *sendbuf, int sendcount, 
        const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype ) const;
    Requests ialltoall(const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype) const;

    Requests ialltoallv( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], const Datatype &sendtype,
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        const Datatype &recvtype ) const;
    Requests ialltoallw( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], const Datatype::mpi_t sendtypes[],
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        const Datatype::mpi_t recvtypes[] ) const;

    Requests ireduce( const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op, int root ) const;
    Requests ireduce( const ConstDatapacket &send_dpacket, void *recvbuf,
        const Oppacket &op, int root ) const;
    Requests ireduce(const void *sendbuf, const Datapacket &recv_dpacket,
        const Oppacket &op, int root ) const;
    Requests ireduce(const ConstDatapacket &send_dpacket, 
        const Datapacket &recv_dpacket, const Oppacket &op, int root ) const;
    
    Requests iallreduce( const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op ) const;
    Requests iallreduce( const ConstDatapacket &send_dpacket, void *recvbuf, 
        const Oppacket &op ) const;
    Requests iallreduce(const void *sendbuf, const Datapacket &recv_dpacket,
        const Oppacket &op ) const;
    Requests iallreduce(const ConstDatapacket &send_dpacket, 
        const Datapacket &recv_dpacket,
        const Oppacket &op ) const;

    Requests ireduce_scatter_block( const void *sendbuf, void *recvbuf, 
        int recvcount, const Datatype &dtype, const Oppacket &op ) const;
    Requests ireduce_scatter( const void *sendbuf, void *recvbuf, 
        const int recvcounts[], const Datatype &dtype, 
        const Oppacket &op )const;
    Requests iscan( const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, const Oppacket &op ) const;
    Requests iexscan( const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, const Oppacket &op ) const;
protected:
    static Comm _from_raw(mpi_t obj, int state) noexcept;
    static string _topostr( int topo );

    static bool _null_copy_fn( Comm &oldcomm, int keyval, 
        void *extra_state, void *attr_val, void *&attr_val_out) noexcept;
    static bool _dup_fn( Comm &oldcomm, int keyval, 
        void *extra_state, void *attr_val, void *&attr_val_out) noexcept;
    static void _null_del_fn( Comm &comm, int keyval, 
        void *attr_val, void *extra_state ) noexcept;

    struct _attr_extra_state_t{
        copy_attr_fn_t copy_attr_fn;
        del_attr_fn_t del_attr_fn;
        void *extra_state;
        _attr_extra_state_t( 
            copy_attr_fn_t _copy_attr_fn = _null_copy_fn, 
            del_attr_fn_t _del_attr_fn = _null_del_fn, 
            void *_extra_state = nullptr):
            copy_attr_fn( _copy_attr_fn ), del_attr_fn( _del_attr_fn ),
            extra_state(_extra_state) { }
    };
    static std::unordered_map<int, _attr_extra_state_t *> _attr_extra_state;
    static int _copy_attr_fn( mpi_t oldcomm, int keyval, void *extra_state, 
        void *attr_val, void *attr_val_out, int *flag );
    static int _del_attr_fn( mpi_t comm, int keyval,
        void *attr_val, void *extra_state );
};

inline ostream & operator<< (ostream &os, const Comm &comm) { 
    return comm.info(os, 0); 
}

inline Comm Comm::_from_raw(mpi_t obj, int state) noexcept {
    return Comm( std::make_shared<_obj_raw_t>(obj, state) );
}

template<typename AttrT>
int Comm::create_keyval(){
    auto copy_attr_fn = []( Comm &oldcomm, int keyval, 
        void *extra_state, void *attr_val, void *&attr_val_out )->bool{
            try{
                attr_val_out = attr_val ? 
                    new AttrT( *(AttrT *)attr_val ) : NULL;
            }catch( ... ){
                ErrMPI::throw_(1, emFLPFB);
            }
            return true;
        };
    auto del_attr_fn = []( Comm &comm, int keyval, 
        void *attr_val, void *extra_state ){
            try{
                if( attr_val ) delete (AttrT *)attr_val;
            }catch( ... ){
                ErrMPI::throw_(1, emFLPFB);
            } 
        };
    return create_keyval( copy_attr_fn, del_attr_fn );
}
template<typename AttrT>
bool Comm::get_attr( int keyval, AttrT * &attr_val ) const{
    void *ptr;
    auto flag = get_attr(keyval, ptr);
    attr_val = (AttrT *)ptr;
    return flag;
}

template<typename T>
std::pair<Win, T*> Comm::win_allocate(size_t n, int disp_unit, 
    const Info &info) const 
{
    void *base_ptr=nullptr;
    auto win = win_allocate(base_ptr, n*sizeof(T), disp_unit, info);
    return {std::move(win), static_cast<T*>(base_ptr)};
}

template<typename T>
std::pair<Win, T*> Comm::win_allocate_shared(size_t n, int disp_unit, 
    const Info &info) const
{
    void *base_ptr=nullptr;
    auto win = win_allocate_shared(base_ptr, n*sizeof(T), disp_unit, info);
    return {std::move(win), static_cast<T*>(base_ptr)};
}

template<typename ...Args>
void Comm::send( int dest, int tag, Args && ...args ) const{
    auto [p, n, dt] = ConstDatapacket{ std::forward<Args>(args)... };
    _obj_ptr->send(p, n, dt.raw(), dest, tag );
}

template<typename ...Args>
void Comm::bsend( int dest, int tag, Args && ...args ) const{
    auto [p, n, dt] = ConstDatapacket{ std::forward<Args>(args)... };
    _obj_ptr->bsend( p, n, dt.raw(), dest, tag );
}

template<typename ...Args>
void Comm::ssend( int dest, int tag, Args && ...args ) const{
    auto [p, n, dt] = ConstDatapacket{ std::forward<Args>(args)... };
    _obj_ptr->ssend( p, n, dt.raw(), dest, tag );
}

template<typename ...Args>
void Comm::rsend( int dest, int tag, Args && ...args ) const{
    auto [p, n, dt] = ConstDatapacket{ std::forward<Args>(args)... };
    _obj_ptr->rsend( p, n, dt.raw(), dest, tag );
}

template<typename ...Args>
Status Comm::recv( int src, int tag, Args && ...args ) const{
    auto [p, n, dt] = Datapacket{ std::forward<Args>(args)... };
    return _obj_ptr->recv( p, n, dt.raw(), src, tag );
}

template<typename ...Args>
Requests Comm::isend( int dest, int tag, Args && ...args ) const{
    auto [p, n, dt] = ConstDatapacket{ std::forward<Args>(args)... };
    auto rq = _obj_ptr->isend( p, n, dt.raw(), dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::ibsend( int dest, int tag, Args && ...args ) const{
    auto [p, n, dt] = ConstDatapacket{ std::forward<Args>(args)... };
    auto rq = _obj_ptr->ibsend( p, n, dt.raw(), dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::issend( int dest, int tag, Args && ...args ) const{
    auto [p, n, dt] = ConstDatapacket{ std::forward<Args>(args)... };
    auto rq = _obj_ptr->issend( p, n, dt.raw(), dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::irsend( int dest, int tag, Args && ...args ) const{
    auto [p, n, dt] = ConstDatapacket{ std::forward<Args>(args)... };
    auto rq = _obj_ptr->irsend( p, n, dt.raw(), dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::irecv( int src, int tag, Args && ...args ) const{
    auto [p, n, dt] = Datapacket{ std::forward<Args>(args)... };
    auto rq = _obj_ptr->irecv( p, n, dt.raw(), src, tag );
    return Requests::_from_raw( rq, 0 );
}

} // namespace HIPP::MPI

#endif	//_HIPPMPI_MPI_COMM_H_