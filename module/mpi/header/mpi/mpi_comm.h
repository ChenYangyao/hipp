/**
 * creat: Yangyao CHEN, 2020/01/18
 *      [write   ] Comm - the high-level communicator interface for MPI system.
 */ 

#ifndef _HIPPMPI_MPI_COMM_H_
#define _HIPPMPI_MPI_COMM_H_
#include "mpi_obj_base.h"
#include "mpi_raw_comm.h"
#include "mpi_group.h"
#include "mpi_datapacket.h"
#include "mpi_op.h"
#include "mpi_status.h"
#include "mpi_requests.h"
namespace HIPP{
namespace MPI{

/**
 * the high-level communicator interface for MPI system
 * 
 * The communicator instance should not be directly constructed, but should be
 * returned by other function calls like the communicator management functions
 * Comm class, or by the world() function of Env class. After you have a initial
 * communicator like the 'world', it is convinient to operate on its process 
 * group and use Comm::create() to create a new communicator.
 * 
 * After the communicator object is obtained, the communication in this 
 * communicator is invoked by its member functions.
 */
class Comm: public MPIObj<_Comm> {
public:
    typedef MPIObj<_Comm> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    /**
     * display some basic information of the communicator to `os`
     * @fmt_cntl:   control the display format. 0 for inline information and 1
     *              for a verbose, multiple-line information. 2 for a exhausted
     *              priting, with lots of information to be printed.
     * The `os` object is returned.
     * 
     * The overloaded `<<` operator is equivalent to info() with the default 
     * `fmt_cntl`.
     */
    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Comm &comm );
    
    /**
     * free the current communicator, and set it to a null communicator as 
     * returned by nullval().
     * free() can be called at any time, even for the pre-defined communicators.
     */
    void free() noexcept;

    /**
     * inquiry the communicator infomation - size, rank and test if it is null.
     */
    int size() const;
    int rank() const;
    bool is_null() const;

    bool is_inter() const;
    int remote_size() const;

    /**
     * communicator management
     * 
     * split() - split the communicator into several ones.
     * dup()   - copy the current communicator
     * create()- create new communicators
     * These operations are all collective, and must be called by all the 
     * processes in the group of current communicator.
     * 
     * world(), selfval() and nullval() return the predefined communicators. 
     * These calls are local.
     * 
     * create_inter() - create a new inter-communicater.
     * This must be called by two groups of processes, both provide the local
     * leader. The local leader must specify the peer_comm that contains self
     * and the remote leader.
     * 
     * merge_inter()  - merge the inter-communicator into a intra one.
     */
    Comm split( int color, int key = 0 )const;
    Comm dup();
    Comm create( const Group &group);
    static Comm world() noexcept;
    static Comm selfval() noexcept;
    static Comm nullval() noexcept;
    Comm create_inter( int local_leader, const Comm &peer_comm, 
        int remote_leader, int tag );
    Comm merge_inter( int high );

    /**
     * access the group of processes in the communicator.
     */
    const Group group() const;
    Group group();
    const Group remote_group() const;
    Group remote_group();

    /**
     * virtual topology functions
     */
    Comm cart_create( const vector<int> &dims, 
        const vector<int> &periods, int reorder = 1 )const;
    static void dims_create( int nnodes, int ndims, vector<int> &dims );
    int topo_test()const;
    int cartdim_get()const;
    void cart_get( vector<int> &dims, vector<int> &periods, 
        vector<int> &coords )const;
    int cart_rank( const vector<int> &coords )const;
    vector<int> cart_coords( int rank )const;
    void cart_shift( int direction, int disp, 
        int &rank_src, int &rank_dest )const;
    Comm cart_sub( const vector<int> &remain_dims );


    /**
     * point-to-point communication
     * 
     * In the following send/recv operations, `args` are data buffer to be 
     * sent from/received into. Any arguments that can be used to construct
     * a `Datapacket` type is a valid way of specifying the data buffer.
     * 
     * According to MPI standard, the non-blocking version is started with an 
     * `i`. Both blocking and non-blocking versions provide four send modes: 
     * standard, synchronous, bufferd and ready modes. The blocking recv returns 
     * a Status object containing the information that has been received. The 
     * non-blocking send/recv returns a Requests object for handling the 
     * completion/testing of the communication.
     */
    template<typename ...Args>
    void send( int dest, int tag, Args && ...args );
    template<typename ...Args>
    void bsend( int dest, int tag, Args && ...args );
    template<typename ...Args>
    void ssend( int dest, int tag, Args && ...args );
    template<typename ...Args>
    void rsend( int dest, int tag, Args && ...args );
    template<typename ...Args>
    Status recv( int src, int tag, Args && ...args );

