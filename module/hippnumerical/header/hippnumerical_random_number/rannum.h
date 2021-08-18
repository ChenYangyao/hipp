/**
create: Yangyao CHEN, 2021/08/12
    [write   ] The random number generator component.
*/

#ifndef _HIPPNUMERICAL_RANNUM_H_
#define _HIPPNUMERICAL_RANNUM_H_

#include "rannum_uniform_real.h"
#include "rannum_uniform_int.h"

#include "rannum_gaussian.h"

#include "rannum_poisson.h"

namespace HIPP::NUMERICAL {

/**
The predefined random number generators.

They are thread-local variable, share the same thread-local engine 
`global_random_engine`.

Note that calling `seed()` of one affects the others because of the sharing of 
engine.
*/
inline thread_local static 
GaussianRandomNumber<> randn;               /* standard normal dist */

inline thread_local static 
UniformRealRandomNumber<> rand;             /* uniform real dist in [0,1] */

inline thread_local static 
UniformRealRandomNumber<float> randfloat;   /* uniform real dist in [0,1], using `float` */

inline thread_local static 
UniformIntRandomNumber<char> randchar;      /* uniform char in [0, max char] */

inline thread_local static 
UniformIntRandomNumber<short> randshort;    /* uniform short in [0, max short] */

inline thread_local static 
UniformIntRandomNumber<> randint;           /* uniform int in [0, max int] */

inline thread_local static 
UniformIntRandomNumber<unsigned int> randuint;
                                            /* uniform unsigned int in [0, max unsigned int] */

inline thread_local static 
UniformIntRandomNumber<long> randlong;      /* uniform long in [0, max long] */

inline thread_local static 
UniformIntRandomNumber<long long> randlonglong;
                                            /* uniform long long in [0, max long long] */

inline thread_local static 
UniformIntRandomNumber<std::size_t> randsize;
                                            /* uniform size_t in [0, max size_t] */

} // namespace HIPP::NUMERICAL

#endif	//_HIPPNUMERICAL_RANNUM_H_
