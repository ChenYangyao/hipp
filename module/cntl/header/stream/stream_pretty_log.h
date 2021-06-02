/**
create: Yangyao CHEN, 2021/05/31
    [write   ] PLogStream - the pretty stream for producing log.
*/
#ifndef _HIPPCNTL_STREAM_PRETTY_LOG_H_
#define _HIPPCNTL_STREAM_PRETTY_LOG_H_
#include "stream_base.h"
#include "stream_pretty.h"
namespace HIPP {

class PLogStream;

namespace _hippcntl_stream_pretty_log_helper {

/**
Guard of the push operation of PLogStream.

On the calling of a guarded push, the PLogStream releases a guard object. 
On the destruction of the guard object, the side effect of push operation is
restored.
*/
class Guard {
public:
    Guard(PLogStream &pls, bool hint_pop=false);
    
    Guard(Guard &&) noexcept;
    Guard & operator=(Guard &&) noexcept;

    Guard(const Guard &) = delete;
    Guard & operator=(const Guard &) = delete;
    
    ~Guard() noexcept;

    /**
    Switch on/off the hint on pop.
    If on, an extra entry is output to the PLogStream at the destruction of 
    the guard (similar to calling `PLogStream::pop(true)` ).
    */
    void hint_pop_on() noexcept;
    void hint_pop_off() noexcept;

    /**
    Attribute getter - whether the hint on pop is turned on.
    */
    bool hint_pop() const noexcept;

    /**
    Change the `level` of the PLogStream. 
    @scoped: if true, the guard object is responsible for restoring the old 
        `level`. Otherwise the `level` is really changed - the guard does not
        restore it.
    */
    void set_level(int level, bool scoped=true) noexcept;
private:
    PLogStream *_pls;
    bool _hint_pop;
    int _level_old;

    void _destroy() noexcept;
};

/**
The stream operand of PLogStream. See the description of 
`PLogStream::operator<<` for the detail usage of the stream operand.
*/
class StreamOperand: protected PStream::stream_op_t {
public:
    typedef PStream::stream_op_t parent_t;
    using parent_t::it_pair_t;

    StreamOperand(ostream &os, bool enabled) noexcept;
    StreamOperand(const parent_t & op, bool enabled) noexcept;

    StreamOperand(const StreamOperand &) noexcept = default;
    StreamOperand(StreamOperand &&) noexcept = default;
    StreamOperand & operator=(const StreamOperand &) noexcept = default;
    StreamOperand & operator=(StreamOperand &&) noexcept = default;
    ~StreamOperand() noexcept {}

    StreamOperand & operator, (ostream& (*pf)(ostream&));
    StreamOperand & operator, (std::ios& (*pf)(std::ios&)) ;
    StreamOperand & operator, (std::ios_base& (*pf)(std::ios_base&));
    
    template<typename T>
    StreamOperand & operator,(T &&x);

    ostream & get_stream() const noexcept;
private:
    bool _enabled;
};

} // namespace _hippcntl_stream_pretty_log_helper

/**
PLogStream - pretty stream for producing log.

Description
-----------
PLogStream is like PStream, has overloaded `<<` operator which can then be 
chained using the comma operator.
e.g.,
plog << "Begin function main, got (", vector{1,2,3}, ")", endl;

PLogStream has extra methods to produce log records. Its features include
- Log entries are nested with indents hinting the stack height, i.e., PLogStream 
  allows entering/leaving scopes, and produces extra indents in the more inner
  scope.
- The type of information is printed can be controlled, i.e., log entries can be 
  filtered according to its priority level.
*/
class PLogStream: protected PStream {
public:
    typedef _hippcntl_stream_pretty_log_helper::Guard guard_t;
    typedef _hippcntl_stream_pretty_log_helper::StreamOperand stream_op_t;

