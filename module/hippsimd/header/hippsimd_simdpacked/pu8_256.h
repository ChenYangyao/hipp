#ifndef _HIPPSIMD_PU8_256_H_
#define _HIPPSIMD_PU8_256_H_
#include "packedbase.h"
#include "../hippsimd_simdopcode/opcode.h"
namespace HIPP {
namespace SIMD {

namespace _pu8_256_helper {
class PackedBase{
public:
    typedef uint8_t scal_t;
    typedef __m256i vec_t;
    enum: size_t {
        NPACK=32, NBIT=256, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };

    
};
} // namespace _pu8_256_helper

template<> class Packed<uint8_t, 32>: public _pu8_256_helper::PackedBase {
public:
    static vec_t adds(vec_t a, vec_t b) noexcept { return _mm256_adds_epu8(a, b); }
    static vec_t subs(vec_t a, vec_t b) noexcept { return _mm256_subs_epu8(a, b); }

    static vec_t avg(vec_t a, vec_t b) noexcept { return _mm256_avg_epu8(a, b); }
    static vec_t max(vec_t a, vec_t b) noexcept { return _mm256_max_epu8(a, b); }
    static vec_t min(vec_t a, vec_t b) noexcept { return _mm256_min_epu8(a, b); }


};

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PU8_256_H_