    template<typename ...Args>
    Requests isend( int dest, int tag, Args && ...args );
    template<typename ...Args>
    Requests ibsend( int dest, int tag, Args && ...args );
    template<typename ...Args>
    Requests issend( int dest, int tag, Args && ...args );
    template<typename ...Args>
    Requests irsend( int dest, int tag, Args && ...args );
    template<typename ...Args>
    Requests irecv( int src, int tag, Args && ...args );

    /**
     * collective communication
     * 
     * According to MPI standard, some collective communication functions can
     * apply to both inter and intra communicators. If intra communicators
     * are used, then only the 'all to all' functions are bi-directional, others
     * are uni-directional.
     * Some recv/send buffer can be specified with a HIPP::MPI::IN_PLACE, this
     * is exactly same as MPI_IN_PLACE.
     * The non-blocking version here returns a Requests object for later testing
     * and completion. The requests object should not be freed manually before
     * completion.
     * In all cases, the datatype argument mush be exactly a Datatype object or
     * array of Datatype objects. This is different from the point-to-point
     * communication, where you can pass a string to indicate a basic type. One
     * exception is alltoallw() and ialltoallw(), in which the datatype 
     * arguments is an array of original MPI datatype (This design avoid the 
     * problem when using non-blocking collective operation, and also avoid 
     * overhead in converting the datatype from high-level instance to MPI 
     * original one).
     */
    void barrier() const;
    void bcast( void *buf, int count, const Datatype &dtype, int root) const;
    void gather( const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    void gatherv(
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype, int root ) const;
    void scatter(
        const void *sendbuf, int sendcount, const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root )const;
    void scatterv(
        const void *sendbuf, const int sendcounts[], const int displs[], 
        const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    void allgather( const void *sendbuf, int sendcount, 
        const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype ) const;
    void allgatherv(
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype ) const;
    void alltoall( const void *sendbuf, int sendcount, const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype ) const;
    void alltoallv( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], const Datatype &sendtype,
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        const Datatype &recvtype ) const;
    void alltoallw( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], const Datatype::mpi_t sendtypes[],
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        const Datatype::mpi_t recvtypes[] ) const;
    void reduce( const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op, int root ) const;
    void allreduce( const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op ) const;
    static void reduce_local( const void *inbuf, void *inoutbuf, int count, 
        const Datatype &dtype, const Oppacket &op );
    void reduce_scatter_block( const void *sendbuf, void *recvbuf, 
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
    Requests igather( 
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    Requests igatherv(
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype, int root ) const;
    Requests iscatter(
        const void *sendbuf, int sendcount, const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root )const;
    Requests iscatterv(
        const void *sendbuf, const int sendcounts[], const int displs[], 
        const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    Requests iallgather( const void *sendbuf, int sendcount, 
        const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype ) const;
    Requests iallgatherv(
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype ) const;
    Requests ialltoall( const void *sendbuf, int sendcount, 
        const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype ) const;
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
    Requests iallreduce( const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op ) const;
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
};

inline ostream & operator<<( ostream &os, const Comm &comm )
    { return comm.info(os); }

inline Comm Comm::_from_raw(mpi_t obj, int state) noexcept{
    return Comm( std::make_shared<_obj_raw_t>(obj, state) );
}

template<typename ...Args>
void Comm::send( int dest, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    _obj_ptr->send( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
}

template<typename ...Args>
void Comm::bsend( int dest, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    _obj_ptr->bsend( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
}

template<typename ...Args>
void Comm::ssend( int dest, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    _obj_ptr->ssend( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
}

template<typename ...Args>
void Comm::rsend( int dest, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    _obj_ptr->rsend( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
}

template<typename ...Args>
Status Comm::recv( int src, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    return _obj_ptr->recv( dp._buff, dp._size, dp._dtype.raw(), src, tag );
}

template<typename ...Args>
Requests Comm::isend( int dest, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->isend( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::ibsend( int dest, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->ibsend( dp._buff, dp._size, dp._dtype.raw(), 
        dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::issend( int dest, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->issend( dp._buff, dp._size, dp._dtype.raw(), 
        dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::irsend( int dest, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->irsend( dp._buff, dp._size, dp._dtype.raw(), 
        dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::irecv( int src, int tag, Args && ...args ){
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->irecv( dp._buff, dp._size, dp._dtype.raw(), src, tag );
    return Requests::_from_raw( rq, 0 );
}

} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_COMM_H_