/**
create: Yangyao CHEN, 2020/01/15
Base file to be included in all files in the MPI module.
*/ 

#ifndef _HIPPMPI_MPI_BASE_H_
#define _HIPPMPI_MPI_BASE_H_

#include <mpi.h>
#include <hippcntl.h>

namespace HIPP::MPI {

typedef MPI_Aint aint_t;
typedef MPI_Offset offset_t;
typedef MPI_Count count_t;
typedef std::size_t size_t;
typedef std::ptrdiff_t ptrdiff_t;

const int 
    UNDEFINED = MPI_UNDEFINED,
    ROOT = MPI_ROOT,
    PROC_NULL = MPI_PROC_NULL,
    ANY_SOURCE = MPI_ANY_SOURCE,
    ANY_TAG = MPI_ANY_TAG,
    
    ERR_IN_STATUS = MPI_ERR_IN_STATUS,

    KEYVAL_INVALID = MPI_KEYVAL_INVALID,
    
    /**
    Returns of object comparisons.
    */
    IDENT = MPI_IDENT,
    SIMILAR = MPI_SIMILAR,
    UNEQUAL = MPI_UNEQUAL,
    
    /**
    Arguments to distributed datatype constructors.
    */
    DISTRIBUTE_BLOCK     = MPI_DISTRIBUTE_BLOCK,
    DISTRIBUTE_CYCLIC    = MPI_DISTRIBUTE_CYCLIC,
    DISTRIBUTE_NONE      = MPI_DISTRIBUTE_NONE,
    DISTRIBUTE_DFLT_DARG = MPI_DISTRIBUTE_DFLT_DARG,

    /**
    Store order of high-dim arrays.
     */
    ORDER_C       = MPI_ORDER_C,
    ORDER_FORTRAN = MPI_ORDER_FORTRAN,

    /**
    Topology status.
    */
    GRAPH      = MPI_GRAPH, 
    CART       = MPI_CART,
    DIST_GRAPH = MPI_DIST_GRAPH;

/**
Special weight values.
*/
int * const UNWEIGHTED    = MPI_UNWEIGHTED,
    * const WEIGHTS_EMPTY = MPI_WEIGHTS_EMPTY;

/**
Special memory buffer location specifier
*/
void* const BOTTOM   = MPI_BOTTOM, 
    * const IN_PLACE = MPI_IN_PLACE;

const size_t 
    BSEND_OVERHEAD = MPI_BSEND_OVERHEAD;

} // namespace HIPP::MPI

#endif	//_HIPPMPI_MPI_BASE_H_