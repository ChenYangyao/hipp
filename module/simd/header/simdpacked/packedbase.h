#ifndef _HIPPSIMD_PACKEDBASE_H_
#define _HIPPSIMD_PACKEDBASE_H_
#include <x86intrin.h>
#include "type_trait.h"
namespace HIPP{
namespace SIMD{
template<typename ScaleT, size_t NPack> class Packed {};

typedef std::size_t size_t;

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PACKEDBASE_H_