/**
create: Yangyao CHEN, 2021/08/12
    [write   ] The probability distribution component.
*/

#ifndef _HIPPNUMERICAL_RANDIST_H_
#define _HIPPNUMERICAL_RANDIST_H_

/** 
The probability distribution classes provide (optional) static members:
- pdf() is the probability density function.
- cdf_lower() and cdf_upper() corresponds to P(x) and Q(x) in GSL, 
  satisfying:
    - P(x) + Q(x) = 1.
    - 0 <= P(x) <= 1; 0 <= Q(x) <= 1.
- "inv" functions are their inverse.
*/

#include "randist_gaussian.h"
#include "randist_gaussian_tail.h"
#include "randist_bivariate_gaussian.h"
#include "randist_lognormal.h"

#include "randist_exp_pow.h"
#include "randist_exponential.h"
#include "randist_laplace.h"

#include "randist_cauchy.h"
#include "randist_rayleigh.h"
#include "randist_rayleigh_tail.h"

#include "randist_uniform_real.h"

#endif	//_HIPPNUMERICAL_RANDIST_H_