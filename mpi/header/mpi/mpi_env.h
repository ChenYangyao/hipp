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

class Env{
public:
    Env();
    Env(int &argc, char **&argv );
    Env(int &argc, char **&argv, int required, int &provided );
    ~Env() noexcept;

    ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Env &);
    
    /**
     * can be called when MPI is inactive, that is, before MPI_Init() or after 
     * MPI_Finalize(). Must be thread-safe.
     */
    static void version( int &version, int &subversion );
    static string library_version();

    /**
     * inquire runtime environment
     */
    int tag_ub() const;
    int host() const;
    int io() const;
    int wtime_is_global() const;

    /**
     * inquire processor information
     */
    static string processor_name();

    static Comm world() noexcept { return Comm::world(); }
protected:
    MPI_Comm _comm;
    int _get_attr( int keyval, int &attrval ) const noexcept;
};

inline ostream & operator<<( ostream &os, const Env &env){ 
    return env.info( os );
}



} // namespace MPI
} // namespace HIPP

#endif	//_HIPPMPI_MPI_ENV_H_