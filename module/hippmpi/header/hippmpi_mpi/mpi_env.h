/**
create: Yangyao CHEN, 2020/01/21
    [write   ] Env - the MPI environment inquiry and management class.
*/ 

#ifndef _HIPPMPI_MPI_ENV_H_
#define _HIPPMPI_MPI_ENV_H_

#include "mpi_comm.h"

#define HIPPMPI_STD_VERSION MPI_VERSION
#define HIPPMPI_STD_SUBVERSION MPI_SUBVERSION
#define HIPPMPI_STD ( MPI_VERSION*100 + MPI_SUBVERSION )

namespace HIPP{
namespace MPI{

namespace _mpi_env_helper{
struct comm_intern_priv_attr_t{
    Comm comm_dup;
    int prev, next;

    comm_intern_priv_attr_t();
    ~comm_intern_priv_attr_t() noexcept;

    bool has_comm_dup() const;
    comm_intern_priv_attr_t &set_comm_dup( const Comm &comm );
    comm_intern_priv_attr_t &del_comm_dup();
    
    static void del_attr_fn( Comm &comm, int keyval, void *attr_val, 
        void *extra_state );

    static const int tag_seqblock = 1;
};
} // namespace _mpi_env_helper
class SeqBlock;


/**
The MPI environment inquiry and management class.
*/
class Env{
public:
    enum: int {
        THREAD_SINGLE       = MPI_THREAD_SINGLE, 
        THREAD_FUNNELED     = MPI_THREAD_FUNNELED,
        THREAD_SERIALIZED   = MPI_THREAD_SERIALIZED,
        THREAD_MULTIPLE     = MPI_THREAD_MULTIPLE };

    Env();
    Env(int &argc, char **&argv );
    Env(int &argc, char **&argv, int required, int &provided );
    Env(const Env &) = delete;
    Env(Env &&) = delete;
    Env & operator=(const Env &) = delete;
    Env & operator=(Env &&) = delete;
    ~Env() noexcept;

    /**
    ``info()`` prints a short (``fmt_cntl=0``) or a verbose (``fmt_cntl=1``) or
    a more verbose (``fmt_cntl = 2``) description of the MPI environment to the 
    stream ``os``.
    Larger ``level`` produces more indents.

    Operator ``<<`` is equivalent to ``info()`` with default ``fmt_cntl`` and
    ``level``.

    The passed stream ``os`` is returned.
    */
    ostream &info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const Env &);
    
    /**
    version(): return the standard version and subversion which the 
    implementation is compliant to. 
    
    library_version(): return a string that represents the library version 
    (i.e., the implementation version).

    These two can be called when MPI is inactive, that is, before the 
    initialization of after finalization of MPI environment. They must be 
    thread-safe.
    */
    static void version( int &version, int &subversion );
    static string library_version();

    /**
    Query information about the library implementation: 
    
    - the upper bound of tag value; 
    - the rank of the host process (if none, returns ``PROC_NULL``);
    - the rank of process that has I/O facilities (possibly my rank; if none, 
      returns ``PROC_NULL``; if any, return ``ANY_SOURCE``);
    - a Boolean value indicating whether the clocks are synchronized; 
    - the name of the host machine.
    */
    int tag_ub() const;
    int host() const;
    int io() const;
    int wtime_is_global() const;
    static string processor_name();

    /**
    Get the global "WORLD" communicator predefined by MPI. New communicators 
    can be created, if necessary, from the predefined ones.

    ``world_detail()`` returns the "WORLD" communicatior, the rank of the 
    current process in it, and the size of it, as a tuple.

    Example
    ----------
    auto [comm, rank, size] = Env::world_detail();
    */
    static Comm world() noexcept;
    static std::tuple<Comm, int, int> world_detail() noexcept;
protected:
    friend class SeqBlock;

    typedef _mpi_env_helper::comm_intern_priv_attr_t _comm_intern_priv_attr_t;
    static int _comm_intern_priv_keyval;

    MPI_Comm _comm;
    int _get_attr( int keyval, int &attrval ) const noexcept;
    void _init_comm_intern_attr();
    void _free_comm_intern_attr();
    void _init_predefined_objects();
    void _free_predefined_objects();
};

inline ostream & operator<<(ostream &os, const Env &env){ 
    return env.info(os);
}

inline Comm Env::world() noexcept { 
    return Comm::world(); 
}

inline std::tuple<Comm, int, int> Env::world_detail() noexcept {
    auto comm = world();
    int rank = comm.rank(), size = comm.size();
    return {comm, rank, size};
}


} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_ENV_H_