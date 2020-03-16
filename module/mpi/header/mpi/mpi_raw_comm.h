/**
 * creat: Yangyao CHEN, 2020/01/21
 *      [write   ] _Comm - the intermediate-level MPI communicator interface.
 */

#ifndef _HIPPMPI_MPI_RAW_COMM_H_
#define _HIPPMPI_MPI_RAW_COMM_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP{
namespace MPI{

/**
 * the intermediate-level MPI communicator interface.
 * 
 * _Comm should not be constructed by users directly. The life time of it is 
 * handled by the high-level interfaces.
 */
class _Comm {
public:
    typedef MPI_Comm mpi_t;
    typedef MPI_Comm_copy_attr_function copy_attr_fn_t;
    typedef MPI_Comm_delete_attr_function del_attr_fn_t;
    static const copy_attr_fn_t *const NULL_COPY_FN, * const DUP_FN;
    static const del_attr_fn_t *const NULL_DEL_FN;

    /**
     * intermediate-level communicator constructors.
     * @val:    the actual original MPI_Comm variable.
     * @state:  indicates how the life time of communicator is controlled.
     *          If the life time is system-managed, that is, not handled by
     *          this instance, set state to 0.
     *          Otherwise set state to 1, and the communicator is freed when 
     *          deconstructing.
     * For the null-communicator, it is safe to set state 0 or 1.
     */
    _Comm( mpi_t val, int state ) noexcept: _val(val), _state( state ){ }
    ~_Comm() noexcept{
        if( _state != 0 && ! is_null() ){
            if( MPI_Comm_free( &_val ) != MPI_SUCCESS )
                ErrMPI::abort(1, emFLPFB);
        }
    }
    static void free( mpi_t &comm ){
        ErrMPI::check(  MPI_Comm_free(&comm), emFLPFB );
    }

    int size() const{
        int _size;
        ErrMPI::check( MPI_Comm_size(_val, &_size), emFLPFB);
        return _size;
    }
    int rank() const{
        int _rank;
        ErrMPI::check( MPI_Comm_rank(_val, &_rank), emFLPFB );
        return _rank;
    }
    mpi_t raw() const noexcept { return _val; }
    bool is_null() const noexcept { return _val == nullval(); }
    bool is_inter() const {
        int _flag;
        ErrMPI::check( MPI_Comm_test_inter(_val, &_flag), emFLPFB );
        return bool(_flag);
    }
    int remote_size() const {
        int _size;
        ErrMPI::check( MPI_Comm_remote_size(_val, &_size), emFLPFB );
        return _size;
    }

    /**
     * attribute caching
     */
    static int create_keyval( copy_attr_fn_t *copy_attr_fn, 
        del_attr_fn_t *delete_attr_fn, void *extra_state ){
        int keyval;
        ErrMPI::check( 
            MPI_Comm_create_keyval( 
                copy_attr_fn, delete_attr_fn, &keyval, extra_state ), emFLPFB );
        return keyval;
    }
    static void free_keyval( int *comm_keyval ) {
        ErrMPI::check( MPI_Comm_free_keyval( comm_keyval ), emFLPFB );
    }
    void set_attr( int keyval, void *attr_val ) {
        ErrMPI::check( MPI_Comm_set_attr(_val, keyval, attr_val), emFLPFB );
    }
    bool get_attr( int keyval, void *attr_val ) const {
        int flag;
        ErrMPI::check( 
            MPI_Comm_get_attr(_val, keyval, attr_val, &flag), emFLPFB );
        return bool(flag);
    }
    void del_attr( int keyval ) {
        ErrMPI::check( MPI_Comm_delete_attr(_val, keyval), emFLPFB, 
            "  ... keyval=", keyval, '\n' );
    }
    
    /**
     * new communicator constructors
     */
    mpi_t split( int color, int key = 0 )const{
        mpi_t newcomm;
        ErrMPI::check( 
            MPI_Comm_split( _val, color, key, &newcomm ), emFLPFB );
        return newcomm;
    }
    mpi_t dup( )const{
        mpi_t newcomm;
        ErrMPI::check( MPI_Comm_dup( _val, &newcomm ), emFLPFB );
        return newcomm;
    }
    static mpi_t create( mpi_t comm, MPI_Group group ){
        mpi_t newcomm;
        ErrMPI::check( MPI_Comm_create(comm, group, &newcomm), emFLPFB );
        return newcomm;
    }
    static mpi_t nullval() noexcept { return MPI_COMM_NULL; }
    static mpi_t selfval() noexcept { return MPI_COMM_SELF; }
    static mpi_t world() noexcept { return MPI_COMM_WORLD; }

