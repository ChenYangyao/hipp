/**
create: Yangyao CHEN, 2021/05/13
    [write   ] DefaultLogger - Logger with predefined states.
*/ 
#ifndef _HIPPMPI_MPPROF_DEFAULT_LOGGER_H_
#define _HIPPMPI_MPPROF_DEFAULT_LOGGER_H_
#include "mpprof_logger.h"
namespace HIPP::MPI {

/**
The default logger of the profiling library, with predefined groups and states.
*/
class DefaultLogger : public Logger {
public:
    typedef Logger logger_t;
    typedef logger_t::state_manager_t state_manager_t;
    typedef logger_t::msg_t msg_t;
    typedef logger_t::guard_t guard_t;

    /**
    The indices of predefined states in the state array.
    The state id can be obtained from _state_name2id[index], such as 
    _state_name2id[SEND]. However, this should be managed and accessed only
    internally.
    
    @MAX_STATE_ID: the maximal possible index of the state array.
    */
    enum predefined_state : int { 
        SEND=0, BSEND=1, SSEND=2, RSEND=3, 
        ISEND=10,IBSEND=11, ISSEND=12, IRSEND=13,
        RECV=20, IRECV=30,
        
        BCAST=40, GATHER=41, GATHERV=42, SCATTER=43,
        SCATTERV=44, ALLGATHER=45, ALLGATHERV=46, ALLTOALL=47,
        ALLTOALLV=48, ALLTOALLW=49, REDUCE=50, ALLREDUCE=51,
        REDUCE_SCATTER=52, REDUCE_SCATTER_BLOCK=53, 
        SCAN=54, EXSCAN=55, BARRIER=56,
        
        IBCAST=70, IGATHER=71, IGATHERV=72, ISCATTER=73,
        ISCATTERV=74, IALLGATHER=75, IALLGATHERV=76, IALLTOALL=77,
        IALLTOALLV=78, IALLTOALLW=79, IREDUCE=80, IALLREDUCE=81,
        IREDUCE_SCATTER=82, IREDUCE_SCATTER_BLOCK=83, 
        ISCAN=84, IEXSCAN=85, IBARRIER=86,
        
        TICK=100, DURATION=101, 
        TICKSEND=110, DURATIONSEND=111,
        TICKRECV=110, DURATIONRECV=111,
        ALLTICK=120, ALLDURATION=121,

        MAX_STATE_ID=127 };

    /**
    Constructor
    Start a default logger in a communicator `global_comm`. P2P and collective
    events are defined within this communicator.

    The constructor is a collective call for `global_comm` - all involved 
    processes must call it with the same `logfilename`.

    @logfilename: file name of where the log records are dumpped.
    */
    DefaultLogger(Comm global_comm, const string &logfilename);
    ~DefaultLogger();

    DefaultLogger(const DefaultLogger &) = delete;
    DefaultLogger(DefaultLogger &&) = delete;
    DefaultLogger & operator=(const DefaultLogger &) = delete;
    DefaultLogger & operator=(DefaultLogger &&) = delete;

    /**
    The default logger defines the following events. A call to any of them 
    produces a new log record for a specific event. If the call returns a 
    guard (which means the event is a duration, not a time-point), the record
    is further modified to save the end of this event at the destruction of the 
    guard.

    All the events have a `msg` argument, which allows store extra user 
    information for later inspection.

    tick() and duration() - sequential events for a single process. `tick()` is 
        a time-point event, `duration()` is a duration event.

    ticksend() and tickrecv() - p2p time-point events for the sender and 
        receiver, respectively.
    durationsend() and durationrecv() - p2p duration event.
    
    alltick() and allduration() - collective time-point and duration events.

    Other functions are either p2p or collective duration event. They 
    correspond to MPI calls.
    */
    void tick(const msg_t &msg=msg_t::empty_msg);
    guard_t duration(const msg_t &msg=msg_t::empty_msg);

    void ticksend(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    void tickrecv(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t durationsend(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t durationrecv(int dest, int tag, const msg_t &msg=msg_t::empty_msg);

    guard_t send(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t bsend(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t ssend(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t rsend(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t isend(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t ibsend(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t issend(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t irsend(int dest, int tag, const msg_t &msg=msg_t::empty_msg);

    guard_t recv(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    guard_t irecv(int dest, int tag, const msg_t &msg=msg_t::empty_msg);
    
    void alltick(const msg_t &msg=msg_t::empty_msg);
    guard_t allduration(const msg_t &msg=msg_t::empty_msg);
    
    guard_t bcast(const msg_t &msg=msg_t::empty_msg);
    guard_t gather(const msg_t &msg=msg_t::empty_msg);
    guard_t gatherv(const msg_t &msg=msg_t::empty_msg);
    guard_t scatter(const msg_t &msg=msg_t::empty_msg);
    guard_t scatterv(const msg_t &msg=msg_t::empty_msg);
    guard_t allgather(const msg_t &msg=msg_t::empty_msg);
    guard_t allgatherv(const msg_t &msg=msg_t::empty_msg);
    guard_t alltoall(const msg_t &msg=msg_t::empty_msg);
    guard_t alltoallv(const msg_t &msg=msg_t::empty_msg);
    guard_t alltoallw(const msg_t &msg=msg_t::empty_msg);
    guard_t reduce(const msg_t &msg=msg_t::empty_msg);
    guard_t allreduce(const msg_t &msg=msg_t::empty_msg);
    guard_t reduce_scatter(const msg_t &msg=msg_t::empty_msg);
    guard_t reduce_scatter_block(const msg_t &msg=msg_t::empty_msg);
    guard_t scan(const msg_t &msg=msg_t::empty_msg);
    guard_t exscan(const msg_t &msg=msg_t::empty_msg);
    guard_t barrier(const msg_t &msg=msg_t::empty_msg);

    guard_t ibcast(const msg_t &msg=msg_t::empty_msg);
    guard_t igather(const msg_t &msg=msg_t::empty_msg);
    guard_t igatherv(const msg_t &msg=msg_t::empty_msg);
    guard_t iscatter(const msg_t &msg=msg_t::empty_msg);
    guard_t iscatterv(const msg_t &msg=msg_t::empty_msg);
    guard_t iallgather(const msg_t &msg=msg_t::empty_msg);
    guard_t iallgatherv(const msg_t &msg=msg_t::empty_msg);
    guard_t ialltoall(const msg_t &msg=msg_t::empty_msg);
    guard_t ialltoallv(const msg_t &msg=msg_t::empty_msg);
    guard_t ialltoallw(const msg_t &msg=msg_t::empty_msg);
    guard_t ireduce(const msg_t &msg=msg_t::empty_msg);
    guard_t iallreduce(const msg_t &msg=msg_t::empty_msg);
    guard_t ireduce_scatter(const msg_t &msg=msg_t::empty_msg);
    guard_t ireduce_scatter_block(const msg_t &msg=msg_t::empty_msg);
    guard_t iscan(const msg_t &msg=msg_t::empty_msg);
    guard_t iexscan(const msg_t &msg=msg_t::empty_msg);
    guard_t ibarrier(const msg_t &msg=msg_t::empty_msg);
protected:
    int _state_name2id[MAX_STATE_ID+1];
};

} // namespace HIPP::MPI
#endif	//_HIPPMPI_MPPROF_DEFAULT_LOGGER_H_