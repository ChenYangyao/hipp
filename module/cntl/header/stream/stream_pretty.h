#ifndef _HIPPCNTL_STREAM_PRETTY_H_
#define _HIPPCNTL_STREAM_PRETTY_H_
#include "../incl/incl.h"
#include "../error/error.h"
namespace HIPP {

namespace _hippcntl_stream_pretty_helper {
class StreamOperand {
public:
    explicit StreamOperand(ostream &os) noexcept : _os(os) {}

    StreamOperand & operator, (ostream& (*pf)(ostream&))
        { _os << pf; return *this; }
    StreamOperand & operator, (std::ios& (*pf)(std::ios&)) 
        { _os << pf; return *this; }
    StreamOperand & operator, (std::ios_base& (*pf)(std::ios_base&))
        { _os << pf; return *this; }
    template<typename T>
    StreamOperand & operator,(T &&x) { 
        _os << std::forward<T>(x); 
        return *this; 
    }

    ostream & get_stream() const noexcept { return _os; }
private:
    ostream &_os;
}; 
} // namespace _hippcntl_stream_pretty_helper


class PStream {
public:
    typedef _hippcntl_stream_pretty_helper::StreamOperand stream_op_t;

    PStream(ostream &os) noexcept: _op(os) {}
    PStream(const PStream &that) noexcept: PStream(that._op.get_stream()){}
    PStream(PStream &&that) noexcept: PStream(that._op.get_stream()) {}
    PStream & operator=(const PStream &that) =delete;
    PStream & operator=(PStream &&that) =delete;
    ~PStream() noexcept {}

    stream_op_t & operator<< (ostream& (*pf)(ostream&))
        { _op.get_stream() << pf; return _op; }
    stream_op_t & operator<< (std::ios& (*pf)(std::ios&)) 
        { _op.get_stream() << pf; return _op; }
    stream_op_t & operator<< (std::ios_base& (*pf)(std::ios_base&))
        { _op.get_stream() << pf; return _op; }
    template<typename T>
    stream_op_t & operator<<(T &&x) { 
        _op.get_stream() << std::forward<T>(x); 
        return _op;
    }

    ostream & get_stream() const noexcept { return _op.get_stream(); }
protected:
    stream_op_t _op;
};

extern PStream pout;
extern PStream perr;


} // namespace HIPP
#endif	//_HIPPCNTL_STREAM_PRETTY_H_