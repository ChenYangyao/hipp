#ifndef _HIPPSIMD_PI32_256_H_
#define _HIPPSIMD_PI32_256_H_
#include "packedbase.h"
#include "immintrin.h"
#include "../simdopcode/opcode.h"
namespace HIPP {
namespace SIMD {
#ifdef __AVX2__

namespace _pi32_256_helper {
class PackBase{
public:
    typedef int32_t scal_t;
    typedef __m256i vec_t;
    enum: size_t {
        NPACK=8, NBIT=256, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };

    typedef typename TypeCvt<__m128i, 1, 3, 0>::ret vec_h4c_dp_t;   // 2 x 64
    typedef typename TypeCvt<__m256i, 2, 3, 0>::ret vec_hc_dp_t;    // 4 x 64
    typedef typename TypeCvt<__m128i, 2, 2, 0>::ret vec_hc_t;       // 4 x 32
    typedef typename TypeCvt<__m128i, 3, 1, 0>::ret vec_hp_t;       // 8 x 16
    typedef typename TypeCvt<__m128i, 4, 0, 0>::ret vec_dc_h4p_t;   // 16 x 8

};
} // namespace _pi32_256_helper


template<> class Packed<int32_t, 8>: public _pi32_256_helper::PackBase {
public:
    static vec_t loadm(const scal_t *base_addr, vec_t mask) noexcept        { return _mm256_maskload_epi32(base_addr, mask); }
    static vec_t gather(const scal_t *base_addr, vec_t vindex, const int scale) noexcept                                { return _mm256_i32gather_epi32(base_addr, vindex, scale); }
    static vec_t gatherm(vec_t src, const scal_t *base_addr, vec_t vindex, vec_t mask, const int scale) noexcept        { return _mm256_mask_i32gather_epi32(src, base_addr, vindex, mask, scale); }

    static void storem(scal_t *mem_addr, vec_t mask, vec_t a) noexcept      { _mm256_maskstore_epi32(mem_addr, mask, a); }

    static vec_t from_hp(vec_hp_t a) noexcept                               { return _mm256_cvtepi16_epi32(a); }
    static vec_t from_h4p(vec_dc_h4p_t a) noexcept                          { return _mm256_cvtepi8_epi32(a); }
    static vec_t from_u_dc_h4p(vec_hp_t a) noexcept                         { return _mm256_cvtepu16_epi32(a); }    

    static vec_t set(scal_t e7, scal_t e6, scal_t e5, scal_t e4, scal_t e3, scal_t e2, scal_t e1, scal_t e0) noexcept { return _mm256_set_epi32(e7, e6, e5, e4, e3, e2, e1, e0); }
    static vec_t setr(scal_t e7, scal_t e6, scal_t e5, scal_t e4, scal_t e3, scal_t e2, scal_t e1, scal_t e0) noexcept { return _mm256_setr_epi32(e7, e6, e5, e4, e3, e2, e1, e0); }
    static vec_t set1(scal_t a) noexcept                                    { return _mm256_set1_epi32(a); }
    static vec_t set1(vec_hc_t a) noexcept                                  { return _mm256_broadcastd_epi32(a); }
    static vec_t insert(vec_t a, scal_t i, const int index)                 { return _mm256_insert_epi32(a, i, index); }
    static vec_t unpackhi(vec_t a, vec_t b) noexcept                        { return _mm256_unpackhi_epi32(a,b); }
    static vec_t unpacklo(vec_t a, vec_t b) noexcept                        { return _mm256_unpacklo_epi32(a,b); }

    static vec_t add(vec_t a, vec_t b) noexcept                             { return _mm256_add_epi32(a, b); }
    static vec_t hadd(vec_t a, vec_t b) noexcept                            { return _mm256_hadd_epi32(a, b); }
    static vec_t hsub(vec_t a, vec_t b) noexcept                            { return _mm256_hsub_epi32(a, b); }
    static vec_t mul_as_lo(vec_t a, vec_t b) noexcept                       { return _mm256_mullo_epi32(a, b); }
    static vec_t sign(vec_t a, vec_t b) noexcept                            { return _mm256_sign_epi32(a, b); }
    static vec_t sub(vec_t a, vec_t b) noexcept                             { return _mm256_sub_epi32(a, b); }

    static vec_t eq(vec_t a, vec_t b) noexcept                              { return _mm256_cmpeq_epi32(a, b); }
    static vec_t gt(vec_t a, vec_t b) noexcept                              { return _mm256_cmpgt_epi32(a, b); }

    static vec_t sl(vec_t a, vec_h4c_dp_t count) noexcept                   { return _mm256_sll_epi32(a, count); }
    static vec_t sl(vec_t a, vec_t count) noexcept                          { return _mm256_sllv_epi32(a, count); }
    static vec_t sli(vec_t a, const int imm8) noexcept                      { return _mm256_slli_epi32(a, imm8); }
    static vec_t sr(vec_t a, vec_h4c_dp_t count) noexcept                   { return _mm256_srl_epi32(a, count); }
    static vec_t sr(vec_t a, vec_t count) noexcept                          { return _mm256_srlv_epi32(a, count); }
    static vec_t sri(vec_t a, const int imm8) noexcept                      { return _mm256_srli_epi32(a, imm8); }

    static vec_t sra(vec_t a, vec_h4c_dp_t count) noexcept                  { return _mm256_sra_epi32(a, count); }
    static vec_t sra(vec_t a, vec_t count) noexcept                         { return _mm256_srav_epi32(a, count); }
    static vec_t srai(vec_t a, const int imm8) noexcept                     { return _mm256_srai_epi32(a, imm8); }

    static vec_t blend(vec_t a, vec_t b, const int imm8) noexcept           { return _mm256_blend_epi32(a, b, imm8); }

    static vec_t abs(vec_t a) noexcept                                      { return _mm256_abs_epi32(a); }
    static vec_t max(vec_t a, vec_t b) noexcept                             { return _mm256_max_epi32(a, b); }
    static vec_t min(vec_t a, vec_t b) noexcept                             { return _mm256_min_epi32(a, b); }
};

#endif

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PI32_256_H_