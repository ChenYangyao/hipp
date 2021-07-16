#ifndef _HIPPSIMD_PACKEDBASE_H_
#define _HIPPSIMD_PACKEDBASE_H_
#include <x86intrin.h>
#include "type_trait.h"
#include <cstdint>
namespace HIPP{
namespace SIMD{
template<typename ScaleT, size_t NPack> class Packed {};

typedef std::size_t size_t;

using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;



} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PACKEDBASE_H_