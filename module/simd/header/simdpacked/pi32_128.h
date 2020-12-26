#ifndef _HIPPSIMD_PI32_128_H_
#define _HIPPSIMD_PI32_128_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"
namespace HIPP::SIMD {
#ifdef __SSE__

namespace _pi32_128_helper {
class PackBase {
public:
    typedef int32_t scal_t;
    typedef __m128i vec_t;
    enum: size_t {
        NPACK=4, NBIT=128, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };
};
} // namespace _pi32_128_helper

template<> class Packed<int32_t, 4>: public _pi32_128_helper::PackBase {
public:
#ifdef __SSE2__
    static vec_t set1(scal_t a) noexcept                                        { return _mm_set1_epi32(a); }

    static vec_t eq(vec_t a, vec_t b) noexcept                                  { return _mm_cmpeq_epi32(a, b); }
    static vec_t gt(vec_t a, vec_t b) noexcept                                  { return _mm_cmpgt_epi32(a, b); }
    static vec_t lt(vec_t a, vec_t b) noexcept                                  { return _mm_cmplt_epi32(a, b); }
    
    static vec_t add(vec_t a, vec_t b) noexcept                                 { return _mm_add_epi32(a, b); }
    static vec_t sub(vec_t a, vec_t b) noexcept                                 { return _mm_sub_epi32(a, b); }
#endif //__SSE2__

#ifdef __SSE4_1__
    static vec_t mul_as_lo(vec_t a, vec_t b) noexcept                           { return _mm_mullo_epi32(a, b); }
#endif //__SSE4_1__

};

#endif //__SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_PI32_128_H_