/**
create: Yangyao CHEN, 2022/01/28
The only header to be included when using the HIPP NUMERICAL module.
*/

#ifndef _HIPPNUMERICAL_H_
#define _HIPPNUMERICAL_H_

#include "hippnumerical_gsl_util/gsl_util.h"
#include "hippnumerical_function/function.h"
#include "hippnumerical_random_number/randist.h"
#include "hippnumerical_random_number/rannum.h"

#include "hippnumerical_linalg/linalg_sarray.h"
#include "hippnumerical_linalg/linalg_darray.h"

/*
Sub-modules for geometry-related computation.
*/
#include "hippnumerical_geometry/geometry.h"
#include "hippnumerical_kdsearch/kdsearch.h"

#endif	//_HIPPNUMERICAL_H_