    static mpi_t create_inter( mpi_t local_comm, int local_leader, 
        mpi_t peer_comm, int remote_leader, int tag ){
        mpi_t newcomm;
        ErrMPI::check( MPI_Intercomm_create( local_comm, local_leader, 
            peer_comm, remote_leader, tag, &newcomm), emFLPFB );
        return newcomm;
    }
    static mpi_t merge_inter( mpi_t intercomm, int high ){
        mpi_t newcomm;
        ErrMPI::check( MPI_Intercomm_merge( intercomm, high, &newcomm ),
            emFLPFB );
        return newcomm;
    }
    /**
     * access the group content
     */
    MPI_Group group() const{
        MPI_Group obj;
        ErrMPI::check( MPI_Comm_group(_val, &obj), emFLPFB );
        return obj;
    }
    MPI_Group remote_group() const{
        MPI_Group obj;
        ErrMPI::check( MPI_Comm_remote_group(_val, &obj), emFLPFB );
        return obj;
    }

    /**
     * virtual topology functions
     */
    mpi_t cart_create( int ndims, const int dims[], const int periods[], 
        int reorder )const{
        mpi_t comm_cart;
        ErrMPI::check( MPI_Cart_create( _val, ndims, dims, periods, reorder, 
            &comm_cart ), emFLPFB, "  ... ndims=", ndims, '\n' );
        return comm_cart;
    }
    static void dims_create( int nnodes, int ndims, int dims[] ){
        ErrMPI::check(
            MPI_Dims_create(nnodes, ndims, dims), emFLPFB, 
                "  ... nnodes=", nnodes, ", ndims=", ndims, '\n');
    }

