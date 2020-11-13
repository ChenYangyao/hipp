#ifndef _HIPPSIMD_PU64_256_H_
#define _HIPPSIMD_PU64_256_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"
namespace HIPP {
namespace SIMD {

namespace _pu64_256_helper {
class PackedBase{
public:
    typedef uint64_t scal_t;
    typedef __m256i vec_t;
    enum: size_t {
        NPACK=4, NBIT=256, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };

    typedef typename TypeCvt<__m128i, 3, 1, 0>::ret vec_dc_h4p_t;   // 8 x 16
    typedef typename TypeCvt<__m128i, 2, 2, 0>::ret vec_hp_t;       // 4 x 32
};
} // namespace _pu64_256_helper

template<> class Packed<uint64_t, 4>: public _pu64_256_helper::PackedBase {
public:
    static vec_t mul_from_lo(vec_t a, vec_t b) noexcept         { return _mm256_mul_epu32(a, b); }


};

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PU64_256_H_