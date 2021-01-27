#ifndef _HIPPSIMD_PI8_128_H_
#define _HIPPSIMD_PI8_128_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"
namespace HIPP::SIMD {
#ifdef __SSE__

namespace _pi8_128_helper {
class PackBase {
public:
    typedef int8_t scal_t;
    typedef char char_t;
    typedef __m128i vec_t;
    enum: size_t {
        NPACK=16, NBIT=128, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };
};
} // namespace _pi8_128_helper

template<> class Packed<int8_t, 16>: public _pi8_128_helper::PackBase {
public:
    static vec_t loadu_si64(const void *mem_addr) noexcept                      { return _mm_loadu_si64(mem_addr); }
#ifdef __SSE2__
    static vec_t load(const vec_t *mem_addr) noexcept                           { return _mm_load_si128(mem_addr); }
    static vec_t loadu(const vec_t *mem_addr) noexcept                          { return _mm_loadu_si128(mem_addr); }

    static void store(vec_t *mem_addr, vec_t a) noexcept                        { return _mm_store_si128(mem_addr, a); }
    static void storeu(vec_t *mem_addr, vec_t a) noexcept                       { return _mm_storeu_si128(mem_addr, a); }

    static vec_t set() noexcept                                                 { return setzero(); }
    static vec_t setzero() noexcept                                             { return _mm_setzero_si128(); }
    static vec_t set1(char_t a) noexcept                                        { return _mm_set1_epi8(a); }

    static vec_t eq(vec_t a, vec_t b) noexcept                                  { return _mm_cmpeq_epi8(a, b); }
    static vec_t gt(vec_t a, vec_t b) noexcept                                  { return _mm_cmpgt_epi8(a, b); }
    static vec_t lt(vec_t a, vec_t b) noexcept                                  { return _mm_cmplt_epi8(a, b); }

    static vec_t and_(vec_t a, vec_t b) noexcept                                { return _mm_and_si128(a, b); }
    static vec_t or_(vec_t a, vec_t b) noexcept                                 { return _mm_or_si128(a, b); }
    static vec_t xor_(vec_t a, vec_t b) noexcept                                { return _mm_xor_si128(a, b); }
    static vec_t andnot(vec_t a, vec_t b) noexcept                              { return _mm_andnot_si128(a, b); }

    static vec_t add(vec_t a, vec_t b) noexcept                                 { return _mm_add_epi8(a, b); }
    static vec_t sub(vec_t a, vec_t b) noexcept                                 { return _mm_sub_epi8(a, b); }

    static vec_t sli_si(vec_t a, int imm8) noexcept                             { return _mm_slli_si128(a, imm8); }
    static vec_t sri_si(vec_t a, int imm8) noexcept                             { return _mm_srli_si128(a, imm8); }
#endif //__SSE2__


};

#endif //__SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_PI8_128_H_