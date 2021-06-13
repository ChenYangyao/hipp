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

    DefaultLogger(Comm global_comm, const string &logfilename);
    ~DefaultLogger();

    DefaultLogger(const DefaultLogger &) = delete;
    DefaultLogger(DefaultLogger &&) = delete;
    DefaultLogger & operator=(const DefaultLogger &) = delete;
    DefaultLogger & operator=(DefaultLogger &&) = delete;

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