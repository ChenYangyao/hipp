/**
 * creat: Yangyao CHEN, 2020/01/22
 *      [write   ] Requests - the high-level MPI requests interface.
 */ 

#ifndef _HIPPMPI_MPI_REQUESTS_H_
#define _HIPPMPI_MPI_REQUESTS_H_
#include "mpi_obj_base.h"
#include "mpi_raw_requests.h"
namespace HIPP{
namespace MPI{
    
class Comm;
class File;

/**
 * the high-level MPI requests interface.
 * 
 * Requests controlls the life time of an array of MPI_Requests. We choose to 
 * build the object consisting of an array of requests but not a single request 
 * object, since the later may cause large overhaed in the multiple-completion
 * call of non-blocking communication.
 */
class Requests: public MPIObj<_Requests>{
public:
    typedef MPIObj<_Requests> _obj_base_t;
    using _obj_base_t::_obj_base_t;

    /**
     * constructor
     * Construct a empty request array. Users may later put new requists into
     * the instance by put() or operator+=().
     */
    Requests();
    
    ostream &info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Requests &rqs );

    /**
     * free all requests in this instance, and set the current instance to
     * a null value as returned by nullval(). 
     * 
     * clear() is similar to free() set the current instance to an empty 
     * request array. The difference is that the null value is a length-1 array, 
     * with a null value, but an empty array is length-0.
     */
    void free();
    void clear();

    /**
     * inquery the information of the current request array.
     * raw() -  access the original MPI_Request instance in the current array.
     * is_null() - 
     *          test whether the request is a null value.
     * Both of these two have two overloaded versions, without the argument, 
     * it returnes the information of the first request in this array, other-
     * wise returns the i-th specified by the argument.
     * 
     * size() - return the size of the array.
     * empty() -
     *          test whether the array is empty.
     */
    mpi_t raw(int i)const;
    bool is_null() const;
    bool is_null(int i) const;
    int size() const;
    bool empty() const;

    /**
     * return a null value, which is a length-1 request array with the only 
     * element to be a null value.
     */
    static Requests nullval() noexcept;

    /**
     * put() transfer the requests in `rqs` into the current instance (appended 
     * at the tail of the request array). `rqs` will become empty.
     * 
     * operator+=() is equivalent to put().
     * 
     * get() does the opposite thing, extracting the request(s) in the current
     * instance and return them. 
     * get(i) returns the i-th request, get(b, e) returns a range of requests
     * specified indexed with [b, e).
     * After calling get(), the returned requests are removed from the caller
     * instance, the hole is filled by the tail elements remaining in the caller
     * instance.
     */
    void put( Requests & rqs);
    void put( Requests && rqs);
    Requests & operator+=( Requests & rqs );
    Requests & operator+=( Requests && rqs );
    Requests get( int i );
    Requests get( int b, int e );

    /**
     * test the completion of the request(s).
     */
    Status wait();
    Status wait(int i);
    Status test(int &flag);
    Status test(int i, int &flag);
    Status status(int &flag) const;
    Status status(int i, int &flag) const;
    Status waitany(int &index);
    Status testany(int &index, int &flag);
    void waitall(vector<Status> &statuses);
    void testall(int &flag, vector<Status> &statuses);
    void waitsome( int &count, vector<int> &indices, vector<Status> &statuses);
    void testsome( int &count, vector<int> &indices, vector<Status> &statuses);
protected:
    static Requests _from_raw(mpi_t rq, int state);
    friend class Comm;
    friend class File;
};

inline ostream & operator<<( ostream &os, const Requests &rqs ){
    return rqs.info(os);
}

inline Requests Requests::_from_raw(mpi_t rq, int state){
    auto ptr = std::make_shared<_obj_raw_t>( rq, state );
    return Requests(ptr);
}

} // namespace MPI
} // namespace HIPP
#endif	//_HIPPMPI_MPI_REQUESTS_H_