    enum : int { 
        LV_EMERG=0,   LV_ALERT=1,  LV_CRIT=2, LV_ERR=3, 
        LV_WARNING=4, LV_NOTICE=5, LV_INFO=6, LV_DEBUG=7, 
        LV_MIN=-1,    LV_MAX=100};
    enum : size_t {
        ENTRY_PREFIX_MAXLEN = 32
    };

    /**
    Constructor.
    @os:    which the log output are directed to.
    @level: the current priority level.
    @level_used:    under which level the information should be printed.
    @stack_height:  the depth of nesting.
    */
    PLogStream(ostream &os, int level=LV_NOTICE, int level_used=LV_NOTICE, 
        int stack_height=0) noexcept;

    PLogStream(const PLogStream &that) noexcept;
    PLogStream(PLogStream &&that) noexcept;

    PLogStream & operator=(const PLogStream &) = delete;
    PLogStream & operator=(PLogStream &&) = delete;
    ~PLogStream() noexcept {}

    /**
    Attribute setters.
    set_level() - current priority level.
    set_level_used() - used priority level. Log entries with 
        `level <= level_used` will be printed.
    set_indent() - number of extra spaces padded at the front of each log entry.
    set_entry_prefix() - prefix padded at the front of each log entry. The 
        prefix should be no longer than `ENTRY_PREFIX_MAXLEN-1`. Otherwise
        it is truncated.
    */
    PLogStream & set_level(int level=LV_NOTICE) noexcept;
    PLogStream & set_level_used(int level_used=LV_NOTICE) noexcept;
    PLogStream & set_indent(int indent=2) noexcept;
    PLogStream & set_entry_prefix(const string &entry_prefix="|- ") noexcept;
    
    /**
    Attribute getters.
    stack_height() - current stack height. If the height is non-zero,
        `indent*stack_height` spaces and `entry prefix` are printed for each 
        entry. Otherwise nothing is padded.
    Other attributes are explained in the corresponding setters.
    */
    int level() const noexcept;
    int level_used() const noexcept;
    int indent() const noexcept;
    string entry_prefix() const noexcept;
    int stack_height() const noexcept;

    /**
    Output an object to this stream.
    The object can be an standard formatter `pf`, or any object `x` that is 
    printable to PStream.

    The returned `stream_op_t` object allows chaining the subsequent outputs
    using comma operator, e.g., plog << "the object ", x, endl;

    The contents output by `<<` and `,` in a single statement is viewed as 
    a single log entry. Paddings, as controlled by `indent` and `entry_prefix`
    is added.
    */
    stream_op_t operator<< (ostream& (*pf)(ostream&));
    stream_op_t operator<< (std::ios& (*pf)(std::ios&));
    stream_op_t operator<< (std::ios_base& (*pf)(std::ios_base&));
    template<typename T>
    stream_op_t operator<<(T &&x);

    /**
    `push()` increases the stack height by 1, while `pop()` decreases it by 1.
    These two operations are used to increse the indent of log entry.

    It is always recommended to use the guarded version `push_g()` and 
    `push_at()` instead of the direct `push()` and `pop()`. (RAII style)

    For push(), if `args` are non-empty, they are ouput into the stream, with
    `indent*stack_height` spaces padding at front. Then, stack height is 
    increased.

    For pop(), if `hint` is true, an entry is output, and then the stack height
    is decreased.
    */
    template<typename ...Args>
    void push(Args &&...args);
    void pop(bool hint=false);
    
    /**
    The guarded versions of `push()`.
    push_g() - call `push()` and returns a guard.
    push_at() - change the level to `level`, call `push()`, and returns a guard.

    The guard is responsible for restoring the `stack_height` on its destruction
    (similar to calling `pop()`). The `level` is also restored.
    */
    template<typename ...Args>
    guard_t push_g(Args &&...args);
    template<typename ...Args>
    guard_t push_at(int level, Args &&...args);

    template<typename It>
    stream_op_t::it_pair_t<It> operator()(It b, It e);

    ostream & get_stream() const noexcept;
protected:
    int _level;
    int _level_used;
    int _stack_height;
    
