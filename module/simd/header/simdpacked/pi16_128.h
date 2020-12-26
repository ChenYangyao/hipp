#ifndef _HIPPSIMD_PI16_128_H_
#define _HIPPSIMD_PI16_128_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"
namespace HIPP::SIMD {
#ifdef __SSE__

namespace _pi16_128_helper {
class PackBase {
public:
    typedef int16_t scal_t;
    typedef __m128i vec_t;
    enum: size_t {
        NPACK=8, NBIT=128, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };
};
} // namespace _pi16_128_helper

template<> class Packed<int16_t, 8>: public _pi16_128_helper::PackBase {
public:
#ifdef __SSE2__
    static vec_t set1(scal_t a) noexcept                                        { return _mm_set1_epi16(a); }

    static vec_t eq(vec_t a, vec_t b) noexcept                                  { return _mm_cmpeq_epi16(a, b); }
    static vec_t gt(vec_t a, vec_t b) noexcept                                  { return _mm_cmpgt_epi16(a, b); }
    static vec_t lt(vec_t a, vec_t b) noexcept                                  { return _mm_cmplt_epi16(a, b); }

    static vec_t add(vec_t a, vec_t b) noexcept                                 { return _mm_add_epi16(a, b); }
    static vec_t sub(vec_t a, vec_t b) noexcept                                 { return _mm_sub_epi16(a, b); }
    static vec_t mul_as_lo(vec_t a, vec_t b) noexcept   { return _mm_mullo_epi16(a, b); }
#endif //__SSE2__
};

#endif //__SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_PI16_128_H_