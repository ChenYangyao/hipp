#include <mpprof_default_logger.h>
namespace HIPP::MPI {

DefaultLogger::DefaultLogger(Comm global_comm, const string &logfilename) 
: logger_t ( 
    std::make_shared<state_manager_t>(std::move(global_comm)), 
    logfilename)
{
    std::fill_n(_state_name2id, MAX_STATE_ID+1, -1);

    auto &sm = *_sm;
    typedef state_manager_t::state_t state_t;
    typedef state_manager_t::group_t group_t;
    
    // sequential state
    _state_name2id[TICK] = sm.add_sequential_state("__TICK", 
        state_t::TIME_POINT);
    _state_name2id[DURATION] = sm.add_sequential_state("__DURATION", 
        state_t::DURATION);
    
    constexpr int tp = state_t::TIME_POINT, dur = state_t::DURATION,
        group_id = group_t::GLOBAL_GROUP,
        sender = state_t::P2P_SENDER, recver = state_t::P2P_RECEIVER;

    // p2p state
    _state_name2id[TICKSEND] = sm.add_p2p_state(
        "__TICKSEND", tp, group_id, 
        state_t::P2P_SENDER, 1);
    _state_name2id[TICKRECV] = sm.add_p2p_state(
        "__TICKRECV", tp, group_id, 
        state_t::P2P_RECEIVER, 1);
    _state_name2id[DURATIONSEND] = sm.add_p2p_state(
        "__DURATIONSEND", dur, group_id, 
        state_t::P2P_SENDER, 2);
    _state_name2id[DURATIONRECV] = sm.add_p2p_state(
        "__DURATIONRECV", dur, group_id, 
        state_t::P2P_RECEIVER, 2);

    constexpr int p2p_match_id = 0;
    _state_name2id[SEND] = sm.add_p2p_state("__SEND", dur, 
        group_id, sender, p2p_match_id);
    _state_name2id[BSEND] = sm.add_p2p_state("__BSEND", dur, 
        group_id, sender, p2p_match_id);
    _state_name2id[SSEND] = sm.add_p2p_state("__SSEND", dur, 
        group_id, sender, p2p_match_id);
    _state_name2id[RSEND] = sm.add_p2p_state("__RSEND", dur, 
        group_id, sender, p2p_match_id);
    _state_name2id[ISEND] = sm.add_p2p_state("__ISEND", dur, 
        group_id, sender, p2p_match_id);
    _state_name2id[BSEND] = sm.add_p2p_state("__BSEND", dur, 
        group_id, sender, p2p_match_id);
    _state_name2id[SSEND] = sm.add_p2p_state("__SSEND", dur, 
        group_id, sender, p2p_match_id);
    _state_name2id[RSEND] = sm.add_p2p_state("__RSEND", dur, 
        group_id, sender, p2p_match_id);

    _state_name2id[RECV] = sm.add_p2p_state("__RECV", dur, 
        group_id, recver, p2p_match_id);
    _state_name2id[IRECV] = sm.add_p2p_state("__IRECV", dur, 
        group_id, recver, p2p_match_id);


    // collective state
    _state_name2id[ALLTICK] = sm.add_collective_state(
        "__ALLTICK", tp, group_id);
    _state_name2id[ALLDURATION] = sm.add_collective_state(
        "__ALLDURATION", dur, group_id);

    _state_name2id[BCAST] = sm.add_collective_state(
        "__BCAST", dur, group_id);
    _state_name2id[GATHER] = sm.add_collective_state(
        "__GATHER", dur, group_id);
    _state_name2id[GATHERV] = sm.add_collective_state(
        "__GATHERV", dur, group_id);
    _state_name2id[SCATTER] = sm.add_collective_state(
        "__SCATTER", dur, group_id);
    _state_name2id[SCATTERV] = sm.add_collective_state(
        "__SCATTERV", dur, group_id);
    _state_name2id[ALLGATHER] = sm.add_collective_state(
        "__ALLGATHER", dur, group_id);
    _state_name2id[ALLGATHERV] = sm.add_collective_state(
        "__ALLGATHERV", dur, group_id);
    _state_name2id[ALLTOALL] = sm.add_collective_state(
        "__ALLTOALL", dur, group_id);
    _state_name2id[ALLTOALLV] = sm.add_collective_state(
        "__ALLTOALLV", dur, group_id);
    _state_name2id[ALLTOALLW] = sm.add_collective_state(
        "__ALLTOALLW", dur, group_id);
    _state_name2id[REDUCE] = sm.add_collective_state(
        "__REDUCE", dur, group_id);
    _state_name2id[ALLREDUCE] = sm.add_collective_state(
        "__ALLREDUCE", dur, group_id);
    _state_name2id[REDUCE_SCATTER] = sm.add_collective_state(
        "__REDUCE_SCATTER", dur, group_id);
    _state_name2id[REDUCE_SCATTER_BLOCK] = sm.add_collective_state(
        "__REDUCE_SCATTER_BLOCK", dur, group_id);
    _state_name2id[SCAN] = sm.add_collective_state(
        "__SCAN", dur, group_id);
    _state_name2id[EXSCAN] = sm.add_collective_state(
        "__EXSCAN", dur, group_id);
    _state_name2id[BARRIER] = sm.add_collective_state(
        "__BARRIER", dur, group_id);

    _state_name2id[IBCAST] = sm.add_collective_state(
        "__IBCAST", dur, group_id);
    _state_name2id[IGATHER] = sm.add_collective_state(
        "__IGATHER", dur, group_id);
    _state_name2id[IGATHERV] = sm.add_collective_state(
        "__IGATHERV", dur, group_id);
    _state_name2id[ISCATTER] = sm.add_collective_state(
        "__ISCATTER", dur, group_id);
    _state_name2id[ISCATTERV] = sm.add_collective_state(
        "__ISCATTERV", dur, group_id);
    _state_name2id[IALLGATHER] = sm.add_collective_state(
        "__IALLGATHER", dur, group_id);
    _state_name2id[IALLGATHERV] = sm.add_collective_state(
        "__IALLGATHERV", dur, group_id);
    _state_name2id[IALLTOALL] = sm.add_collective_state(
        "__IALLTOALL", dur, group_id);
    _state_name2id[IALLTOALLV] = sm.add_collective_state(
        "__IALLTOALLV", dur, group_id);
    _state_name2id[IALLTOALLW] = sm.add_collective_state(
        "__IALLTOALLW", dur, group_id);
    _state_name2id[IREDUCE] = sm.add_collective_state(
        "__IREDUCE", dur, group_id);
    _state_name2id[IALLREDUCE] = sm.add_collective_state(
        "__IALLREDUCE", dur, group_id);
    _state_name2id[IREDUCE_SCATTER] = sm.add_collective_state(
        "__IREDUCE_SCATTER", dur, group_id);
    _state_name2id[IREDUCE_SCATTER_BLOCK] = sm.add_collective_state(
        "__IREDUCE_SCATTER_BLOCK", dur, group_id);
    _state_name2id[ISCAN] = sm.add_collective_state(
        "__ISCAN", dur, group_id);
    _state_name2id[IEXSCAN] = sm.add_collective_state(
        "__IEXSCAN", dur, group_id);
    _state_name2id[IBARRIER] = sm.add_collective_state(
        "__IBARRIER", dur, group_id);
}

DefaultLogger::~DefaultLogger() {}

void DefaultLogger::tick(const msg_t &msg) {
    push_sequential(_state_name2id[TICK], msg);
}
auto DefaultLogger::duration(const msg_t &msg) -> guard_t {
    return push_sequential_g(_state_name2id[DURATION], msg);
}
void DefaultLogger::ticksend(int dest, int tag, const msg_t &msg) {
    push_p2p(_state_name2id[TICKSEND], dest, tag, msg);
}
void DefaultLogger::tickrecv(int dest, int tag, const msg_t &msg) {
    push_p2p(_state_name2id[TICKRECV], dest, tag, msg);
}
auto DefaultLogger::durationsend(int dest, int tag, const msg_t &msg) 
-> guard_t 
{
    return push_p2p_g(_state_name2id[DURATIONSEND], dest, tag, msg);
}
auto DefaultLogger::durationrecv(int dest, int tag, const msg_t &msg) 
-> guard_t 
{
    return push_p2p_g(_state_name2id[DURATIONRECV], dest, tag, msg);
}
auto DefaultLogger::send(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[SEND], dest, tag, msg);
}
auto DefaultLogger::bsend(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[BSEND], dest, tag, msg);
}
auto DefaultLogger::ssend(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[SSEND], dest, tag, msg);
}
auto DefaultLogger::rsend(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[RSEND], dest, tag, msg);
}
auto DefaultLogger::isend(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[ISEND], dest, tag, msg);
}
auto DefaultLogger::ibsend(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[IBSEND], dest, tag, msg);
}
auto DefaultLogger::issend(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[ISSEND], dest, tag, msg);
}
auto DefaultLogger::irsend(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[IRSEND], dest, tag, msg);
}
auto DefaultLogger::recv(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[RECV], dest, tag, msg);
}
auto DefaultLogger::irecv(int dest, int tag, const msg_t &msg) -> guard_t {
    return push_p2p_g(_state_name2id[IRECV], dest, tag, msg);
}
void DefaultLogger::alltick(const msg_t &msg) {
    push_collective(_state_name2id[ALLTICK], msg);
}
auto DefaultLogger::allduration(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ALLDURATION], msg);
}
auto DefaultLogger::bcast(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[BCAST], msg);
}
auto DefaultLogger::gather(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[GATHER], msg);
}
auto DefaultLogger::gatherv(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[GATHERV], msg);
}
auto DefaultLogger::scatter(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[SCATTER], msg);
}
auto DefaultLogger::scatterv(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[SCATTERV], msg);
}
auto DefaultLogger::allgather(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ALLGATHER], msg);
}
auto DefaultLogger::allgatherv(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ALLGATHERV], msg);
}
auto DefaultLogger::alltoall(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ALLTOALL], msg);
}
auto DefaultLogger::alltoallv(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ALLTOALLV], msg);
}
auto DefaultLogger::alltoallw(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ALLTOALLW], msg);
}
auto DefaultLogger::reduce(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[REDUCE], msg);
}
auto DefaultLogger::allreduce(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ALLREDUCE], msg);
}
auto DefaultLogger::reduce_scatter(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[REDUCE_SCATTER], msg);
}
auto DefaultLogger::reduce_scatter_block(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[REDUCE_SCATTER_BLOCK], msg);
}
auto DefaultLogger::scan(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[SCAN], msg);
}
auto DefaultLogger::exscan(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[EXSCAN], msg);
}
auto DefaultLogger::barrier(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[BARRIER], msg);
}
auto DefaultLogger::ibcast(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IBCAST], msg);
}
auto DefaultLogger::igather(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IGATHER], msg);
}
auto DefaultLogger::igatherv(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IGATHERV], msg);
}
auto DefaultLogger::iscatter(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ISCATTER], msg);
}
auto DefaultLogger::iscatterv(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ISCATTERV], msg);
}
auto DefaultLogger::iallgather(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IALLGATHER], msg);
}
auto DefaultLogger::iallgatherv(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IALLGATHERV], msg);
}
auto DefaultLogger::ialltoall(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IALLTOALL], msg);
}
auto DefaultLogger::ialltoallv(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IALLTOALLV], msg);
}
auto DefaultLogger::ialltoallw(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IALLTOALLW], msg);
}
auto DefaultLogger::ireduce(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IREDUCE], msg);
}
auto DefaultLogger::iallreduce(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IALLREDUCE], msg);
}
auto DefaultLogger::ireduce_scatter(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IREDUCE_SCATTER], msg);
}
auto DefaultLogger::ireduce_scatter_block(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IREDUCE_SCATTER_BLOCK], msg);
}
auto DefaultLogger::iscan(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[ISCAN], msg);
}
auto DefaultLogger::iexscan(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IEXSCAN], msg);
}
auto DefaultLogger::ibarrier(const msg_t &msg) -> guard_t {
    return push_collective_g(_state_name2id[IBARRIER], msg);
}


} // namespace HIPP::MPI