#ifndef _HIPPMPI_MPPROF_LOGGER_H_
#define _HIPPMPI_MPPROF_LOGGER_H_
#include "mpprof_base.h"
namespace HIPP::MPI {
class Logger;
namespace _mpprof_logger_helper {
typedef ErrLogic _el;

/**
mst_t - produce a valid message to put into the message field of group, state,
or event.

The message may be an ordinary message constructed from a string, or a styled
message that holds extra fields. The libray is responsible to represent them 
with a proper format, e.g., JSON or YAML.

Any string prefixed with a double underline "__" is reserved by the library, 
and cannot be used as an ordinary message.
*/
class msg_t {
public:
    enum : int {MSG_KEY=0, SIZE_KEY=1};             /** Used as the key of a key-value pair. */
    static const msg_t empty_msg;                   /** An empty message instance. */

    /** Empty message */
    msg_t() {}

    /** Ordinary message */
    msg_t(const string &msg): _msg(std::move(msg)) {}
    msg_t(const char *msg): _msg(msg) {}
    
    /** Message with key-value pair. */
    template<typename T, typename ...Args>
    msg_t(int key, T &&val, Args &&...args);

    /**
    Generage a message with a `size` field and a `msg` field, to hold together 
    a size information and an ordinary message.
    @size: size, e.g., the size of an event.
    @msg: an ordinary message.
    */
    static msg_t sized(size_t size, const string &msg="") {
        return msg_t(SIZE_KEY, size, MSG_KEY, msg);
    }

    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const msg_t &msg);

    /** 
    Refer to the underlying representation. For the ordinary-constructed 
    message, the underlying representation is the same as the message passed
    by the user. Otherwise it may be different.
    */
    const string & as_str() const { return _msg; }
    string & as_str()  { return _msg; }
private:
    string _msg;

    template<typename T>
    static void _append(string &msg, int key, T &&val);
    template<typename T, typename ...Args>
    static void _append_all(string &msg, int key, T &&val, Args &&...args);
};

inline const msg_t msg_t::empty_msg {};

struct group_t {
    enum : int {NOT_IN_GROUP=-1, GLOBAL_GROUP=0};

    int id;        
    vector<int> ranks_in_global;
    int local_rank, global_rank;
    string msg;

    group_t(int _id, const Group &global, 
        const Group &local, const msg_t &_msg=msg_t::empty_msg);

    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const group_t &g);

    static IO::H5XTable<group_t> _tbl_manip;  
};

/**
@id: `NOT_STATE` | a real state index in [0, N_states).
    Predefined indices: `LOGGER_BEGIN_ID` and `LOGGER_END_ID` are states 
    for the begining and ending events of the logger.
@type: `a | b`, where `a` is one from {`TIME_POINT`, `DURATION`}, and `b` is 
        one from {`SEQUENTIAL`, `P2P`, `COLLECTIVE`}.
    Type of the state. The combination gives 6 possible types.
@group_id: `group_t::NOT_IN_GROUP` (for sequential) | a real group index in 
    [0, N_groups) (for p2p or collective).
@p2p_port: `NO_P2P_PORT` (for non-p2p) | `P2P_SENDER` | `P2P_RECEIVER`.
@p2p_match_id:  sender and receiver states with the same `p2p_match_id` are 
    matched in a p2p event.
@access: `SYSTEM_RESERVED_CALL` | `USER_CALL`.
*/
struct state_t {
    enum : int {NOT_STATE=-1, LOGGER_BEGIN_ID=0, LOGGER_END_ID=1};   // for `id`
    enum : int {                                           // for `type`= a | b
        TIME_POINT=0x0, DURATION=0x1,                      // a
        SEQUENTIAL=0x0, P2P=0x10, COLLECTIVE=0x20          // b
    };
    enum : int {                                                    
        NO_P2P_PORT=-1, P2P_SENDER=0, P2P_RECEIVER=1,      // for `p2p_port`
        NO_P2P_MATCH_ID=-1};                               // for `p2p_match_id`
    enum : int {SYSTEM_RESERVED_CALL=0, USER_CALL=1};      // for `access`

    int id;
    int type;
    int group_id, p2p_port, p2p_match_id;
    int access;
    string msg;

    state_t(int _id, int _type, int _group_id, int _p2p_port, 
        int _p2p_match_id, const msg_t &_msg=msg_t::empty_msg);
    void mark_as_reserve() { access = SYSTEM_RESERVED_CALL; }

    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const state_t &s);

    bool is_time_point() const noexcept { return (type & 0xf) == TIME_POINT; }
    bool is_duration() const noexcept { return (type & 0xf) == DURATION; }
    bool is_sequential() const noexcept { return (type & 0xf0) == SEQUENTIAL; }
    bool is_p2p() const noexcept  { return (type & 0xf0) == P2P; }
    bool is_collective() const noexcept { return (type & 0xf0) == COLLECTIVE; }

    static IO::H5XTable<state_t> _tbl_manip;
};

