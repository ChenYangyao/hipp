/**
 * creat: Yangyao CHEN, 2020/01/15
 * Base file to be included in all files in the MPI module.
 */ 

#ifndef _HIPPMPI_MPI_BASE_H_
#define _HIPPMPI_MPI_BASE_H_

#include <mpi.h>
#include <hippcntl.h>

namespace HIPP{
namespace MPI{

const int 
    UNDEFINED = MPI_UNDEFINED,
    ERR_IN_STATUS = MPI_ERR_IN_STATUS,
    
    /**
     * returns of object comparisons
     */
    IDENT = MPI_IDENT,
    SIMILAR = MPI_SIMILAR,
    UNEQUAL = MPI_UNEQUAL,
    
    /**
     * args of distributed datatype constructor
     */
    DISTRIBUTE_BLOCK = MPI_DISTRIBUTE_BLOCK,
    DISTRIBUTE_CYCLIC = MPI_DISTRIBUTE_CYCLIC,
    DISTRIBUTE_NONE = MPI_DISTRIBUTE_NONE,
    DISTRIBUTE_DFLT_DARG = MPI_DISTRIBUTE_DFLT_DARG,

    /**
     * store order of high-dim arrays
     */
    ORDER_C = MPI_ORDER_C,
    ORDER_FORTRAN = MPI_ORDER_FORTRAN;

/**
 * special memory buffer location specifier
 */
void* const BOTTOM = MPI_BOTTOM, 
    * const IN_PLACE = MPI_IN_PLACE;

typedef MPI_Aint aint_t;
typedef MPI_Offset offset_t;
typedef std::size_t size_t;

} // namespace MPI
} // namespace HIPP


#endif	//_HIPPMPI_MPI_BASE_H_