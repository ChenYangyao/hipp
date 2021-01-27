#ifndef _HIPPSIMD_PI64_128_H_
#define _HIPPSIMD_PI64_128_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"
namespace HIPP::SIMD {
#ifdef __SSE__

namespace _pi64_128_helper {
class PackBase {
public:
    typedef long long scal_t;
    typedef __m128i vec_t;
    enum: size_t {
        NPACK=2, NBIT=128, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };
};
} // namespace _pi64_128_helper

template<> class Packed<long long, 2>: public _pi64_128_helper::PackBase {
public:

#ifdef __SSE2__
    static vec_t set1(scal_t a) noexcept                                        { return _mm_set1_epi64x(a); }

    static vec_t add(vec_t a, vec_t b) noexcept                                 { return _mm_add_epi64(a, b); }
    static vec_t sub(vec_t a, vec_t b) noexcept                                 { return _mm_sub_epi64(a, b); }

    static vec_t sl(vec_t a, vec_t count) noexcept                              { return _mm_sll_epi64(a, count); }
    static vec_t sli(vec_t a, int imm8) noexcept                                { return _mm_slli_epi64(a, imm8); }

    static vec_t sr(vec_t a, vec_t count) noexcept                              { return _mm_srl_epi64(a, count); }
    static vec_t sri(vec_t a, int imm8) noexcept                                { return _mm_srli_epi64(a, imm8); }
#endif //__SSE2__

#ifdef __SSE4_1__
    static vec_t eq(vec_t a, vec_t b) noexcept                                  { return _mm_cmpeq_epi64(a, b); }
#endif //__SSE4_1__
#ifdef __SSE4_2__
    static vec_t gt(vec_t a, vec_t b) noexcept                                  { return _mm_cmpgt_epi64(a, b); }
#endif //__SSE4_2__

};

#endif //__SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_PI64_128_H_