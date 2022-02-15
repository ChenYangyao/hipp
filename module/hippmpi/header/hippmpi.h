#ifndef _HIPPMPI_H_
#define _HIPPMPI_H_

#if __has_include(<hipp_config.h>)
#include <hipp_config.h>
#else 
#warning NO global configuration of HIPP found
#endif

#include "hippmpi_mpi_raw/mpi_raw.h"
#include "hippmpi_mpi/mpi_high.h"

#include "hippmpi_mpe/mpe_sync.h"
#include "hippmpi_mpe/mpe_masterslave.h"
#include "hippmpi_mpe/mpe_symm.h"
#include "hippmpi_mpe/mpe_work_decomp.h"

#ifdef HIPPIO_OFF 
#define _HIPPMPI_CONFIG_NO_MPPROF
#else
#include "hippmpi_mpprof/mpprof.h"
#endif

#endif	//_HIPPMPI_H_