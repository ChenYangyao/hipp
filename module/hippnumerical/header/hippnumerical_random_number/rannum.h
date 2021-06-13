#ifndef _HIPPNUMERICAL_RANNUM_H_
#define _HIPPNUMERICAL_RANNUM_H_

#include "rannum_gaussian.h"
#include "rannum_uniform_real.h"

namespace HIPP::NUMERICAL {

inline thread_local static 
GaussianRandomNumber<> randn;

inline thread_local static 
UniformRealRandomNumber<> rand;

} // namespace HIPP::NUMERICAL

#endif	//_HIPPNUMERICAL_RANNUM_H_
