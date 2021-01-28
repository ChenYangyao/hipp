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
#include "mpi_message.h"
#include "mpi_requests.h"
#include "mpi_win.h"
namespace HIPP{
namespace MPI{

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

    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Comm &comm );

    void free() noexcept;

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
     * point-to-point communication     
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
     * The second overload assumes the recv size and dtype are the same with 
     * those of the sending 
     */
    Status sendrecv(const Datapacket &send_dpacket, int dest, int sendtag, 
        const Datapacket &recv_dpacket, int src, int recvtag);
    Status sendrecv(const Datapacket &send_dpacket, int dest, int sendtag, 
        void *recvbuf, int src, int recvtag);
    Status sendrecv_replace(const Datapacket &dpacket, int dest, int sendtag, 
        int src, int recvtag);

    Status probe(int src, int tag) const;
    Status iprobe(int src, int tag, int &flag) const;
    std::pair<Status, Message> mprobe(int src, int tag) const;
    std::pair<Status, Message> improbe(int src, int tag, int &flag) const;

    /**
     * collective communication
     */
    void barrier() const;
    /**
     * Broadcast calls
     * Overloads:
     * 2: use a datapacket specification
     */
    void bcast(void *buf, int count, const Datatype &dtype, int root) const;
    void bcast(const Datapacket &dpacket, int root) const;
    /** 
     * Gather calls 
     * Overloads: 
     * 2 and 3 the send and recv has same dtype and count.
     */
    void gather(const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    void gather(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, int root) const;
    void gather(const Datapacket &send_dpacket, void *recvbuf, int root) const;
    void gatherv(
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype, int root ) const;
    /**
     * Scatter calls
     * Overloads: 
     * 2 and 3: the send and recv has same dtype and count.
     */
    void scatter(
        const void *sendbuf, int sendcount, const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root )const;
    void scatter(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, int root) const; 
    void scatter(const void *sendbuf, 
        const Datapacket &recv_dpacket, int root) const;
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
    /**
     * Reduce calls
     * Overloads:
     * 2: the count and dtype are taken from send_dpacket.
     * 
     * The same applies for allreduce().
     */
    void reduce( const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op, int root ) const;
    void reduce( const Datapacket &send_dpacket, void *recvbuf,
        const Oppacket &op, int root ) const;
    void allreduce( const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op ) const;
    void allreduce( const Datapacket &send_dpacket, void *recvbuf, 
        const Oppacket &op ) const;
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
    Requests ibcast(const Datapacket &dpacket, int root) const;
    Requests igather( 
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, int recvcount, const Datatype &recvtype, int root) const;
    Requests igather(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, int root) const;
    Requests igather(const Datapacket &send_dpacket, 
        void *recvbuf, int root) const;
    Requests igatherv(
        const void *sendbuf, int sendcount, const Datatype &sendtype, 
        void *recvbuf, const int recvcounts[], const int displs[],
        const Datatype &recvtype, int root ) const;
    Requests iscatter(
        const void *sendbuf, int sendcount, const Datatype &sendtype,
        void *recvbuf, int recvcount, const Datatype &recvtype, int root )const;
    Requests iscatter(const void *sendbuf, void *recvbuf, 
        int count, const Datatype &dtype, int root) const; 
    Requests iscatter(const void *sendbuf, 
        const Datapacket &recv_dpacket, int root) const;
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
    Requests ireduce( const Datapacket &send_dpacket, void *recvbuf,
        const Oppacket &op, int root ) const;
    Requests iallreduce( const void *sendbuf, void *recvbuf, int count, 
        const Datatype &dtype, const Oppacket &op ) const;
    Requests iallreduce( const Datapacket &send_dpacket, void *recvbuf, 
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

inline ostream & operator<<( ostream &os, const Comm &comm )
    { return comm.info(os); }

inline Comm Comm::_from_raw(mpi_t obj, int state) noexcept{
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
    Datapacket dp( std::forward<Args>(args)... );
    _obj_ptr->send( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
}

template<typename ...Args>
void Comm::bsend( int dest, int tag, Args && ...args ) const{
    Datapacket dp( std::forward<Args>(args)... );
    _obj_ptr->bsend( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
}

template<typename ...Args>
void Comm::ssend( int dest, int tag, Args && ...args ) const{
    Datapacket dp( std::forward<Args>(args)... );
    _obj_ptr->ssend( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
}

template<typename ...Args>
void Comm::rsend( int dest, int tag, Args && ...args ) const{
    Datapacket dp( std::forward<Args>(args)... );
    _obj_ptr->rsend( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
}

template<typename ...Args>
Status Comm::recv( int src, int tag, Args && ...args ) const{
    Datapacket dp( std::forward<Args>(args)... );
    return _obj_ptr->recv( dp._buff, dp._size, dp._dtype.raw(), src, tag );
}

template<typename ...Args>
Requests Comm::isend( int dest, int tag, Args && ...args ) const{
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->isend( dp._buff, dp._size, dp._dtype.raw(), dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::ibsend( int dest, int tag, Args && ...args ) const{
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->ibsend( dp._buff, dp._size, dp._dtype.raw(), 
        dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::issend( int dest, int tag, Args && ...args ) const{
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->issend( dp._buff, dp._size, dp._dtype.raw(), 
        dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::irsend( int dest, int tag, Args && ...args ) const{
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->irsend( dp._buff, dp._size, dp._dtype.raw(), 
        dest, tag );
    return Requests::_from_raw( rq, 0 );
}

template<typename ...Args>
Requests Comm::irecv( int src, int tag, Args && ...args ) const{
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _obj_ptr->irecv( dp._buff, dp._size, dp._dtype.raw(), src, tag );
    return Requests::_from_raw( rq, 0 );
}

} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_COMM_H_