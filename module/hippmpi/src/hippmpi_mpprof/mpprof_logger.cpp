#include <mpprof_logger.h>
#include <limits>

namespace HIPP::MPI {
namespace _mpprof_logger_helper {

ostream & msg_t::info(ostream &os, int fmt_cntl) const {
    os << "<msg_t> (msg=\"" << _msg << "\")";
    if( fmt_cntl ) os << '\n';
    return os;
}

group_t::group_t(int _id, const Group &global, const Group &local, 
    const msg_t &_msg)
: id(_id), local_rank(local.rank()), global_rank(global.rank()), 
msg(_msg.as_str())
{
    if(local_rank == UNDEFINED) 
        local_rank = NOT_IN_GROUP;
    if(global_rank == UNDEFINED)
        _el::throw_(_el::eINVALIDARG, emFLPFB, "  ... process (local rank=", 
        local_rank, ") does not in the global group\n");

    int n_local = local.size();
    ranks_in_global = local.translate_ranks(
        ALGORITHM::LinSpaced(0, n_local).get(), global);
    for(int i=0; i<n_local; ++i){
        if( ranks_in_global[i] == UNDEFINED )
            _el::throw_(_el::eINVALIDARG, emFLPFB, "  ... process (local rank=", 
            i, ") does not in the global group\n");
    }
}

ostream & group_t::info(ostream &os, int fmt_cntl) const {
    PStream p(os);
    p << "<group_t> (id=", id, ", msg=\"", msg, 
        "\", ranks=[", ranks_in_global, "])"; 
    if( fmt_cntl ) p << '\n';
    return os;
}

IO::H5XTable<group_t> group_t::_tbl_manip {
    "id", &group_t::id,
    "local_rank", &group_t::local_rank,
    "global_rank", &group_t::global_rank };

state_t::state_t(int _id, int _type, int _group_id, int _p2p_port, 
    int _p2p_match_id, const msg_t &_msg)
: id(_id), type(_type), group_id(_group_id), p2p_port(_p2p_port), 
p2p_match_id(_p2p_match_id), access(USER_CALL), msg(_msg.as_str())
{
    if( is_sequential() ){
        if( group_id != group_t::NOT_IN_GROUP || p2p_port != NO_P2P_PORT 
            || p2p_match_id != NO_P2P_MATCH_ID )
            _el::throw_(_el::eINVALIDARG, emFLPFB, "  ... sequential state", 
            "has invalid group setting (", group_id, ", ", p2p_port, 
            ", ", p2p_match_id, ")\n");
    }else if( is_p2p() ) {
        if( group_id == group_t::NOT_IN_GROUP || p2p_port == NO_P2P_PORT 
            || p2p_match_id == NO_P2P_MATCH_ID )
            _el::throw_(_el::eINVALIDARG, emFLPFB, "  ... p2p state", 
            "has invalid group setting (", group_id, ", ", p2p_port, 
            ", ", p2p_match_id, ")\n");
    }else if( is_collective() ) {
        if( group_id == group_t::NOT_IN_GROUP || p2p_port != NO_P2P_PORT 
            || p2p_match_id != NO_P2P_MATCH_ID )
            _el::throw_(_el::eINVALIDARG, emFLPFB, "  ... collective state", 
            "has invalid group setting (", group_id, ", ", p2p_port, 
            ", ", p2p_match_id, ")\n");
    }
}
ostream & state_t::info(ostream &os, int fmt_cntl) const {
    PStream p(os);
    p << "<state_t> (id=", id, ", type=", type, ", group_id=", group_id, 
        ", p2p_port=", p2p_port, ", p2p_match_id=", p2p_match_id,
        ", access=", access, ", msg=\"", msg, "\")";
    if( fmt_cntl ) p << '\n';
    return os;
}
IO::H5XTable<state_t> state_t::_tbl_manip {
    "id", &state_t::id,
    "type", &state_t::type,
    "group_id", &state_t::group_id,
    "p2p_port", &state_t::p2p_port,
    "p2p_match_id", &state_t::p2p_match_id,
    "access", &state_t::access
};
ostream & event_t::info(ostream &os, int fmt_cntl) const {
    PStream p(os);
    p << "<event_t> (id=", id, ", msg_bpos=", msg_bpos, ", state_id=", state_id, 
        ", p2p_dest=", p2p_dest, ", p2p_tag=", p2p_tag, 
        "epoch=[", epoch_begin, ", ", epoch_end, "])";
    if( fmt_cntl ) p << '\n';
    return os;
}
IO::H5XTable<event_t> event_t::_tbl_manip {
    "id", &event_t::id,
    "msg_bpos", &event_t::msg_bpos,
    "state_id", &event_t::state_id,
    "p2p_dest", &event_t::p2p_dest,
    "p2p_tag", &event_t::p2p_tag,
    "epoch_begin", &event_t::epoch_begin,
    "epoch_end", &event_t::epoch_end
};
ostream & stored_event_t::info(ostream &os, int fmt_cntl) const {
    PStream p(os);
    p << "<stored_event_t> (stored_id=", stored_id, 
        ", stored_epoch_end=", stored_epoch_end, ")";
    if( fmt_cntl ) p << '\n';
    return os;
}
IO::H5XTable<stored_event_t> stored_event_t::_tbl_manip {
    "stored_id", &stored_event_t::stored_id,
    "stored_epoch_end", &stored_event_t::stored_epoch_end
};
ostream & guard_t::info(ostream &os, int fmt_cntl) const {
    PStream p(os);
    p << "<guard_t> (event_id=", _event_id, ")";
    if( fmt_cntl ) p << '\n';
    return os;
}
state_manager_t::state_manager_t(Comm global_comm) 
:_global_comm(std::move(global_comm)), _global_group(_global_comm.group()), 
_global_mtx(_global_comm)
{
    // add the pre-defined group and states
    add_group(_global_group, "__Global group");
    
    int sb = add_sequential_state("__Logger begin", state_t::TIME_POINT),
        se = add_sequential_state("__Logger end", state_t::TIME_POINT);
    state(sb).mark_as_reserve();
    state(se).mark_as_reserve();
}
state_manager_t::~state_manager_t(){}
ostream & state_manager_t::info(ostream &os, int fmt_cntl) const {
    PStream p(os);
    p << "<state_manager_t> (global group size=", n_groups(), 
        ", no. groups=", _groups.size(), ", no. states=", n_states(), ")";
    if( fmt_cntl ) p << '\n';
    if( fmt_cntl >= 2 ) {
        if( n_groups() > 0 ){
            p << "  |- groups:\n";
            for(auto &g: _groups)
                p << "     ", g;
        }
        if( n_states() > 0 ){
            p << "  |- states:\n";
            for(auto &s: _states)
                p << "     ", s;
        }
    }
    return os;
}
int state_manager_t::add_group(const Group &g, const msg_t &msg) {
    int n_grps = _groups.size();
    _groups.emplace_back(n_grps, _global_group, g, msg);  
    return n_grps;
}
int state_manager_t::add_state(const msg_t &msg, int type, int group_id, 
    int p2p_port, int p2p_match_id) 
{
    int n_states = _states.size();
    _states.emplace_back(n_states, type, group_id, p2p_port, 
        p2p_match_id, msg);
    return n_states;
}
int state_manager_t::add_sequential_state(const msg_t &msg, int type) {
    return add_state(msg, type | state_t::SEQUENTIAL, 
        group_t::NOT_IN_GROUP,
        state_t::NO_P2P_PORT, state_t::NO_P2P_MATCH_ID);
}
int state_manager_t::add_p2p_state(const msg_t &msg, int type, int group_id, 
    int p2p_port, int p2p_match_id){
    return add_state(msg, type | state_t::P2P, group_id, p2p_port, 
        p2p_match_id);
}
int state_manager_t::add_collective_state(const msg_t &msg, int type, 
    int group_id) 
{
    return add_state(msg, type | state_t::COLLECTIVE, 
        group_id, state_t::NO_P2P_PORT, state_t::NO_P2P_MATCH_ID);
}
void state_manager_t::store(IO::H5Group g) const {
    auto g_grp = g.create_group("groups");
    group_t::_tbl_manip.write(_groups, g_grp);
    _store_msgs(_groups, g_grp);
    
    size_t nprocs = _global_comm.size();
    vector<int> ranks;
    for(auto &grp: _groups){
        auto &rs = grp.ranks_in_global; int n_rs = rs.size();
        ranks.insert(ranks.end(), rs.begin(), rs.end());
        ranks.insert(ranks.end(), nprocs-n_rs, -1);
    }
    g_grp.create_dataset<int>("ranks_in_global", 
        {_groups.size(), nprocs}, "excl").write(ranks);
    
    auto g_st = g.create_group("states");
    state_t::_tbl_manip.write(_states, g_st);
    _store_msgs(_states, g_st);
}
} // namespace _mpprof_logger_helper

Logger::Logger(std::shared_ptr<state_manager_t> sm, const string &logfilename)
: _sm(sm), _logfilename(logfilename), _n_stores(0), _first_event_id(0)
{
    auto &comm = _sm->global_comm();
    auto &mtx = _sm->global_mtx();
    int rank = comm.rank();

    // create the log file
    if(rank == 0){
        IO::H5File(_logfilename, "w");
    }

    // synchronize the starting epoch
    comm.barrier();
    push_sequential(state_t::LOGGER_BEGIN_ID);

    // dump the group and state data
    {   auto lk_g = mtx.lock_g();
        IO::H5File fout(_logfilename, "a");
        auto g = fout.create_group(str("rank.", rank));
        auto g_st = g.create_group("state_manager");
        _sm->store(g_st.create_group("begin"));
    }
}
Logger::~Logger() {
    auto &comm = _sm->global_comm();
    auto &mtx = _sm->global_mtx();
    int rank = comm.rank();
    
    // synchronize the ending epoch
    comm.barrier();
    push_sequential(state_t::LOGGER_END_ID);

    // dump the event data
    check_and_store(0);

    // dump the group and state data
    {   auto lk_g = mtx.lock_g(); 
        IO::H5File fout(_logfilename, "a");
        auto g_st = fout.open_group(str("rank.", rank, "/state_manager"));
        _sm->store(g_st.create_group("end"));
    }
}
ostream & Logger::info(ostream &os, int fmt_cntl) const {
    PStream p(os);
    if( fmt_cntl <= 0 ) {
        p <<HIPPCNTL_CLASS_INFO_INLINE(HIPP::MPI::Logger);
        return os;
    }
    p <<HIPPCNTL_CLASS_INFO(HIPP::MPI::Logger), 
        "  |- "; _sm->info(os, std::max(fmt_cntl, 1)); 
    p <<"  |- logfilename=", _logfilename, "\n"
        "  |- no. stores=", _n_stores, 
            ", first_event_id=", _first_event_id, "\n"
        "  |- no. events=", _events.size(), 
            ", no, stored_events=", _stored_events.size(), "\n"
        "  |- buff size=", buff_size(), "\n";
    return os;
}
void Logger::store() {
    auto &comm = _sm->global_comm();
    auto &mtx = _sm->global_mtx();
    int rank = comm.rank();
    
    // After dumping, event details are cleared. The only remaining is
    // the increased `_n_stores` and `_first_event_id`.
    int id_store = _n_stores++;
    _first_event_id = _new_event_id();

    {   auto lk_g = mtx.lock_g(); 

        IO::H5File fout(_logfilename, "a");
        auto g = fout.try_create_group(str("rank.", rank));
        auto gstore = g.create_group(str("store.", id_store));
    
        gstore.create_dataset_str("event_msgs", _event_msgs.size()+1)
            .write(_event_msgs);
        event_t::_tbl_manip.write(_events, gstore);
        stored_event_t::_tbl_manip.write(_stored_events, gstore);
    }
    _events.clear();
    _stored_events.clear();
    _event_msgs.clear();
}


} // namespace HIPP::MPI