struct event_t {
    enum : int {NO_P2P_DEST=-1, NO_P2P_TAG=-1};
    enum : long long {INVALID_ID=-1};

    long long id;                       
    long long msg_bpos;             // allow a variable length
    int state_id;
    int p2p_dest, p2p_tag;          // significant only for p2p event
    double epoch_begin, epoch_end;  // for time_point, epoch_end ignored

    
    event_t(long long _id, long long _msg_bpos, int _state_id, double _p2p_dest, 
        double _p2p_tag, double _epoch_begin);
    void set_epoch_end(double _epoch_end) noexcept { epoch_end = _epoch_end; }

    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const event_t &e);

    static IO::H5XTable<event_t> _tbl_manip;
};

// if the event is already stored, use stored_event_t for the duration end.
struct stored_event_t {
    long long stored_id;
    double stored_epoch_end;

    stored_event_t(long long id, double epoch_end) noexcept
        : stored_id(id), stored_epoch_end(epoch_end) {}

    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const stored_event_t &e);

    static IO::H5XTable<stored_event_t> _tbl_manip;
};  

class guard_t {
public:
    guard_t(Logger &logger, long long event_id) noexcept 
    : _logger(logger), _event_id(event_id) {}
    
    ~guard_t() { end_push(); }

    guard_t(const guard_t &) = delete;
    guard_t(guard_t &&) = delete;
    guard_t & operator=(const guard_t &) = delete;
    guard_t & operator=(guard_t &&) = delete;
    void end_push();

    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const guard_t &gd);
private:
    Logger &_logger;
    long long _event_id;
};

class state_manager_t {
public:
    typedef _mpprof_logger_helper::msg_t msg_t;
    typedef _mpprof_logger_helper::group_t group_t;
    typedef _mpprof_logger_helper::state_t state_t;

    state_manager_t(Comm global_comm);
    ~state_manager_t();
    state_manager_t(const state_manager_t &) = delete;
    state_manager_t(state_manager_t &&) = delete;
    state_manager_t & operator=(const state_manager_t &) = delete;
    state_manager_t & operator=(state_manager_t &&) = delete;

    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const state_manager_t &sm);

    int add_group(const Group &g, const msg_t &msg);
    
    /**
    Define a new state. Its index is returned.
    Three shortcuts are provided for sequential, p2p and collective states, 
    respectively, so that `type` may be only `state_t::TIME_POINT` or 
    `state_t::DURATION`.
    */
    int add_state(const msg_t &msg, int type, int group_id, 
        int p2p_port, int p2p_match_id);
    int add_sequential_state(const msg_t &msg, int type);
    int add_p2p_state(const msg_t &msg, int type, int group_id, 
        int p2p_port, int p2p_match_id);
    int add_collective_state(const msg_t &msg, int type, int group_id);

    /** Attribute getters. */
    size_t n_groups() const noexcept { return _groups.size(); }
    size_t n_states() const noexcept { return _states.size(); }

    const group_t & group(int group_id) const { return _groups[group_id]; }
    const state_t & state(int state_id) const { return _states[state_id]; }
    const Comm & global_comm() const { return _global_comm; }
    const Group & global_group() const { return _global_group; }
    const Mutex & global_mtx() const { return _global_mtx; }
    
    group_t & group(int group_id) { return _groups[group_id]; }
    state_t & state(int state_id) { return _states[state_id]; }
    Comm & global_comm() { return _global_comm; }
    Group & global_group() { return _global_group; }
    Mutex & global_mtx() { return _global_mtx; }

    /** Store the groups and states into a HDF5 group. */
    void store(IO::H5Group g) const;
private:
    Comm _global_comm;
    Group _global_group;
    Mutex _global_mtx;

    vector<group_t> _groups;
    vector<state_t> _states;

    template<typename T>
    static void _store_msgs(const vector<T> &objs, IO::H5Group &g);
};

} // namespace _mpprof_logger_helper    

/**
The logger for profiling.

Logger takes a set of predefined group and states (defined by the state 
manager) as meta infomation and produces corresponding log records for events.

The records are saved in memory first but can be moved to file by calling 
`store()` or `check_and_store()`. 

Logger is responsible for saving required information, like the start and end 
time of each event, the initialization time of the logger system, and other 
information that may be useful for profiling.
*/
class Logger {
public:
    typedef _mpprof_logger_helper::state_manager_t state_manager_t;
    typedef state_manager_t::msg_t msg_t;
    typedef state_manager_t::group_t group_t;
    typedef state_manager_t::state_t state_t;
    typedef _mpprof_logger_helper::event_t event_t;
    typedef _mpprof_logger_helper::stored_event_t stored_event_t;
    typedef _mpprof_logger_helper::guard_t guard_t;

