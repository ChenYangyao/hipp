#include <mpprof_logger.h>
#include <limits>

namespace HIPP::MPI {
namespace _mpprof_logger_helper {

ostream & msg_t::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps {os};
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Logger::msg_t), "{msg=", _msg, '}';
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);  
        ps << HIPPCNTL_CLASS_INFO(Logger::msg_t),
            ind, "Msg=", _msg, '\n';
    }
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

ostream & group_t::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps {os};
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Logger::group_t), 
            "{id=", id, ", msg=", msg, ", ranks={", ranks_in_global, "}}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);  
        ps << HIPPCNTL_CLASS_INFO(Logger::group_t),
            ind, "Id=", id, ", msg=", msg, '\n',
            ind, "Ranks in global={", ranks_in_global, "}\n";
    }
    return os;
}

IO::H5::XTable<group_t> group_t::_tbl_manip {
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
ostream & state_t::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps {os};
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Logger::state_t), 
            "{id=", id, ", type=", type, ", group_id=", group_id, 
            ", P2P port=", p2p_port, ", P2P match_id=", p2p_match_id,
            ", access=", access, ", msg=\"", msg, "\"}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);  
        ps << HIPPCNTL_CLASS_INFO(Logger::state_t),
            ind, "Id=", id, ", type=", type, ", group_id=", group_id, '\n',
            ind, "P2P port=", p2p_port, ", P2P match id=", p2p_match_id, '\n',
            ind, "Access=", access, ", msg=\"", msg, "\"\n";
    }
    return os;
}
IO::H5::XTable<state_t> state_t::_tbl_manip {
    "id", &state_t::id,
    "type", &state_t::type,
    "group_id", &state_t::group_id,
    "p2p_port", &state_t::p2p_port,
    "p2p_match_id", &state_t::p2p_match_id,
    "access", &state_t::access
};
ostream & event_t::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps {os};
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Logger::event_t), 
            "{id=", id, ", msg_bpos=", msg_bpos, ", state_id=", state_id, 
            ", P2P dest=", p2p_dest, ", P2P tag=", p2p_tag, 
            ", epoch=(", epoch_begin, ", ", epoch_end, ")}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);  
        ps << HIPPCNTL_CLASS_INFO(Logger::event_t),
            ind, "Id=", id, ", msg_bpos=", msg_bpos, 
                ", state_id=", state_id, '\n', 
            ind, "P2P dest=", p2p_dest, ", P2P tag=", p2p_tag, '\n',
            ind, "Epoch=(", epoch_begin, ", ", epoch_end, ")}\n";
    }
    return os;
}
IO::H5::XTable<event_t> event_t::_tbl_manip {
    "id", &event_t::id,
    "msg_bpos", &event_t::msg_bpos,
    "state_id", &event_t::state_id,
    "p2p_dest", &event_t::p2p_dest,
    "p2p_tag", &event_t::p2p_tag,
    "epoch_begin", &event_t::epoch_begin,
    "epoch_end", &event_t::epoch_end
};
ostream & stored_event_t::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps {os};
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Logger::stored_event_t), 
            "{stored_id=", stored_id, 
            ", stored_epoch_end=", stored_epoch_end, "}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);  
        ps << HIPPCNTL_CLASS_INFO(Logger::stored_event_t),
            ind, "Stored id=", stored_id, 
                ", stored epoch end=", stored_epoch_end, '\n';
            
    }
    return os;
}
IO::H5::XTable<stored_event_t> stored_event_t::_tbl_manip {
    "stored_id", &stored_event_t::stored_id,
    "stored_epoch_end", &stored_event_t::stored_epoch_end
};
ostream & guard_t::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps {os};
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Logger::guard_t), 
            "{event_id=", _event_id, "}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);  
        ps << HIPPCNTL_CLASS_INFO(Logger::guard_t),
            ind, "Event id=", _event_id, '\n';
            
    }
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
ostream & state_manager_t::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps {os};
    auto n_grps = n_groups(), n_st = n_states();
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Logger::state_manager_t), 
            "{no. groups=", n_grps, ", no. states=", n_st, "}";
    } else {
        auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);  
        string ind_less(' ', ind.size()),
            ind_more(' ', ind.size()+HIPPCNTL_CLASS_INFO_INDENT);
        ps << HIPPCNTL_CLASS_INFO(Logger::state_manager_t),
            ind, n_grps, " groups";
        if( n_grps > 0 ) {
            ps << " {\n", ind_more, _groups[0];
            for(size_t i=1; i<n_grps; ++i){
                ps << ",\n", ind_more, _groups[i];
            }
            ps << '\n', ind_less, '}';
        }
        ps << '\n';
        ps << ind, n_st, " states";
        if( n_st > 0 ) {
            ps << "{\n", ind_more, _states[0];
            for(size_t i=1; i<n_st; ++i){
                ps << ",\n", ind_more, _states[i];
            }
            ps << '\n', ind_less, '}';
        }
        ps << '\n';
            
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
void state_manager_t::store(IO::H5::Group g) const {
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
        IO::H5::File(_logfilename, "w");
    }

    // synchronize the starting epoch
    comm.barrier();
    push_sequential(state_t::LOGGER_BEGIN_ID);

    // dump the group and state data
    {   auto lk_g = mtx.lock_g();
        IO::H5::File fout(_logfilename, "a");
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
        IO::H5::File fout(_logfilename, "a");
        auto g_st = fout.open_group(str("rank.", rank, "/state_manager"));
        _sm->store(g_st.create_group("end"));
    }
}

ostream & Logger::info(ostream &os, int fmt_cntl, int level) const {
    PStream ps(os);
    if( fmt_cntl == 0 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Logger), "{state manager=", *_sm, 
            "log file name=", _logfilename, ", no, stores=", _n_stores,
            ", no. events=", _events.size(), '}';
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);  
    ps <<HIPPCNTL_CLASS_INFO(Logger), 
        ind, "State manager="; _sm->info(os, fmt_cntl, level+1); 
    ps << ind, "Log file name=", _logfilename, '\n',
         ind, "No. stores=", _n_stores, 
            ", first event id=", _first_event_id, '\n',
         ind, "No. events=", _events.size(), 
            ", no, stored_events=", _stored_events.size(), '\n',
         ind, "Buff size=", buff_size(), '\n';
    return os;
}

void Logger::store(bool shrink_buff) {
    auto &comm = _sm->global_comm();
    auto &mtx = _sm->global_mtx();
    int rank = comm.rank();
    
    // After dumping, event details are cleared. The only remaining is
    // the increased `_n_stores` and `_first_event_id`.
    int id_store = _n_stores++;
    _first_event_id = _new_event_id();

    {   auto lk_g = mtx.lock_g(); 

        IO::H5::File fout(_logfilename, "a");
        auto g = fout.create_group(str("rank.", rank), "ac");
        auto gstore = g.create_group(str("store.", id_store));
    
        gstore.create_dataset_str("event_msgs", _event_msgs.size()+1)
            .write_str(_event_msgs);
        event_t::_tbl_manip.write(_events, gstore);
        stored_event_t::_tbl_manip.write(_stored_events, gstore);
    }
    _events.clear();
    _stored_events.clear();
    _event_msgs.clear();
    if( shrink_buff ) {
        _events.shrink_to_fit();
        _stored_events.shrink_to_fit();
        _event_msgs.shrink_to_fit();
    }
}


} // namespace HIPP::MPI