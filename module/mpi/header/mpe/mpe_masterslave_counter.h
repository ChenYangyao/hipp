#ifndef _HIPPMPI_MPE_MASTERSLAVE_COUNTER_H_
#define _HIPPMPI_MPE_MASTERSLAVE_COUNTER_H_
#include "mpe_base.h"
namespace HIPP{
namespace MPI{

#define _HIPP_TEMPHD template<typename ValueT>
#define _HIPP_TEMPARG <ValueT>

/**
 * a counter for task management
 * 
 * Given a group of processes, the Counter<> instance seperates them into one
 * master process and other slave processes. The master preserves a counter
 * ranging from [0, max_count). The slave process each can query the current 
 * value of the counter and then the counter is increased. 
 */
template<typename ValueT = int>
class Counter{
public:
    typedef ValueT value_t;

    /**
     * initialize the counter. 
     * The master and slaves will be taken from `comm`. The rank of master
     * int `comm` is `master_rank`.
     * @max_count:  maximum value the counter will provide. After achieving
     *              this value, the slave's query returns stop signal.
     * @commtag:    to initialize the master-slave model, some point-to-point
     *              communication in this `comm` must be performed. The 
     *              `commtag` is the tag value used in the communication.
     * @start:      whether the counting is start. If not, you may later call
     *              re_count() to start.
     * 
     * The initializer is a collective operation. All processes in `comm` must
     * call it and with the same value of `max_count`, `comm_tag`, 
     * `master_rank` and `start`.
     * 
     * It is user's responsibility to make sure the Counter instance in any 
     * process participating the counting is not deconstructed until all slaves 
     * receive stop signal.
     * 
     * The Counter cannot be copied, but it can be moved. After moving, all
     * status is transferred and the moved one is no longer usable.
     */
    Counter( const Comm &comm, value_t max_count, int commtag, 
        int master_rank=0, int start=1 );
    ~Counter() noexcept;
    Counter( const Counter & ) = delete;
    Counter( Counter && );
    Counter & operator=( const Counter & ) = delete;
    Counter & operator=( Counter && );


    bool is_master();
    bool is_slave();
    /**
     * for slave, call this to get a counter value.
     * If returning true, value is written a counter value. If returning false,
     * value is unchanged.
     */
    bool get( value_t &value );

    /**
     * change the max no. of count. 
     * This can be called only after the the counting process is finished - 
     * for slave, receiving the stop signal, or for master, exit the initializer
     * or re_count().
     * This call is collective. All involved processes must call it and with
     * same value of max_count.
     */
    Counter & set_max_count( value_t max_count ) noexcept;
    /**
     * restart the counting process.
     * This can be called only after the counting process is finished.
     * Barrier between two counting process is not necessary.
     */
    void re_count();
protected:
    static const Datatype & _value_mpi_t;
    static const int _tag_qeury = 0, _tag_rep = 1;

    Comm _inter_comm;
    value_t _max_count;
    int _state, _round;

    void _master_do();
    value_t _slave_get();
};


_HIPP_TEMPHD
const Datatype & Counter _HIPP_TEMPARG::_value_mpi_t
    = *_TypeCvt<typename Counter _HIPP_TEMPARG::value_t>::datatype;

_HIPP_TEMPHD
Counter _HIPP_TEMPARG::Counter( 
    const Comm &comm, value_t max_count, int commtag, int master_rank, 
    int start )
:_inter_comm(nullptr), _max_count(max_count), _state(0), _round(0)
{
    if( comm.is_null() )
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
            "  ... null communicator");
    int size = comm.size();
    if( size < 2 || master_rank < 0 || master_rank >= size )
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
            "  ... master rank ", master_rank, " and communicator size ", size,
            " are not allowed\n");
    
    _state = comm.rank() == master_rank;
    int remote_leader = _state ? int(master_rank==0) : master_rank ;
    _inter_comm = comm.split( _state )
        .create_inter( 0, comm, remote_leader, commtag );

    if( start && _state )
        _master_do();
}

_HIPP_TEMPHD 
Counter _HIPP_TEMPARG::~Counter() noexcept{}

_HIPP_TEMPHD 
Counter _HIPP_TEMPARG::Counter( Counter &&c )
:_inter_comm( std::move(c._inter_comm) ), _max_count(c._max_count),
_state(c._state), _round(c._round)
{}

_HIPP_TEMPHD 
Counter _HIPP_TEMPARG & 
Counter _HIPP_TEMPARG::operator=( Counter &&c ){
    if( &c != this ){
        _inter_comm = std::move(c._inter_comm);
        _max_count = c._max_count;
        _state = c._state;
        _round = c._round;
    }
    return *this;
}

_HIPP_TEMPHD
bool Counter _HIPP_TEMPARG::is_master(){
    return _state;
}

_HIPP_TEMPHD
bool Counter _HIPP_TEMPARG::is_slave(){
    return !_state;
}

_HIPP_TEMPHD
bool Counter _HIPP_TEMPARG::get( value_t &value ){
    value_t buff_value = _slave_get();
    bool ret = false;
    if( buff_value != _max_count ){
        value = buff_value;
        ret = true;
    }
    return ret;
}

_HIPP_TEMPHD
void Counter _HIPP_TEMPARG::re_count(){
    _round ^= 1;
    if( _state )
        _master_do();
}
_HIPP_TEMPHD
Counter _HIPP_TEMPARG & 
Counter _HIPP_TEMPARG::set_max_count( value_t max_count ) noexcept{
    _max_count = max_count;
    return *this;
}

_HIPP_TEMPHD
void Counter _HIPP_TEMPARG::_master_do(){
    value_t count = 0;
    int size = _inter_comm.remote_size(), done_size = 0;
    int tag_query = _tag_qeury + (_round << 1), 
        tag_rep = _tag_rep + (_round << 1);
    while( done_size < size ){
        Status status = _inter_comm.recv( ANY_SOURCE, 
            tag_query, BOTTOM, 0, INT );
        _inter_comm.send( status.source(), 
            tag_rep, &count, 1, _value_mpi_t );
        if( count != _max_count ){
            ++count;
        }else{
            ++done_size;
        }
    }
}

_HIPP_TEMPHD
typename Counter _HIPP_TEMPARG::value_t 
Counter _HIPP_TEMPARG::_slave_get(){
    int tag_query = _tag_qeury + (_round << 1), 
        tag_rep = _tag_rep + (_round << 1);

    _inter_comm.send(0, tag_query, BOTTOM, 0, INT);

    value_t buff_value;
    _inter_comm.recv(0, tag_rep, &buff_value, 1, _value_mpi_t);
    return buff_value;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG

} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPE_MASTERSLAVE_COUNTER_H_