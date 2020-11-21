#ifndef _HIPPSIMD_PS256_H_
#define _HIPPSIMD_PS256_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"
namespace HIPP{
namespace SIMD{
#ifdef __AVX__

namespace _ps256_helper{
class Packs8Base{
public:
    typedef float scal_t;
    typedef __m256 vec_t;
    typedef typename TypeCvt<vec_t, -1, 0, 1>::ret vec_hc_t;

    typedef int32_t iscal_t;    
    typedef typename TypeCvt<vec_t, 0, 0, 0>::ret ivec_t;

    typedef __mmask8 mask8_t;

    enum: size_t { 
        NPACK=8, 
        NBIT=256, 
        VECSIZE=sizeof(vec_t), 
        SCALSIZE=sizeof(scal_t) };
};
} // namespace _ps256_helper

template<> class Packed<float, 8>: public _ps256_helper::Packs8Base{
public:
    /**
     * L/S operations
     * load() - load 8 packed single precision float from `mem_addr` into dst.
     * Post-fix can be:
     * u:   not aligned to any.
     * m:   load if the highest bit of element in mask is asseted. Otherwise 
     *      zero out.
     * 1:   broadcast single 32-bit float into dst.
     * 
     * bcast() - load and broad cast. If input with scal_t*, broadcast one
     * float, if with vec_hc_t *, broadcast the vec_hc_t (4 floats).
     * gather() - load from different memory locations.
     * Post-fix can be:
     * m:   load if the highest bit of element in mask is asserted. Otherwise
     *      copy from src.
     * 
     * store() - store 8 packed single precision float into `mem_addr`.
     * Post-fix can be:
     * m:   store if selected by mask. Otherwise not store.
     * u:   not aligned to any.
     * 
     * stream() - non-temporary memory hints.
     */
    static vec_t load( const scal_t *mem_addr ) noexcept;
    static vec_t loadu( const scal_t *mem_addr ) noexcept;
    static vec_t loadm( const scal_t *mem_addr, ivec_t mask ) noexcept;
    static vec_t load1( const scal_t *mem_addr ) noexcept;

    static vec_t bcast( const scal_t *mem_addr ) noexcept;
    static vec_t bcast( const vec_hc_t *mem_addr ) noexcept;
#ifdef __AVX2__
    static vec_t gather( const scal_t *base_addr, 
        ivec_t vindex, const int scale=SCALSIZE ) noexcept;
    static vec_t gatherm( vec_t src, const scal_t *base_addr, 
        ivec_t vindex, vec_t mask, const int scale=SCALSIZE ) noexcept;
#endif
    static void store( scal_t *mem_addr, vec_t a ) noexcept;
    static void storem( scal_t *mem_addr, ivec_t mask, vec_t a ) noexcept;
    static void storeu( scal_t *mem_addr, vec_t a ) noexcept;
    static void stream( scal_t *mem_addr, vec_t a ) noexcept;
#if defined(__AVX512F__) && defined(__AVX512VL__)
    static void scatter(void *base_addr, ivec_t vindex, vec_t a, 
        int scale=SCALSIZE) noexcept;
    static void scatterm(void *base_addr, mask8_t k, ivec_t vindex, vec_t a, 
        int scale=SCALSIZE) noexcept;
#endif

    static ivec_t to_si(vec_t a) noexcept               { return _mm256_castps_si256(a); }
    static vec_t from_si(ivec_t a) noexcept             { return _mm256_castsi256_ps(a); }
    static vec_t from_ivec( ivec_t a ) noexcept;
    static ivec_t to_ivec( vec_t a ) noexcept;
    static ivec_t tot_ivec( vec_t a ) noexcept;
    static scal_t to_scal( vec_t a ) noexcept;

    static int movemask( vec_t a ) noexcept;
    static vec_t movehdup( vec_t a ) noexcept;
    static vec_t moveldup( vec_t a ) noexcept;
    static vec_t set( scal_t e7, scal_t e6, scal_t e5, scal_t e4, 
        scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
#ifdef __AVX2__
    static vec_t set1( vec_hc_t a ) noexcept;        // broadcast a[0], 1 float
#endif
    static vec_t set1( scal_t a ) noexcept;          // broadcast a, 1 float
    static vec_t set() noexcept;                     // zero out all
    static vec_t setzero() noexcept;                 // same as set()
    static vec_t undefined() noexcept;               // undefined value

    static vec_t add( vec_t a, vec_t b ) noexcept;
    static vec_t sub( vec_t a, vec_t b ) noexcept;
    static vec_t mul( vec_t a, vec_t b ) noexcept;
    static vec_t div( vec_t a, vec_t b ) noexcept;
    static vec_t hadd( vec_t a, vec_t b ) noexcept;
    static vec_t hsub( vec_t a, vec_t b ) noexcept;

    static vec_t eq( vec_t a, vec_t b ) noexcept;
    static vec_t neq( vec_t a, vec_t b ) noexcept;
    static vec_t false_( vec_t a, vec_t b ) noexcept;
    static vec_t lt( vec_t a, vec_t b ) noexcept;
    static vec_t le( vec_t a, vec_t b ) noexcept;
    static vec_t gt( vec_t a, vec_t b ) noexcept;
    static vec_t ge( vec_t a, vec_t b ) noexcept;
    static vec_t cmp( vec_t a, vec_t b, const int op ) noexcept;

    static vec_t and_( vec_t a, vec_t b ) noexcept;
    static vec_t andnot( vec_t a, vec_t b ) noexcept;
    static vec_t or_( vec_t a, vec_t b ) noexcept;
    static vec_t xor_( vec_t a, vec_t b ) noexcept;

    static vec_t blend( vec_t a, vec_t b, const int imm8 ) noexcept;
    static vec_t blend( vec_t a, vec_t b, vec_t mask ) noexcept;
   
    static vec_t rcp( vec_t a ) noexcept;
    static vec_t sqrt( vec_t a ) noexcept;
    static vec_t rsqrt( vec_t a ) noexcept;
    static vec_t ceil( vec_t a ) noexcept;
    static vec_t floor( vec_t a ) noexcept;
    static vec_t round( vec_t a, const int rounding ) noexcept;
    static vec_t max( vec_t a, vec_t b ) noexcept;
    static vec_t min( vec_t a, vec_t b ) noexcept;
};



inline Packed<float, 8>::vec_t Packed<float, 8>::load( const scal_t *mem_addr ) noexcept{ 
    return _mm256_load_ps(mem_addr); 
}
inline Packed<float, 8>::vec_t Packed<float, 8>::loadu( const scal_t *mem_addr ) noexcept{
    return _mm256_loadu_ps(mem_addr);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::loadm( const scal_t *mem_addr, ivec_t mask ) noexcept{
    return _mm256_maskload_ps( mem_addr, mask );
}
inline Packed<float, 8>::vec_t Packed<float, 8>::load1( const scal_t *mem_addr ) noexcept{
    return _mm256_broadcast_ss(mem_addr);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::bcast( const vec_hc_t *mem_addr ) noexcept{
    return _mm256_broadcast_ps(mem_addr);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::bcast( const scal_t *mem_addr ) noexcept{
    return load1(mem_addr);
}
#ifdef __AVX2__
inline Packed<float, 8>::vec_t Packed<float, 8>::gather( 
    const scal_t *base_addr, ivec_t vindex, const int scale ) noexcept{
    return _mm256_i32gather_ps(base_addr, vindex, scale);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::gatherm( 
    vec_t src, const scal_t *base_addr, ivec_t vindex, vec_t mask, const int scale ) noexcept{
    return _mm256_mask_i32gather_ps(src, base_addr, vindex, mask, scale);
}
#endif
inline void Packed<float, 8>::store( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_store_ps(mem_addr, a);
}
inline void Packed<float, 8>::storem( scal_t *mem_addr, ivec_t mask, vec_t a ) noexcept{
    _mm256_maskstore_ps(mem_addr, mask, a);
}
inline void Packed<float, 8>::storeu( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_storeu_ps(mem_addr, a);
}
inline void Packed<float, 8>::stream( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_stream_ps(mem_addr, a);
}
#if defined(__AVX512F__) && defined(__AVX512VL__)
inline void Packed<float, 8>::scatter(
    void *base_addr, ivec_t vindex, vec_t a, int scale) noexcept{
    _mm256_i32scatter_ps(base_addr, vindex, a, scale);
}
inline void Packed<float, 8>::scatterm(
    void *base_addr, mask8_t k, ivec_t vindex, vec_t a, int scale) noexcept{
    _mm256_mask_i32scatter_ps(base_addr, k, vindex, a, scale);
}
#endif
inline Packed<float, 8>::vec_t Packed<float, 8>::from_ivec( ivec_t a ) noexcept{
    return _mm256_cvtepi32_ps( a );
}
inline Packed<float, 8>::ivec_t Packed<float, 8>::to_ivec( vec_t a ) noexcept{
    return _mm256_cvtps_epi32(a);
}
inline Packed<float, 8>::ivec_t Packed<float, 8>::tot_ivec( vec_t a ) noexcept{
    return _mm256_cvttps_epi32(a);
}
inline Packed<float, 8>::scal_t Packed<float, 8>::to_scal( vec_t a ) noexcept{
    return _mm256_cvtss_f32(a);
}
inline int Packed<float, 8>::movemask( vec_t a ) noexcept{
    return _mm256_movemask_ps(a);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::movehdup( vec_t a ) noexcept{
    return _mm256_movehdup_ps(a);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::moveldup( vec_t a ) noexcept{
    return _mm256_moveldup_ps(a);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::set( scal_t e7, scal_t e6, scal_t e5, scal_t e4, scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept{
    return _mm256_set_ps( e7, e6, e5, e4, e3, e2, e1, e0 );
}
#ifdef __AVX2__
inline Packed<float, 8>::vec_t Packed<float, 8>::set1( vec_hc_t a ) noexcept{
    return _mm256_broadcastss_ps( a );
}
#endif
inline Packed<float, 8>::vec_t Packed<float, 8>::set1( scal_t a ) noexcept{
    return _mm256_set1_ps(a);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::set() noexcept{
    return _mm256_setzero_ps();
}
inline Packed<float, 8>::vec_t Packed<float, 8>::setzero() noexcept{
    return set();
}
inline Packed<float, 8>::vec_t Packed<float, 8>::undefined() noexcept{ 
    return _mm256_undefined_ps(); 
}
inline Packed<float, 8>::vec_t Packed<float, 8>::add( vec_t a, vec_t b ) noexcept{
    return _mm256_add_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::sub( vec_t a, vec_t b ) noexcept{
    return _mm256_sub_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::mul( vec_t a, vec_t b ) noexcept{
    return _mm256_mul_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::div( vec_t a, vec_t b ) noexcept{
    return _mm256_div_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::hadd( vec_t a, vec_t b ) noexcept{
    return _mm256_hadd_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::hsub( vec_t a, vec_t b ) noexcept{
    return _mm256_hsub_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::eq( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_ps(a,b,(int)Mode::CMP_EQ);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::neq( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_ps(a,b,(int)Mode::CMP_NEQ);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::false_( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_ps(a,b,(int)Mode::CMP_FALSE);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::lt( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_ps(a,b,(int)Mode::CMP_LT);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::le( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_ps(a,b,(int)Mode::CMP_LE);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::gt( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_ps(a,b,(int)Mode::CMP_GT);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::ge( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_ps(a,b,(int)Mode::CMP_GE);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::cmp( vec_t a, vec_t b, const int op ) noexcept{
    return _mm256_cmp_ps(a,b,op);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::and_( vec_t a, vec_t b ) noexcept{
    return _mm256_and_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::andnot( vec_t a, vec_t b ) noexcept{
    return _mm256_andnot_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::or_( vec_t a, vec_t b ) noexcept{
    return _mm256_or_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::xor_( vec_t a, vec_t b ) noexcept{
    return _mm256_xor_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::blend( vec_t a, vec_t b, 
    const int imm8 ) noexcept{
    return _mm256_blend_ps(a,b,imm8);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::blend( vec_t a, vec_t b, vec_t mask ) noexcept{
    return _mm256_blendv_ps(a,b,mask);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::rcp( vec_t a ) noexcept{
    return _mm256_rcp_ps(a);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::sqrt( vec_t a ) noexcept{
    return _mm256_sqrt_ps(a);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::rsqrt( vec_t a ) noexcept{
    return _mm256_rsqrt_ps(a);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::ceil( vec_t a ) noexcept{
    return _mm256_ceil_ps(a);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::floor( vec_t a ) noexcept{
    return _mm256_floor_ps(a);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::round( vec_t a, const int rounding ) noexcept{
    return _mm256_round_ps(a, rounding);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::max( vec_t a, vec_t b ) noexcept{
    return _mm256_max_ps(a,b);
}
inline Packed<float, 8>::vec_t Packed<float, 8>::min( vec_t a, vec_t b ) noexcept{
    return _mm256_min_ps(a,b);
}
#endif  // __AVX__

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PS256_H_