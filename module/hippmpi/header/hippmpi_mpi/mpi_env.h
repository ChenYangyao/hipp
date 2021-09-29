/**
create: Yangyao CHEN, 2020/01/21
    [write   ] Env - the MPI environment inquiry and management class.
*/ 

#ifndef _HIPPMPI_MPI_ENV_H_
#define _HIPPMPI_MPI_ENV_H_
#include "mpi_base.h"
#include "mpi_error.h"
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
        THREAD_SINGLE = MPI_THREAD_SINGLE, 
        THREAD_FUNNELED = MPI_THREAD_FUNNELED,
        THREAD_SERIALIZED = MPI_THREAD_SERIALIZED,
        THREAD_MULTIPLE = MPI_THREAD_MULTIPLE };

    Env();
    Env(int &argc, char **&argv );
    Env(int &argc, char **&argv, int required, int &provided );
    Env(const Env &) = delete;
    Env(Env &&) = delete;
    Env & operator=(const Env &) = delete;
    Env & operator=(Env &&) = delete;
    ~Env() noexcept;

    /**
    info() - print MPI environment details
    @fmt_cntl: 0/1 for inline/block info, 2 for block library version.
    
    operator(os, env) is equivalent to env.info(os).
    */
    ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Env &);
    
    /**
    Can be called when MPI is inactive, that is, before MPI_Init() or after 
    MPI_Finalize(). Must be thread-safe.
    */
    static void version( int &version, int &subversion );
    static string library_version();

    /**
    Inquire runtime environment
    */
    int tag_ub() const;
    int host() const;
    int io() const;
    int wtime_is_global() const;

    /**
    Inquire processor information
    */
    static string processor_name();

    static Comm world() noexcept { return Comm::world(); }
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

inline ostream & operator<<( ostream &os, const Env &env){ 
    return env.info( os );
}


} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_ENV_H_