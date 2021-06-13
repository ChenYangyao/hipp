#ifndef _HIPPSIMD_PI16_128_H_
#define _HIPPSIMD_PI16_128_H_
#include "packedbase.h"
#include "../hippsimd_simdopcode/opcode.h"
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

    static vec_t sl(vec_t a, vec_t count) noexcept                              { return _mm_sll_epi16(a, count); }
    static vec_t sli(vec_t a, int imm8) noexcept                                { return _mm_slli_epi16(a, imm8); }

    static vec_t sr(vec_t a, vec_t count) noexcept                              { return _mm_srl_epi16(a, count); }
    static vec_t sri(vec_t a, int imm8) noexcept                                { return _mm_srli_epi16(a, imm8); }
    static vec_t sra(vec_t a, vec_t count) noexcept                             { return _mm_sra_epi16(a, count); }
    static vec_t srai(vec_t a, int imm8) noexcept                               { return _mm_srai_epi16(a, imm8); }
#endif //__SSE2__
};

#endif //__SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_PI16_128_H_