    int topo_test()const{
        int status;
        ErrMPI::check( MPI_Topo_test(_val, &status), emFLPFB );
        return status;
    }
    int cartdim_get()const{
        int ndims;
        ErrMPI::check( MPI_Cartdim_get(_val,&ndims), emFLPFB );
        return ndims;
    }
    void cart_get( int maxdims, int dims[], int periods[], int coords[] )const{
        ErrMPI::check(
            MPI_Cart_get(_val, maxdims, dims, periods, coords), emFLPFB, 
            "  ... maxdims=", maxdims, '\n');
    }
    int cart_rank( const int coords[] )const{
        int rank;
        ErrMPI::check(
            MPI_Cart_rank(_val, coords, &rank), emFLPFB);
        return rank;
    }
    void cart_coords( int rank, int maxdims, int coords[] )const{
        ErrMPI::check( MPI_Cart_coords(_val, rank, maxdims, coords), 
            emFLPFB, "  ... rank=", rank, ", maxdims=", maxdims );
    }
    void cart_shift( int direction, int disp, int *rank_src, 
        int *rank_dest )const{
        ErrMPI::check( MPI_Cart_shift(_val, direction, disp, 
            rank_src, rank_dest), emFLPFB, "  ... direction=", direction,
            ", disp=", disp, '\n' );
    }
    mpi_t cart_sub( const int remain_dims[] )const{
        mpi_t newcomm;
        ErrMPI::check(
            MPI_Cart_sub( _val, remain_dims, &newcomm ), emFLPFB);
        return newcomm;
    }
    /**
     * wrappers of MPI point-to-point communication.
     */
    void send( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const{
        ErrMPI::check( MPI_Send(buff, count, dtype, dest, tag, _val), 
            emFLPFB );
    }
    void bsend(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const {
        ErrMPI::check( MPI_Bsend(buff, count, dtype, dest, tag, _val),
            emFLPFB );
    }
    void ssend(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const {
        ErrMPI::check( MPI_Ssend(buff, count, dtype, dest, tag, _val),
            emFLPFB );
    }
    void rsend(const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const {
        ErrMPI::check( MPI_Rsend(buff, count, dtype, dest, tag, _val),
            emFLPFB );
    }
    MPI_Status recv( void *buff, int count, MPI_Datatype dtype, 
        int src, int tag ) const{
        MPI_Status st;
        ErrMPI::check( MPI_Recv(buff, count, dtype, src, tag, _val, &st), 
            emFLPFB );
        return st;
    }
    MPI_Request isend( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const{
        MPI_Request rq;
        ErrMPI::check( MPI_Isend(buff, count, dtype, dest, tag, _val, &rq), 
            emFLPFB );
        return rq;
    }
    MPI_Request ibsend( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const{
        MPI_Request rq;
        ErrMPI::check( MPI_Ibsend(buff, count, dtype, dest, tag, _val, &rq), 
            emFLPFB );
        return rq;
    }
    MPI_Request issend( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const{
        MPI_Request rq;
        ErrMPI::check( MPI_Issend(buff, count, dtype, dest, tag, _val, &rq), 
            emFLPFB );
        return rq;
    }
    MPI_Request irsend( const void *buff, int count, MPI_Datatype dtype, 
        int dest, int tag ) const{
        MPI_Request rq;
        ErrMPI::check( MPI_Irsend(buff, count, dtype, dest, tag, _val, &rq), 
            emFLPFB );
        return rq;
    }
    MPI_Request irecv( void *buff, int count, MPI_Datatype dtype, 
        int src, int tag ) const{
        MPI_Request rq;
        ErrMPI::check( MPI_Irecv(buff, count, dtype, src, tag, _val, &rq) );
        return rq;
    }

    /**
     * wrappers of MPI original blocking/non-blocking collective communication
     * (data transfer and computation).
     */
    void barrier() const{
        ErrMPI::check( 
            MPI_Barrier(_val), emFLPFB );
    }
    void bcast( void *buf, int count, MPI_Datatype dtype, int root ) const{
        ErrMPI::check( 
            MPI_Bcast(buf, count, dtype, root, _val), emFLPFB );
    }
    void gather( const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const{
        ErrMPI::check( 
            MPI_Gather(sendbuf, sendcount, sendtype, 
                recvbuf, recvcount, recvtype, root, _val), emFLPFB );
    }
    void gatherv(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        MPI_Datatype recvtype, int root ) const{
        ErrMPI::check( 
            MPI_Gatherv(sendbuf, sendcount, sendtype, 
                recvbuf, recvcounts, displs, recvtype, root, _val), emFLPFB );
    }
    void scatter(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const{
        ErrMPI::check( 
            MPI_Scatter(sendbuf, sendcount, sendtype, 
                recvbuf, recvcount, recvtype, root, _val), emFLPFB );
    }
    void scatterv(
        const void *sendbuf, const int sendcounts[], const int displs[], 
        MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root) const{
        ErrMPI::check( 
            MPI_Scatterv(sendbuf, sendcounts, displs, sendtype, 
                recvbuf, recvcount, recvtype, root, _val), emFLPFB );
    }
    void allgather( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype ) const{
    ErrMPI::check( 
        MPI_Allgather(sendbuf, sendcount, sendtype, 
            recvbuf, recvcount, recvtype, _val), emFLPFB );
    }
    void allgatherv(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        MPI_Datatype recvtype ) const{
        ErrMPI::check( 
            MPI_Allgatherv(sendbuf, sendcount, sendtype, 
                recvbuf, recvcounts, displs, recvtype, _val), emFLPFB );
    }
    void alltoall( const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype ) const{
        ErrMPI::check( 
            MPI_Alltoall(sendbuf, sendcount, sendtype, 
                recvbuf, recvcount, recvtype,_val), emFLPFB );
    }
    void alltoallv( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], MPI_Datatype sendtype,
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        MPI_Datatype recvtype ) const{
        ErrMPI::check( 
            MPI_Alltoallv(sendbuf, sendcounts, senddispls, sendtype, 
                recvbuf, recvcounts, recvdispls, recvtype, _val), emFLPFB );
    }
    void alltoallw( const void *sendbuf, const int sendcounts[], 
        const int senddispls[], const MPI_Datatype sendtypes[],
        void *recvbuf, const int recvcounts[], const int recvdispls[], 
        const MPI_Datatype recvtypes[] ) const{
        ErrMPI::check( 
            MPI_Alltoallw(sendbuf, sendcounts, senddispls, sendtypes, 
                recvbuf, recvcounts, recvdispls, recvtypes, _val), emFLPFB );
    }
    void reduce( const void *sendbuf, void *recvbuf, int count, 
        MPI_Datatype dtype, MPI_Op op, int root ) const{
        ErrMPI::check(
            MPI_Reduce( sendbuf, recvbuf, count, dtype, 
                op, root, _val ), emFLPFB);
    }
    void allreduce( const void *sendbuf, void *recvbuf, int count, 
        MPI_Datatype dtype, MPI_Op op ) const{
        ErrMPI::check(
            MPI_Allreduce( sendbuf, recvbuf, count, dtype, 
                op, _val ), emFLPFB);
    }
    static void reduce_local( const void *inbuf, void *inoutbuf, int count, 
        MPI_Datatype dtype, MPI_Op op ){
        ErrMPI::check(
            MPI_Reduce_local( inbuf, inoutbuf, count, dtype, op ), emFLPFB);
    }
    void reduce_scatter_block( const void *sendbuf, void *recvbuf, 
        int recvcount, MPI_Datatype dtype, MPI_Op op ) const{
        ErrMPI::check(
            MPI_Reduce_scatter_block( sendbuf, recvbuf, recvcount, dtype, 
                op, _val ), emFLPFB);
    }
    void reduce_scatter( const void *sendbuf, void *recvbuf, 
        const int recvcounts[], MPI_Datatype dtype, MPI_Op op ) const{
        ErrMPI::check(
            MPI_Reduce_scatter(sendbuf, recvbuf, recvcounts,
                dtype, op, _val), emFLPFB);
    }
    void scan( const void *sendbuf, void *recvbuf, 
        int count, MPI_Datatype dtype, MPI_Op op ) const{
        ErrMPI::check(
            MPI_Scan(sendbuf, recvbuf, count, dtype, op, _val), emFLPFB );
    }
    void exscan( const void *sendbuf, void *recvbuf, 
        int count, MPI_Datatype dtype, MPI_Op op ) const{
        ErrMPI::check(
            MPI_Exscan( sendbuf, recvbuf, count, dtype, op, _val ), emFLPFB );
    }
    MPI_Request ibarrier() const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Ibarrier(_val, &req), emFLPFB );
        return req;
    }
    MPI_Request ibcast( 
        void *buf, int count, MPI_Datatype dtype, int root ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Ibcast( buf, count, dtype, root, _val, &req ), emFLPFB );
        return req;
    }
    MPI_Request igather( 
        const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Igather( sendbuf, sendcount, sendtype, 
                recvbuf, recvcount, recvtype, root, _val, &req ), emFLPFB );
        return req;
    }
    MPI_Request igatherv(
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
    MPI_Request iscatter(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Iscatter( sendbuf, sendcount, sendtype, 
            recvbuf, recvcount, recvtype, root, _val, &req ), emFLPFB
        );
        return req;
    }
    MPI_Request iscatterv(
        const void *sendbuf, const int sendcounts[], const int displs[], 
        MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype, int root) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Iscatterv( sendbuf, sendcounts, displs, sendtype, 
            recvbuf, recvcount, recvtype, root, _val, &req ), emFLPFB );
        return req;
    }
    MPI_Request iallgather( 
        const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Iallgather( sendbuf, sendcount, sendtype,
                recvbuf, recvcount, recvtype, _val, &req ), emFLPFB );
        return req;
    }
    MPI_Request iallgatherv(
        const void *sendbuf, int sendcount, MPI_Datatype sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        MPI_Datatype recvtype ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Iallgatherv( sendbuf, sendcount, sendtype,
                recvbuf, recvcounts, displs, recvtype, _val, &req ), emFLPFB );
        return req;
    }
    MPI_Request ialltoall( 
        const void *sendbuf, int sendcount, MPI_Datatype sendtype,
        void *recvbuf, int recvcount, MPI_Datatype recvtype ) const {
        MPI_Request req;
        ErrMPI::check(
            MPI_Ialltoall( sendbuf, sendcount, sendtype,
                recvbuf, recvcount, recvtype, _val, &req ), emFLPFB );
        return req;
    }
    MPI_Request ialltoallv( const void *sendbuf, const int sendcounts[], 
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
    MPI_Request ialltoallw( const void *sendbuf, const int sendcounts[], 
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
    MPI_Request ireduce( const void *sendbuf, void *recvbuf, int count, 
        MPI_Datatype dtype, MPI_Op op, int root ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Ireduce( sendbuf, recvbuf, count, dtype, op, root, _val, &req ), 
                emFLPFB );
        return req;
    }
    MPI_Request iallreduce( const void *sendbuf, void *recvbuf, int count, 
        MPI_Datatype dtype, MPI_Op op ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Iallreduce( sendbuf, recvbuf, count, dtype, op, _val, &req ), 
                emFLPFB );
        return req;
    }
    MPI_Request ireduce_scatter_block( const void *sendbuf, void *recvbuf, 
        int recvcount, MPI_Datatype dtype, MPI_Op op ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Ireduce_scatter_block( 
                sendbuf, recvbuf, recvcount, dtype, op, _val, &req ), 
                emFLPFB );
        return req;
    }
    MPI_Request ireduce_scatter( const void *sendbuf, void *recvbuf, 
        const int recvcounts[], MPI_Datatype dtype, MPI_Op op ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Ireduce_scatter( 
                sendbuf, recvbuf, recvcounts, dtype, op, _val, &req ), 
                emFLPFB );
        return req;   
    }
    MPI_Request iscan( const void *sendbuf, void *recvbuf, 
        int count, MPI_Datatype dtype, MPI_Op op ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Iscan( 
                sendbuf, recvbuf, count, dtype, op, _val, &req ), 
                emFLPFB );
        return req;
    }
    MPI_Request iexscan( const void *sendbuf, void *recvbuf, 
        int count, MPI_Datatype dtype, MPI_Op op ) const{
        MPI_Request req;
        ErrMPI::check(
            MPI_Iexscan( 
                sendbuf, recvbuf, count, dtype, op, _val, &req ), 
                emFLPFB );
        return req;
    }
protected:
    mpi_t _val;
    int _state;
};
    
} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_RAW_COMM_H_