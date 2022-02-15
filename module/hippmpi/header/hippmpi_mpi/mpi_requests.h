/**
create: Yangyao CHEN, 2020/01/22
    [write   ] Requests - the high-level MPI requests interface.
*/ 

#ifndef _HIPPMPI_MPI_REQUESTS_H_
#define _HIPPMPI_MPI_REQUESTS_H_

#include "mpi_status.h"

namespace HIPP{
namespace MPI{
    
class Comm;
class File;
class Win;
class Message;

/**
The high-level MPI requests interface.

A request is returned by a non-blocking communication call. A requests object 
host an array of requests (internally, an array of ``MPI_Requests``). The reason 
of allowing one object hosting an array of requests, 
not just a single one , is that the later may cause overhead in the 
multiple-completion calls.

The requests object can be copy/move-constructed/assigned. The copy-to 
object refers to the same array of requests. The copy operations, 
move operations and destructor are ``noexcept``.
*/
class Requests: public MPIObj<_Requests>{
public:
    typedef MPIObj<_Requests> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    /**
    Default constructor - construct an empty request array. Users may later 
    put new requests into the instance by ``put()`` or ``operator+=()``.
    */
    Requests();
    
    /**
    ``info()`` prints a short (``fmt_cntl=0``) or a verbose (``fmt_cntl=1``) 
    description of the current instance to the stream ``os``.
    Larger ``level`` produces more indents.

    Operator ``<<`` is equivalent to ``info()`` with default ``fmt_cntl`` and
    ``level``.

    The passed stream ``os`` is returned.
    */
    ostream &info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const Requests &rqs);

    /**
    free(): free all requests in this instance, and set the current instance to 
    a null value as returned by ``nullval()``. For persistent requests in the 
    array, ``free()`` frees them (so, make sure that they are completed by 
    completion calls). For other types of requests, ``free()`` requires that 
    they are already completed as become null values.
    
    clear(): similar to ``free()``, but sets the current instance to an empty 
    request array (length is 0).
    */
    void free();
    void clear();

    /**
    Inquire the information of the current request array.
    
    raw(): return the internal ``MPI_Request`` value.
    is_null(): test whether the request is a null value.
    
    Both have two overloads. The no-argument version is equivalent to call the 
    indexed version with ``i=0``.
    
    size(): return the size of the array.
    empty(): test whether the array is empty.
    */
    mpi_t raw(int i)const;
    bool is_null() const;
    bool is_null(int i) const;
    int size() const;
    bool empty() const;

    /**
    Return a null value, which is a length-1 request array whose only element 
    is a null request (internally ``MPI_REQUEST_NULL``).
    */
    static Requests nullval() noexcept;

    /**
    put(): transfer the requests from ``rqs`` into the current instance 
    (appended at the tail). ``rqs`` is set empty.
    
    operator+=(): equivalent to ``put()``.
    
    get(): does the opposite thing - extracting the request(s) from the current
    instance and return them.
    ``get(i)`` returns the i-th request. ``get(b, e)`` returns requests
    indexed in the range ``[b, e)``.
    After ``get()``, the returned requests are removed from the caller
    instance, the hole is filled by the remaining requests in the instance 
    where the order may change.
    */
    void put(Requests & rqs);
    void put(Requests && rqs);
    Requests & operator+=(Requests & rqs);
    Requests & operator+=(Requests && rqs);
    Requests get(int i);
    Requests get(int b, int e);

    /**
    start(i): start the i-th communication in the array. The request must be 
    returned from a persistent communication call. On exit, it becomes active.

    ``start()`` is equivalent to ``start(0)``.
    ``startall()`` is equivalent to starting each of the communications with 
    arbitrary order.
    */
    void start();
    void start(int i);
    void startall();

    /**
    Completion calls of the request(s). Please refer to the MPI Standard for 
    detailed semantics.

    ``wait()`` without argument is equivalent to ``wait(0)``. ``test(flag)`` is 
    equivalent to ``test(0, flag)``.

    ``status(flag)`` is equivalent to ``status(0, flag)``. The status call 
    returns flag=true if the communication is complete, and returns the a Status 
    object that describes the status of such. Otherwise it sets flag=false. 
    The status call differs from the test/wait call in that it does not 
    deallocate or inactivate the request.

    ``{test|wait}{all|some}`` without a ``statuses`` argument ignores the 
    statuses information.

    The ``vector`` version auto-resizes the vector to fit the number of 
    requests handled by this object. 
    */
    Status wait();
    Status wait(int i);

    Status test(int &flag);
    Status test(int i, int &flag);
    
    Status status(int &flag) const;
    Status status(int i, int &flag) const;
    
    Status waitany(int &index);
    Status testany(int &index, int &flag);
    
    void waitall(ContiguousBuffer<Status> statuses);
    void waitall(vector<Status> &statuses);
    void waitall();

    void testall(int &flag, ContiguousBuffer<Status> statuses);
    void testall(int &flag, vector<Status> &statuses);
    void testall(int &flag);

    void waitsome(int &count, ContiguousBuffer<int> indices, 
        ContiguousBuffer<Status> statuses);
    void waitsome(int &count, vector<int> &indices, 
        vector<Status> &statuses);
    void waitsome(int &count, ContiguousBuffer<int> indices);
    void waitsome(int &count, vector<int> &indices);

    void testsome(int &count, ContiguousBuffer<int> indices, 
        ContiguousBuffer<Status> statuses);
    void testsome(int &count, vector<int> &indices, 
        vector<Status> &statuses);
    void testsome(int &count, ContiguousBuffer<int> indices);
    void testsome(int &count, vector<int> &indices);

    /**
    Cancel the posted requests. ``cancel()`` is equivalent to ``cancel(0)``.
    */
    void cancel();
    void cancel(int i);
protected:
    static Requests _from_raw(mpi_t rq, int state);
    static Requests _from_raw_bare(mpi_t rq);
    static Requests _from_raw_free(mpi_t rq);
    friend class Comm;
    friend class File;
    friend class Win;
    friend class Message;
};

inline ostream & operator<<(ostream &os, const Requests &rqs){
    return rqs.info(os);
}

inline Requests Requests::_from_raw(mpi_t rq, int state){
    auto ptr = std::make_shared<_obj_raw_t>( rq, state );
    return Requests(ptr);
}

inline Requests Requests::_from_raw_bare(mpi_t rq) {
    return _from_raw(rq, 0);
}
inline Requests Requests::_from_raw_free(mpi_t rq) {
    return _from_raw(rq, _Requests::stFREE);
}

} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_REQUESTS_H_