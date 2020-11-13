#ifndef _HIPPSIMD_PU16_256_H_
#define _HIPPSIMD_PU16_256_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"
namespace HIPP {
namespace SIMD {

namespace _pu16_256_helper {
class PackedBase{
public:
    typedef uint16_t scal_t;
    typedef __m256i vec_t;
    enum: size_t {
        NPACK=16, NBIT=256, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };
};
} // namespace _pu16_256_helper

template<> class Packed<uint16_t, 16>: public _pu16_256_helper::PackedBase {
public:
    static vec_t adds(vec_t a, vec_t b) noexcept { return _mm256_adds_epu16(a, b); }
    static vec_t subs(vec_t a, vec_t b) noexcept { return _mm256_subs_epu16(a, b); }
    static vec_t mul_as_hi(vec_t a, vec_t b) noexcept         { return _mm256_mulhi_epu16(a, b); }

    static vec_t avg(vec_t a, vec_t b) noexcept { return _mm256_avg_epu16(a, b); }
    static vec_t max(vec_t a, vec_t b) noexcept { return _mm256_max_epu16(a, b); }
    static vec_t min(vec_t a, vec_t b) noexcept { return _mm256_min_epu16(a, b); }


};

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PU16_256_H_