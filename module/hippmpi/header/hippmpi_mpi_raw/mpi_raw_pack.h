#ifndef _HIPPMPI_MPI_RAW_PACK_H_
#define _HIPPMPI_MPI_RAW_PACK_H_
#include "mpi_base.h"
#include "mpi_error.h"
namespace HIPP::MPI {

/**
Intermediate functions of MPI pack and unpack operations.
*/
class _Pack {
public:
    /**
    Data pack/unpack functions. The ways of packing depend on the machine
    and the communicator. The packing unit may have extra header. 
    */
    static void pack(const void *inbuf, int incount, MPI_Datatype dtype, 
        void *outbuf, int outsize, int &position, MPI_Comm comm);
    static void unpack(const void *inbuf, int insize, int &position, 
        void *outbuf, int outcount, MPI_Datatype dtype, MPI_Comm comm);
    static int size(int incount, MPI_Datatype dtype, MPI_Comm comm);
    
    /** 
    Data pack/unpack functions using standard, machine independent, no-header 
    format. 
    In MPI-3, datarep = "external32" is the only option.
    */
    static void pack_external(const char *datarep,  
        const void *inbuf, int incount, MPI_Datatype dtype, 
        void *outbuf, aint_t outsize, aint_t &position);
    static void unpack_external(const char *datarep, 
        const void *inbuf, aint_t insize, aint_t &position, 
        void *outbuf, int outcount, MPI_Datatype dtype);
    static aint_t external_size(const char *datarep, 
        int incount, MPI_Datatype dtype);
};

inline void _Pack::pack(const void *inbuf, int incount, MPI_Datatype dtype, 
    void *outbuf, int outsize, int &position, MPI_Comm comm) 
{
    ErrMPI::check(MPI_Pack(inbuf, incount, dtype, outbuf, outsize, 
        &position, comm));
}

inline void _Pack::unpack(const void *inbuf, int insize, int &position, 
    void *outbuf, int outcount, MPI_Datatype dtype, MPI_Comm comm)
{
    ErrMPI::check(MPI_Unpack(inbuf, insize, &position, outbuf, 
        outcount, dtype, comm));
}

inline int _Pack::size(int incount, MPI_Datatype dtype, MPI_Comm comm) {
    int outsize;
    ErrMPI::check(MPI_Pack_size(incount, dtype, comm, &outsize));
    return outsize;
}

inline void _Pack::pack_external(const char *datarep,  
    const void *inbuf, int incount, MPI_Datatype dtype, 
    void *outbuf, aint_t outsize, aint_t &position)
{
    ErrMPI::check(MPI_Pack_external(datarep, inbuf, incount, dtype, 
        outbuf, outsize, &position));
}

inline void _Pack::unpack_external(const char *datarep, 
    const void *inbuf, aint_t insize, aint_t &position, 
    void *outbuf, int outcount, MPI_Datatype dtype)
{
    ErrMPI::check(MPI_Unpack_external(datarep, inbuf, insize, &position, 
        outbuf, outcount, dtype));
}

inline aint_t _Pack::external_size(const char *datarep, 
    int incount, MPI_Datatype dtype)
{
    aint_t outsize;
    ErrMPI::check(MPI_Pack_external_size(datarep, incount, dtype, &outsize));
    return outsize;
}

} // namespace HIPP::MPI
#endif	//_HIPPMPI_MPI_RAW_PACK_H_