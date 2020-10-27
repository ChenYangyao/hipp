/**
 * creat: Yangyao CHEN, 2020/10/23
 *      [write   ] Message - 
 */ 

#ifndef _HIPPMPI_MPI_MESSAGE_H_
#define _HIPPMPI_MPI_MESSAGE_H_
#include "mpi_obj_base.h"
#include "mpi_raw_comm.h"
#include "mpi_status.h"
#include "mpi_requests.h"
namespace HIPP {
namespace MPI{

class Comm;

/**
 * the message object for probe.
 */
class Message {
public:
    typedef MPI_Message mpi_t;
    Message() noexcept {}
    Message(mpi_t message) noexcept : _message(message) { }
    ~Message() noexcept {}

    static Message nullval() noexcept { return Message(MPI_MESSAGE_NULL); }
    static Message no_proc_val() noexcept { return Message(MPI_MESSAGE_NO_PROC); }
    bool is_null() const noexcept { return _message == MPI_MESSAGE_NULL; }
    bool is_no_proc() const noexcept { return _message == MPI_MESSAGE_NO_PROC; }

    template<typename ...Args>
    Status mrecv(Args && ...args);
    template<typename ...Args>
    Requests imrecv(Args && ...args);
protected:
    friend class Comm;
    mpi_t _message;
};

template<typename ...Args>
Status Message::mrecv(Args && ...args){
    Datapacket dp( std::forward<Args>(args)... );
    return _Comm::mrecv(dp._buff, dp._size, dp._dtype.raw(), _message);
}
template<typename ...Args>
Requests Message::imrecv(Args && ...args){
    Datapacket dp( std::forward<Args>(args)... );
    auto rq = _Comm::imrecv(dp._buff, dp._size, dp._dtype.raw(), _message);
    return Requests::_from_raw(rq, 0);
}

} // namespace MPI
} // namespace HIPP



#endif	//_HIPPMPI_MPI_MESSAGE_H_