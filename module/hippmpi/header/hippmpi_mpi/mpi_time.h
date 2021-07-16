#ifndef _MPI_TIME_H_
#define _MPI_TIME_H_
#include "mpi_base.h"
namespace HIPP::MPI {
    
/**
 * Get the wall-time in seconds. The zero-point is not changed in a process, 
 * but different nodes may have different zero-point.
 */
inline
double wtime() {
    return MPI_Wtime();
}

/**
 * Get the resolution of wtime() in seconds, i.e., the time between successive 
 * clock ticks.
 */
inline
double wtick() {
    return MPI_Wtick();
}

} // namespace HIPP::MPI
#endif	//_MPI_TIME_H_