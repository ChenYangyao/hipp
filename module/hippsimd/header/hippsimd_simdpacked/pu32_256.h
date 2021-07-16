#ifndef _HIPPSIMD_PU32_256_H_
#define _HIPPSIMD_PU32_256_H_
#include "packedbase.h"
#include "../hippsimd_simdopcode/opcode.h"
namespace HIPP {
namespace SIMD {

namespace _pu32_256_helper {
class PackedBase{
public:
    typedef uint32_t scal_t;
    typedef __m256i vec_t;
    enum: size_t {
        NPACK=8, NBIT=256, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };

    typedef typename TypeCvt<__m128i, 3, 1, 0>::ret vec_hp_t;   // 8 x 16
};
} // namespace _pu32_256_helper

template<> class Packed<uint32_t, 8>: public _pu32_256_helper::PackedBase {
public:
    
    static vec_t from_hp(vec_hp_t a) noexcept           { return _mm256_cvtepu16_epi32(a); }
    static vec_t max(vec_t a, vec_t b) noexcept { return _mm256_max_epu32(a, b); }
    static vec_t min(vec_t a, vec_t b) noexcept { return _mm256_min_epu32(a, b); }


};

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PU32_256_H_