    int _indent = 2;
    char _entry_prefix[ENTRY_PREFIX_MAXLEN] = "|- ";

    stream_op_t _prt_entry_prefix();
    stream_op_t _prt_indent();
    bool _prt_is_on() const noexcept;
};


/**
Global static variable
*/
extern PLogStream plog;

/**
Implementation
*/
namespace _hippcntl_stream_pretty_log_helper {

#define _HIPP_TEMPRET inline auto Guard::
#define _HIPP_TEMPNORET inline Guard::

_HIPP_TEMPNORET Guard(PLogStream &pls, bool hint_pop) 
: _pls(&pls), _hint_pop(hint_pop), _level_old(pls.level())
{}

_HIPP_TEMPNORET Guard(Guard &&that) noexcept 
: _pls(that._pls), _hint_pop(that._hint_pop), _level_old(that._level_old)
{
    that._pls = nullptr;
}

_HIPP_TEMPRET Guard::operator=(Guard &&that) noexcept -> Guard & {
    _destroy();
    _pls = that._pls;
    _hint_pop = that._hint_pop;
    _level_old = that._level_old;
    that._pls = nullptr;
    return *this;
}

_HIPP_TEMPNORET ~Guard() noexcept {
    _destroy();
}

inline void Guard::hint_pop_on() noexcept {
    _hint_pop = true;
}

inline void Guard::hint_pop_off() noexcept {
    _hint_pop = false;
}

inline bool Guard::hint_pop() const noexcept {
    return _hint_pop;
}

inline void Guard::set_level(int level, bool scoped) noexcept {
    _pls->set_level(level);
    if( !scoped ) 
        _level_old = level;
}

inline void Guard::_destroy() noexcept {
    if( _pls ) {
        _pls->pop(_hint_pop);
        _pls->set_level(_level_old);
    }
}

#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPRET inline auto StreamOperand::
#define _HIPP_TEMPNORET inline StreamOperand::

_HIPP_TEMPNORET StreamOperand(ostream &os, bool enabled) noexcept 
: parent_t(os), _enabled(enabled) {}

_HIPP_TEMPNORET StreamOperand(const parent_t & op, bool enabled) noexcept 
: parent_t(op), _enabled(enabled) {}

_HIPP_TEMPRET operator, (ostream& (*pf)(ostream&)) -> StreamOperand & { 
    if(_enabled) 
        this->parent_t::operator,(pf); 
    return *this; 
}
_HIPP_TEMPRET operator, (std::ios& (*pf)(std::ios&))  -> StreamOperand & { 
    if(_enabled) 
        this->parent_t::operator,(pf); 
    return *this; 
}
_HIPP_TEMPRET operator, (std::ios_base& (*pf)(std::ios_base&)) 
-> StreamOperand & { 
    if(_enabled) 
        this->parent_t::operator,(pf); 
    return *this; 
}

template<typename T>
_HIPP_TEMPRET operator,(T &&x) -> StreamOperand & {
    if(_enabled) 
        this->parent_t::operator,(std::forward<T>(x));
    return *this;
}

_HIPP_TEMPRET get_stream() const noexcept -> ostream & { 
    return this->parent_t::get_stream(); 
}

#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

} // namespace _hippcntl_stream_pretty_log_helper

#define _HIPP_TEMPRET inline auto PLogStream::
#define _HIPP_TEMPNORET inline PLogStream::

_HIPP_TEMPNORET PLogStream(ostream &os, int level, int level_used, 
    int stack_height) noexcept 
: PStream(os), _level(level), _level_used(level_used), _stack_height(0)
{}

_HIPP_TEMPNORET PLogStream(const PLogStream &that) noexcept 
: PLogStream(that.get_stream(), that._level, that._level_used, 
that._stack_height)
{
    set_indent(that._indent);
    set_entry_prefix(that._entry_prefix);
}

_HIPP_TEMPNORET PLogStream(PLogStream &&that) noexcept 
: PLogStream(that.get_stream(), that._level, that._level_used, 
that._stack_height)
{
    set_indent(that._indent);
    set_entry_prefix(that._entry_prefix);
}

_HIPP_TEMPRET set_level(int level) noexcept -> PLogStream & {
    _level = level;
    return *this;
}

_HIPP_TEMPRET set_level_used(int level_used) noexcept -> PLogStream & {
    _level_used = level_used;
    return *this;
}

_HIPP_TEMPRET set_indent(int indent) noexcept -> PLogStream & {
    _indent = indent;
    return *this;
}

_HIPP_TEMPRET set_entry_prefix(const string &entry_prefix) noexcept 
-> PLogStream & 
{
    size_t sz_used = std::min(entry_prefix.size(), ENTRY_PREFIX_MAXLEN-1);
    std::copy_n(entry_prefix.begin(), sz_used, _entry_prefix);
    _entry_prefix[sz_used] = '\0';
    return *this;
}

_HIPP_TEMPRET level() const noexcept -> int {
    return _level;
}

_HIPP_TEMPRET level_used() const noexcept -> int {
    return _level_used;
}


_HIPP_TEMPRET indent() const noexcept -> int {
    return _indent;
}

_HIPP_TEMPRET entry_prefix() const noexcept -> string {
    return _entry_prefix;
}

_HIPP_TEMPRET stack_height() const noexcept -> int {
    return _stack_height;
}

_HIPP_TEMPRET operator<< (ostream& (*pf)(ostream&)) -> stream_op_t {
    return _prt_entry_prefix(), pf;
}

_HIPP_TEMPRET operator<< (std::ios& (*pf)(std::ios&)) -> stream_op_t {
    return _prt_entry_prefix(), pf;
}

_HIPP_TEMPRET operator<< (std::ios_base& (*pf)(std::ios_base&)) -> 
    stream_op_t 
{
    return _prt_entry_prefix(), pf;
}

template<typename T>
_HIPP_TEMPRET operator<<(T &&x) -> stream_op_t {
    return _prt_entry_prefix(), std::forward<T>(x);
}

template<typename ...Args>
_HIPP_TEMPRET push(Args &&...args) -> void {
    if constexpr(sizeof...(Args) > 0) {
        (_prt_indent(), ..., std::forward<Args>(args)), endl;
    }
    ++_stack_height;
}

_HIPP_TEMPRET pop(bool hint) -> void {
    if(hint) 
        (*this) << "Exit (stack=", _stack_height, ")", endl;
    --_stack_height;
}

template<typename ...Args>
_HIPP_TEMPRET push_g(Args &&...args) -> guard_t {
    push(std::forward<Args>(args)...);
    return guard_t {*this, false};
}

template<typename ...Args>
_HIPP_TEMPRET push_at(int level, Args &&...args) -> guard_t {
    guard_t g {*this, false};
    g.set_level(level);
    push(std::forward<Args>(args)...);
    return guard_t {std::move(g)};
}

template<typename It>
_HIPP_TEMPRET operator()(It b, It e) -> stream_op_t::it_pair_t<It> { 
    return {b, e}; 
}

_HIPP_TEMPRET get_stream() const noexcept -> ostream & { 
    return this->PStream::get_stream(); 
}

_HIPP_TEMPRET _prt_entry_prefix() -> stream_op_t {
    if( _stack_height == 0 ) return {_op, _prt_is_on()};
    return _prt_indent(), _entry_prefix;
}

_HIPP_TEMPRET _prt_indent() -> stream_op_t {
    bool is_on = _prt_is_on();
    if( is_on ){
        int n_prt = _indent * _stack_height;
        for(int i=0; i<n_prt; ++i) _op, ' ';
    }
    return {_op, is_on};
}

_HIPP_TEMPRET _prt_is_on() const noexcept -> bool {
    return _level <= _level_used;
}

#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

} // namespace HIPP
#endif	//_HIPPCNTL_STREAM_PRETTY_LOG_H_
