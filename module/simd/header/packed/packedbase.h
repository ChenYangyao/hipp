#ifndef _HIPPSIMD_PACKEDBASE_H_
#define _HIPPSIMD_PACKEDBASE_H_
#include <x86intrin.h>
#include "type_trait.h"
namespace HIPP{
namespace SIMD{
template<typename ScaleT, size_t NPack> class Packed {};
} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PACKEDBASE_H_