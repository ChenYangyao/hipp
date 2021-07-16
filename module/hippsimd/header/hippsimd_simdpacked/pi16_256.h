#ifndef _HIPPSIMD_PI16_256_H_
#define _HIPPSIMD_PI16_256_H_
#include "packedbase.h"
#include "../hippsimd_simdopcode/opcode.h"
namespace HIPP {
namespace SIMD {
#ifdef __AVX2__

namespace _pi16_256_helper {
class PackBase{
public:
    typedef int16_t scal_t;
    typedef __m256i vec_t;
    enum: size_t {
        NPACK=16, NBIT=256, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };

    typedef typename TypeCvt<__m128i, 1, 3, 0>::ret vec_h8c_d4p_t;      //2 x 64
    typedef typename TypeCvt<__m128i, 3, 1, 0>::ret vec_hc_t;           //8 x 16
    typedef typename TypeCvt<__m128i, 4, 0, 0>::ret vec_hp_t;           //16 x 6
    typedef typename TypeCvt<__m256i, 3, 2, 0>::ret vec_hc_dp_t;        //8 x 32
    
};
} // namespace _pi16_256_helper


template<> class Packed<int16_t, 16>: public _pi16_256_helper::PackBase {
public:
    static vec_t from_hc( vec_hp_t a) noexcept                          { return _mm256_cvtepi8_epi16(a); }
    static vec_t from_u_hp(vec_hp_t a) noexcept                         { return _mm256_cvtepu8_epi16(a); }

    static vec_t set(scal_t e15, scal_t e14, scal_t e13, scal_t e12, scal_t e11, scal_t e10, scal_t e9, scal_t e8, scal_t e7, scal_t e6, scal_t e5, scal_t e4, scal_t e3, scal_t e2, scal_t e1, scal_t e0) noexcept { return _mm256_set_epi16(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0); }
    static vec_t setr(scal_t e15, scal_t e14, scal_t e13, scal_t e12, scal_t e11, scal_t e10, scal_t e9, scal_t e8, scal_t e7, scal_t e6, scal_t e5, scal_t e4, scal_t e3, scal_t e2, scal_t e1, scal_t e0) noexcept { return _mm256_setr_epi16(e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0); }
    static vec_t set1(scal_t a) noexcept                                { return _mm256_set1_epi16(a); }
    static vec_t set1(vec_hc_t a) noexcept                              { return _mm256_broadcastw_epi16(a); }
    static vec_t insert(vec_t a, scal_t i, const int index)             { return _mm256_insert_epi16(a, i, index); }
    static vec_t packs(vec_hc_dp_t a, vec_hc_dp_t b) noexcept           { return _mm256_packs_epi32(a, b); }
    static vec_t packus(vec_hc_dp_t a, vec_hc_dp_t b) noexcept          { return _mm256_packus_epi32(a, b); }
    static vec_t unpackhi(vec_t a, vec_t b) noexcept                    { return _mm256_unpackhi_epi16(a,b); }
    static vec_t unpacklo(vec_t a, vec_t b) noexcept                    { return _mm256_unpacklo_epi16(a,b); }

    static vec_t add(vec_t a, vec_t b) noexcept                         { return _mm256_add_epi16(a, b); }
    static vec_t adds(vec_t a, vec_t b) noexcept                        { return _mm256_adds_epi16(a, b); }
    static vec_t hadd(vec_t a, vec_t b) noexcept                        { return _mm256_hadd_epi16(a, b); }
    static vec_t hadds(vec_t a, vec_t b) noexcept                       { return _mm256_hadds_epi16(a, b); }
    static vec_t hsub(vec_t a, vec_t b) noexcept                        { return _mm256_hsub_epi16(a, b); }
    static vec_t hsubs(vec_t a, vec_t b) noexcept                       { return _mm256_hsubs_epi16(a, b); }
    static vec_t madd(vec_t a, vec_t b) noexcept                        { _mm256_madd_epi16(a, b); }

    static vec_t sub(vec_t a, vec_t b) noexcept                         { return _mm256_sub_epi16(a, b); }
    static vec_t subs(vec_t a, vec_t b) noexcept                        { return _mm256_subs_epi16(a, b); }
    static vec_t mul_as_hi(vec_t a, vec_t b) noexcept                   { return _mm256_mulhi_epi16(a, b); }
    static vec_t mul_as_hrs(vec_t a, vec_t b) noexcept                  { return _mm256_mulhrs_epi16(a, b); }
    static vec_t mul_as_lo(vec_t a, vec_t b) noexcept                   { return _mm256_mullo_epi16(a, b); }
    static vec_t sign(vec_t a, vec_t b) noexcept                        { return _mm256_sign_epi16(a, b); }

    static vec_t eq(vec_t a, vec_t b) noexcept                          { return _mm256_cmpeq_epi16(a, b); }
    static vec_t gt(vec_t a, vec_t b) noexcept                          { return _mm256_cmpgt_epi16(a, b); }

    static vec_t sl(vec_t a, vec_h8c_d4p_t count) noexcept              { return _mm256_sll_epi16(a, count); }
    static vec_t sli(vec_t a, const int imm8) noexcept                  { return _mm256_slli_epi16(a, imm8); }
    static vec_t sr(vec_t a, vec_h8c_d4p_t count) noexcept              { return _mm256_srl_epi16(a, count); }
    static vec_t sri(vec_t a, const int imm8) noexcept                  { return _mm256_srli_epi16(a, imm8); }

    static vec_t sra(vec_t a, vec_h8c_d4p_t count) noexcept             { return _mm256_sra_epi16(a, count); }
    static vec_t srai(vec_t a, const int imm8) noexcept                 { return _mm256_srai_epi16(a, imm8); }

    static vec_t blend(vec_t a, vec_t b, const int imm8) noexcept       { return _mm256_blend_epi16(a, b, imm8); }
    
    static vec_t abs(vec_t a) noexcept                                  { return _mm256_abs_epi16(a); }
    static vec_t max(vec_t a, vec_t b) noexcept                         { return _mm256_max_epi16(a, b); }
    static vec_t min(vec_t a, vec_t b) noexcept                         { return _mm256_min_epi16(a, b); }
};

#endif

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PI16_256_H_