    typedef std::chrono::high_resolution_clock clk_t;
    typedef std::chrono::duration<double> sec_t;

    /** 
    Constructor 
    Start a logger with state manager `sm`. `sm` is not owned by the logger, 
    so that you can still add new group/state to it.
    
    @logfilename: file name of where the log records are dumpped.
    */
    Logger(std::shared_ptr<state_manager_t> sm, const string &logfilename);
    
    ~Logger();

    /**
    The Logger cannot be copied or moved, since those may invalidate the 
    guards.
    */
    Logger(const Logger &) = delete;
    Logger(Logger &&) = delete;
    Logger & operator=(const Logger &) = delete;
    Logger & operator=(Logger &&) = delete;

    /**
    Print some information os this logger to the stream `os`.
    @fmt_cntl: 0 for a short and inline information. 1 for a verbose and block 
        version.

    The operator<< is equivalent to `info()` with `fmt_cntl = 1`.
    */
    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const Logger &lg);

    /**
    Add a new log record.
    @state_id: meta info describing this log record.
    @msg_t: extra information to be held by this record.
    @dest: @tag: for a P2P event, must provide a valid dest and tag as in the 
        communication calls - those will be used by the profiling tools.
    
    An event id is returned, which can be passed into `end_push()` to end 
    the record, if the event is a duration.
    For a time point event, `event_t::INVALID_ID` is returned, and should never
    be passed to `end_push()`.

    To avoid any manual error, use the guarded version instead.
    */
    long long push_sequential(int state_id, const msg_t &msg=msg_t::empty_msg);
    long long push_p2p(int state_id, int dest, int tag, 
        const msg_t &msg=msg_t::empty_msg);
    long long push_collective(int state_id, const msg_t &msg=msg_t::empty_msg);
    void end_push(long long event_id);

    /**
    The guarded version of push calls, i.e., RAII idiom is adopted.
    The returned guard is responsible for endding the record at its destruction.
    */
    guard_t push_sequential_g(int state_id, const msg_t &msg=msg_t::empty_msg);
    guard_t push_p2p_g(int state_id, int dest, int tag, 
        const msg_t &msg=msg_t::empty_msg);
    guard_t push_collective_g(int state_id, const msg_t &msg=msg_t::empty_msg);

    /**
    buff_size() - Get the used buffer size (in byte) in memory for log records.
    store() - Save the records into file. This can be called even when some 
        guards are still active.
    check_and_store() - Check if the used buffer size (in byte) exceeds
        `max_buff_size` (by default, it is 32M). If so, store them into file.

    @shrink_buff: if true, the buffer is shrunk after storing. Otherwise they 
        are preserved to accelerate later recording.
    */
    size_t buff_size() const noexcept;
    void store(bool shrink_buff=false);
    void check_and_store(size_t max_buff_size=1024*1024*32, 
        bool shrink_buff=false);

    /**
    Getters.
    state_manager() returns the current state manager. 
    clock_tick() returns the current value of the clock (in second) used by the 
        logger.
    */
    const state_manager_t & state_manager() const { return *_sm; }
    state_manager_t & state_manager() { return *_sm; }
    double clock_tick() const { return _clock_tick(); }
protected:
    typedef ErrLogic _el;
    
    std::shared_ptr<state_manager_t> _sm;
    string _logfilename;
    int _n_stores;
    long long _first_event_id;

    vector<event_t> _events;
    vector<stored_event_t> _stored_events;
    string _event_msgs;

    double _clock_tick() const {
        return std::chrono::duration_cast<sec_t>(
            clk_t::now().time_since_epoch()).count();
    }
    long long _push_msg(const msg_t &msg) {
        size_t bpos = _event_msgs.size();
        _event_msgs += msg.as_str();
        return bpos;
    }
    long long _new_event_id() noexcept {
        return _first_event_id + static_cast<long long>(_events.size());
    }
};


