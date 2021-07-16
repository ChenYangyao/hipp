#ifndef _HIPPMPI_H_
#define _HIPPMPI_H_

#include "hippmpi_mpi/mpi_intermediate.h"
#include "hippmpi_mpi/mpi_high.h"

#include "hippmpi_mpe/mpe_sync.h"
#include "hippmpi_mpe/mpe_masterslave.h"
#include "hippmpi_mpe/mpe_symm.h"
#include "hippmpi_mpe/mpe_work_decomp.h"

#if __has_include(<hippmpi_config.h>)
#include <hippmpi_config.h>
#else
#warning No hippmpi config file found
#endif

#ifndef _HIPPMPI_CONFIG_NO_MPPROF
#include "hippmpi_mpprof/mpprof.h"
#endif


#endif	//_HIPPMPI_H_