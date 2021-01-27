#ifndef _HIPPSIMD_PI8_256_H_
#define _HIPPSIMD_PI8_256_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"
namespace HIPP {
namespace SIMD {
#ifdef __AVX2__

namespace _pi8_256_helper {
class PackBase{
public:
    typedef int8_t scal_t;
    typedef char char_t;
    typedef __m256i vec_t;
    enum: size_t {
        NPACK=32, NBIT=256, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };

    typedef typename TypeCvt<__m128i, 1, 3, 0>::ret vec_h16c_d8p_t; //2 x 64
    typedef typename TypeCvt<__m128i, 4, 0, 0>::ret vec_hc_t;       //16 x 8
    typedef typename TypeCvt<__m256i, 4, 1, 0>::ret vec_hc_dp_t;    //16 x 16
    
};
} // namespace _pi8_256_helper


template<> class Packed<int8_t, 32>: public _pi8_256_helper::PackBase {
public:
    static vec_t load(const vec_t *mem_addr) noexcept               { return _mm256_load_si256(mem_addr); }
    static vec_t loadu(const vec_t *mem_addr) noexcept              { return _mm256_loadu_si256(mem_addr); }
    static vec_t loaddqu(const vec_t *mem_addr) noexcept            { return _mm256_lddqu_si256(mem_addr); }
    static vec_t loadstream(const vec_t *mem_addr) noexcept         { return _mm256_stream_load_si256(mem_addr); }
    
    static void store(vec_t *mem_addr, vec_t a) noexcept            { return _mm256_store_si256(mem_addr, a); }
    static void storeu(vec_t *mem_addr, vec_t a) noexcept           { return _mm256_storeu_si256(mem_addr, a); }
    static void stream(vec_t *mem_addr, vec_t a) noexcept           { return _mm256_stream_si256(mem_addr, a); }
    
    static vec_t set( char_t e31, char_t e30, char_t e29, char_t e28, char_t e27, char_t e26, char_t e25, char_t e24, char_t e23, char_t e22, char_t e21, char_t e20, char_t e19, char_t e18, char_t e17, char_t e16, 
        char_t e15, char_t e14, char_t e13, char_t e12, char_t e11, char_t e10, char_t e9, char_t e8, char_t e7, char_t e6, char_t e5, char_t e4, char_t e3, char_t e2, char_t e1, char_t e0) noexcept 
        { return _mm256_set_epi8(e31, e30, e29, e28, e27, e26, e25, e24, e23, e22, e21, e20, e19, e18, e17, e16, e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0); }
    static vec_t setr( char_t e31, char_t e30, char_t e29, char_t e28, char_t e27, char_t e26, char_t e25, char_t e24, char_t e23, char_t e22, char_t e21, char_t e20, char_t e19, char_t e18, char_t e17, char_t e16, 
        char_t e15, char_t e14, char_t e13, char_t e12, char_t e11, char_t e10, char_t e9, char_t e8, char_t e7, char_t e6, char_t e5, char_t e4, char_t e3, char_t e2, char_t e1, char_t e0) noexcept 
        { return _mm256_setr_epi8(e31, e30, e29, e28, e27, e26, e25, e24, e23, e22, e21, e20, e19, e18, e17, e16, e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0); }
    static vec_t set( vec_hc_t hi, vec_hc_t lo ) noexcept           { return _mm256_set_m128i(hi, lo); }
    static vec_t setr( vec_hc_t lo, vec_hc_t hi ) noexcept          { return _mm256_set_m128i(lo, hi); }
    static vec_t set1(char_t a) noexcept                            { return _mm256_set1_epi8(a); }
    static vec_t set1(vec_hc_t a) noexcept                          { return bcast(a); }
    static vec_t set( ) noexcept                                    { return setzero();  }
    static vec_t setzero( ) noexcept                                { return _mm256_setzero_si256();  }
    static vec_t undefined_si( ) noexcept                           { return _mm256_undefined_si256();  }
    static vec_t bcast(vec_hc_t a) noexcept                         { return _mm256_broadcastb_epi8(a); }
    static vec_t bcast_hc(vec_hc_t a) noexcept                      { return _mm256_broadcastsi128_si256(a); }
    static vec_t from_si_hc(vec_hc_t a) noexcept                    { return _mm256_castsi128_si256(a); }
    static vec_hc_t to_si_hc(vec_t a) noexcept                      { return _mm256_castsi256_si128(a); }
    static int movemask(vec_t a) noexcept                           { return _mm256_movemask_epi8(a); }
    static vec_t packs(vec_hc_dp_t a, vec_hc_dp_t b) noexcept       { return _mm256_packs_epi16(a, b); }
    static vec_t packus(vec_hc_dp_t a, vec_hc_dp_t b) noexcept      { return _mm256_packus_epi16(a, b); }
    static vec_t unpackhi(vec_t a, vec_t b) noexcept                { return _mm256_unpackhi_epi8(a,b); }
    static vec_t unpacklo(vec_t a, vec_t b) noexcept                { return _mm256_unpacklo_epi8(a,b); }

    static vec_t insert(vec_t a, scal_t i, const int index)         { return _mm256_insert_epi8(a, i, index); }
    static vec_t insertf(vec_t a, vec_hc_t b, int imm8)             { return _mm256_insertf128_si256(a, b, imm8); }
    static vec_t inserti(vec_t a, vec_hc_t b, const int imm8)       { return _mm256_inserti128_si256(a, b, imm8); }

    static vec_t add(vec_t a, vec_t b) noexcept                     { return _mm256_add_epi8(a, b); }
    static vec_t adds(vec_t a, vec_t b) noexcept                    { return _mm256_adds_epi8(a, b); }
    static vec_t madd(vec_t a, vec_t b) noexcept                    { return _mm256_maddubs_epi16(a, b); }
    static vec_t sign(vec_t a, vec_t b) noexcept                    { return _mm256_sign_epi8(a, b); }
    static vec_t sub(vec_t a, vec_t b) noexcept                     { return _mm256_sub_epi8(a, b); }
    static vec_t subs(vec_t a, vec_t b) noexcept                    { return _mm256_subs_epi8(a, b); }
    
    static vec_t eq(vec_t a, vec_t b) noexcept                      { return _mm256_cmpeq_epi8(a, b); }
    static vec_t gt(vec_t a, vec_t b) noexcept                      { return _mm256_cmpgt_epi8(a, b); }
    static int testz(vec_t a, vec_t b) noexcept                     { return _mm256_testz_si256(a, b); }

    static vec_t and_(vec_t a, vec_t b) noexcept                    { return _mm256_and_si256(a, b); }
    static vec_t or_(vec_t a, vec_t b) noexcept                     { return _mm256_or_si256(a, b); }
    static vec_t xor_(vec_t a, vec_t b) noexcept                    { return _mm256_xor_si256(a, b); }
    static vec_t andnot(vec_t a, vec_t b) noexcept                  { return _mm256_andnot_si256(a, b); }

    static vec_t sli_hc(vec_t a, const int imm8) noexcept           { return _mm256_bslli_epi128(a, imm8); }
    static vec_t sri_hc(vec_t a, const int imm8) noexcept           { return _mm256_bsrli_epi128(a, imm8); }
    static vec_t sli_si(vec_t a, const int imm8) noexcept           { return _mm256_slli_si256(a, imm8); }  // imm8 bytes
    static vec_t sri_si(vec_t a, const int imm8) noexcept           { return _mm256_srli_si256(a, imm8); }  // imm8 bytes, zero in

    static vec_t blend(vec_t a, vec_t b, vec_t mask) noexcept      { return _mm256_blendv_epi8(a, b, mask); }

    static vec_t abs(vec_t a) noexcept                              { return _mm256_abs_epi8(a); }
    static vec_t max(vec_t a, vec_t b) noexcept                     { return _mm256_max_epi8(a, b); }
    static vec_t min(vec_t a, vec_t b) noexcept                     { return _mm256_min_epi8(a, b); }

    static vec_t alignr(vec_t a, vec_t b, const int imm8) noexcept  { return _mm256_alignr_epi8(a, b, imm8); }

protected:

};

#endif

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PI8_256_H_