namespace _mpprof_logger_helper {

template<typename T, typename ...Args>
msg_t::msg_t(int key, T &&val, Args &&...args) {
    _msg = "__j{";
    _append(_msg, key, val);
    if constexpr( sizeof...(Args) > 0 )
        _append_all(_msg, std::forward<Args>(args)...);
    _msg += "}";    
}

inline ostream & operator<<(ostream &os, const msg_t &msg) { 
    return msg.info(os); 
}

template<typename T>
void msg_t::_append(string &msg, int key, T &&val) {
    if(key == SIZE_KEY){
        msg += str("\"__size\":", std::forward<T>(val));
    }else{
        msg += str("\"__msg\":\"", std::forward<T>(val), '"');
    }
}
template<typename T, typename ...Args>
void msg_t::_append_all(string &msg, int key, T &&val, Args &&...args) {
    msg += ',';
    _append(msg, key, std::forward<T>(val));
    if constexpr( sizeof...(Args) > 0 ) 
        _append_all(msg, std::forward<Args>(args)...);
}

inline ostream & operator<<(ostream &os, const group_t &g) { 
    return g.info(os); 
}
inline ostream & operator<<(ostream &os, const state_t &s) { 
    return s.info(os); 
}

inline event_t::event_t(long long _id, long long _msg_bpos, int _state_id, 
    double _p2p_dest, double _p2p_tag, double _epoch_begin)
:id(_id), msg_bpos(_msg_bpos), state_id(_state_id), p2p_dest(_p2p_dest), 
p2p_tag(_p2p_tag), epoch_begin(_epoch_begin), 
epoch_end(std::numeric_limits<double>::lowest()) { }


inline ostream & operator<<(ostream &os, const event_t &e) { 
    return e.info(os); 
}
inline ostream & operator<<(ostream &os, const stored_event_t &e) { 
    return e.info(os); 
}
inline void guard_t::end_push() {
    if( _event_id != event_t::INVALID_ID ) {
        _logger.end_push(_event_id);
        _event_id = event_t::INVALID_ID;
    }
}
inline ostream & operator<<(ostream &os, const guard_t &gd) { 
    return gd.info(os); 
}
template<typename T>
inline void state_manager_t::_store_msgs(const vector<T> &objs, IO::H5Group &g) {
    vector<string> msgs;
    for(const auto &obj: objs) msgs.emplace_back(obj.msg);
    g.create_dataset<string>("msg", 
        IO::H5TypeStr::shape(msgs), "excl").write(msgs);
}
inline ostream & operator<<(ostream &os, const state_manager_t &sm) { 
    return sm.info(os); 
}
} // namespace _mpprof_logger_helper
inline ostream & operator<<(ostream &os, const Logger &lg) { 
    return lg.info(os); 
}
inline long long Logger::push_sequential(int state_id, const msg_t &msg) {
    auto &st = _sm->state(state_id);
    if( !st.is_sequential() )
        _el::throw_(_el::eDEFAULT, emFLPFB, "  ... state (id=", 
        state_id, ", type=", st.type, ") is not a sequential state\n");
    long long id = _new_event_id();
    _events.emplace_back(id, _push_msg(msg), 
        state_id, event_t::NO_P2P_DEST, event_t::NO_P2P_TAG, _clock_tick());
    return st.is_time_point() ? event_t::INVALID_ID : id;
}
inline long long Logger::push_p2p(int state_id, int dest, int tag, 
    const msg_t &msg) {
    auto &st = _sm->state(state_id);
    if( !st.is_p2p() )
        _el::throw_(_el::eDEFAULT, emFLPFB, "  ... state (id=", 
        state_id, ", type=", st.type, ") is not a p2p state\n");
    long long id = _new_event_id();
    _events.emplace_back(id, _push_msg(msg), state_id, dest, tag, 
        _clock_tick());
    return st.is_time_point() ? event_t::INVALID_ID : id;
}
inline long long Logger::push_collective(int state_id, const msg_t &msg) {
    auto &st = _sm->state(state_id);
    if( !st.is_collective() )
        _el::throw_(_el::eDEFAULT, emFLPFB, "  ... state (id=", 
        state_id, ", type=", st.type, ") is not a collective state\n");
    long long id = _new_event_id();
    _events.emplace_back(id, _push_msg(msg), state_id, event_t::NO_P2P_DEST, 
        event_t::NO_P2P_TAG, _clock_tick());
    return st.is_time_point() ? event_t::INVALID_ID : id;
}
inline void Logger::end_push(long long event_id) {
    double e = _clock_tick();
    if(event_id < _first_event_id){
        _stored_events.emplace_back(event_id, e);
    }else{
        _events[event_id-_first_event_id].set_epoch_end(e);
    }
}
inline Logger::guard_t Logger::push_sequential_g(int state_id, const msg_t &msg) 
{
    return guard_t(*this, push_sequential(state_id, msg));
}
inline Logger::guard_t Logger::push_p2p_g(int state_id, int dest, int tag, 
    const msg_t &msg) {
    return guard_t(*this, push_p2p(state_id, dest, tag, msg));
}
inline Logger::guard_t Logger::push_collective_g(int state_id, const msg_t &msg) 
{
    return guard_t(*this, push_collective(state_id, msg));
}
inline size_t Logger::buff_size() const noexcept {
    return _events.size()*sizeof(event_t) + 
        _stored_events.size()*sizeof(stored_event_t) +
        _event_msgs.size();   
}
inline void Logger::check_and_store(size_t max_buff_size, bool shrink_buff) {
    if( buff_size() > max_buff_size ) 
        store(shrink_buff);
}

} // namespace HIPP::MPI
#endif	//_HIPPMPI_MPPROF_LOGGER_H_