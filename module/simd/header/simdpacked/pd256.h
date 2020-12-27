#ifndef _HIPPSIMD_PD256_H_
#define _HIPPSIMD_PD256_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"
namespace HIPP{
namespace SIMD{

#ifdef __AVX__

namespace _pd256_helper{
class Packd4Base{
public:
    typedef double scal_t;
    typedef float scal_hp_t;
    typedef __m256d vec_t;
    typedef typename TypeCvt<vec_t, -1, 0, 1>::ret vec_hc_t;
    typedef typename TypeCvt<vec_t, 0, -1, 1>::ret vec_hp_t;

    typedef long long iscal_t;
    typedef int32_t iscal_hp_t;
    typedef typename TypeCvt<vec_t, 0, 0, 0>::ret ivec_t;
    typedef typename TypeCvt<vec_t, 0, -1, 0>::ret ivec_hp_t;    
    
    typedef __mmask8 mask8_t;
    enum: size_t { NPACK=4, 
        NBIT=256, 
        VECSIZE=sizeof(vec_t), 
        SCALSIZE=sizeof(scal_t) };
};
} // namespace _pd256_helper

template<> 
class Packed<double, 4>: public _pd256_helper::Packd4Base{
public:
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
    static vec_t gather_idxhp( const scal_t *base_addr, 
        ivec_hp_t vindex, const int scale=SCALSIZE ) noexcept;
    static vec_t gatherm_idxhp( vec_t src, const scal_t *base_addr, 
        ivec_hp_t vindex, vec_t mask, const int scale=SCALSIZE ) noexcept;
#endif
    static void store( scal_t *mem_addr, vec_t a ) noexcept;
    static void storem( scal_t *mem_addr, ivec_t mask, vec_t a ) noexcept;
    static void storeu( scal_t *mem_addr, vec_t a ) noexcept;
    static void stream( scal_t *mem_addr, vec_t a ) noexcept;
#if defined(__AVX512F__) && defined(__AVX512VL__)
    /**
     * scatter 4 doubles 'a' into memory, with address specified by `base_addr`
     * and offset specified by vindex scaled by scale.
     * @scale: either 1, 2, 4, or 8.
     * 
     * In the 'm' version, if a bit in mask `k` is not set, then the double is
     * not stored.
     * You may use 32-bit or 64-bit index, the former with a `idxhp` postfix.
     */
    static void scatter(void *base_addr, ivec_t vindex, vec_t a, 
        const int scale=SCALSIZE) noexcept;
    static void scatterm(void *base_addr, mask8_t k, ivec_t vindex, vec_t a, 
        const int scale=SCALSIZE) noexcept;
    static void scatter_idxhp(void *base_addr, ivec_hp_t vindex, vec_t a, 
        const int scale=SCALSIZE) noexcept;
    static void scatterm_idxhp(void *base_addr, mask8_t k, ivec_hp_t vindex, 
        vec_t a, const int scale=SCALSIZE) noexcept;
#endif
    /**
     * The to_ivec_xx() use NEAR mode for conversion. While the tot_ivec_xx()
     * does the truncation.
     */
    static ivec_t to_si( vec_t a) noexcept                                      { return _mm256_castpd_si256(a); }
    static vec_t from_si(ivec_t a) noexcept                                     { return _mm256_castsi256_pd(a); }
    static vec_t from_ivec_hp( ivec_hp_t a ) noexcept;
    static ivec_hp_t to_ivec_hp( vec_t a ) noexcept                             { return _mm256_cvtpd_epi32(a); }
    static ivec_hp_t tot_ivec_hp( vec_t a ) noexcept                            { return _mm256_cvttpd_epi32(a); }
    static vec_t from_vec_hp( vec_hp_t a ) noexcept;
    static vec_hp_t to_vec_hp( vec_t a ) noexcept                               { return _mm256_cvtpd_ps(a); }
    static vec_hc_t to_vec_hc( vec_t a ) noexcept                               { return _mm256_castpd256_pd128(a); }
#ifdef __AVX2__
    static scal_t to_scal( vec_t a ) noexcept;
#endif
    static vec_hc_t extract_hc(vec_t a, const int imm8) noexcept                { return _mm256_extractf128_pd(a, imm8); }
    static vec_t unpackhi(vec_t a, vec_t b) noexcept                            { return _mm256_unpackhi_pd(a, b); }
    static vec_t unpacklo(vec_t a, vec_t b) noexcept                            { return _mm256_unpacklo_pd(a, b); }

    static int movemask( vec_t a ) noexcept;
    static vec_t movedup( vec_t a ) noexcept;

    static vec_t set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    static vec_t set1( scal_t a ) noexcept;

#ifdef __AVX2__
    static vec_t set1( vec_hc_t a ) noexcept;
#endif
    static vec_t set() noexcept;
    static vec_t setzero() noexcept;
    static vec_t undefined() noexcept;

    /**
     * in the hadd, 
     * c[0] = a[0] + a[1], c[1] = b[0] + b[1], c[2] = a[2]+a[3], ...
     * in the hsub, similar
     * c[0] = a[0] - a[1], c[1] = b[0] - b[1], ...
     */
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
    static int testz( vec_t a, vec_t b ) noexcept                               { return _mm256_testz_pd(a, b); }

    static vec_t and_( vec_t a, vec_t b ) noexcept;
    static vec_t andnot( vec_t a, vec_t b ) noexcept;  // (NOT a) AND b, bitwise
    static vec_t or_( vec_t a, vec_t b ) noexcept;      
    static vec_t xor_( vec_t a, vec_t b ) noexcept;

    /**
     * In the imm8 version, use only lowest 4 bits as mask. If true, take from 
     * b, otherwise take from a.
     * In the mask version, similar but use the highest bit in each 64 bits.
     */
    static vec_t blend( vec_t a, vec_t b, const int imm8 ) noexcept;
    static vec_t blend( vec_t a, vec_t b, vec_t mask ) noexcept;
   
    static vec_t sqrt( vec_t a ) noexcept;
    static vec_t ceil( vec_t a ) noexcept;
    static vec_t floor( vec_t a ) noexcept;
    static vec_t round( vec_t a, const int rounding ) noexcept;
    static vec_t max( vec_t a, vec_t b ) noexcept;  // c[i] = MIN(a[i], b[i])
    static vec_t min( vec_t a, vec_t b ) noexcept;  // c[i] = MAX(a[i], b[i])
};

inline Packed<double,4>::vec_t Packed<double,4>::load( const scal_t *mem_addr ) noexcept{ 
    return _mm256_load_pd(mem_addr); 
}
inline Packed<double,4>::vec_t Packed<double,4>::loadu( const scal_t *mem_addr ) noexcept{
    return _mm256_loadu_pd(mem_addr);
}
inline Packed<double,4>::vec_t Packed<double,4>::loadm( const scal_t *mem_addr, ivec_t mask ) noexcept{
    return _mm256_maskload_pd( mem_addr, mask );
}
inline Packed<double,4>::vec_t Packed<double,4>::load1( const scal_t *mem_addr ) noexcept{
    return bcast(mem_addr);
}
inline Packed<double,4>::vec_t Packed<double,4>::bcast( const scal_t *mem_addr ) noexcept{
    return _mm256_broadcast_sd(mem_addr);
}
inline Packed<double,4>::vec_t Packed<double,4>::bcast( const vec_hc_t *mem_addr ) noexcept{
    return _mm256_broadcast_pd(mem_addr);
}
#ifdef __AVX2__
inline Packed<double,4>::vec_t 
Packed<double,4>::gather( const scal_t *base_addr, 
    ivec_t vindex, const int scale ) noexcept{
    return _mm256_i64gather_pd(base_addr, vindex, scale);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::gatherm( vec_t src, const scal_t *base_addr, 
    ivec_t vindex, vec_t mask, const int scale ) noexcept{
    return _mm256_mask_i64gather_pd(src, base_addr, vindex, mask, scale);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::gather_idxhp( const scal_t *base_addr, 
    ivec_hp_t vindex, const int scale ) noexcept{
    return _mm256_i32gather_pd(base_addr, vindex, scale);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::gatherm_idxhp( vec_t src, const scal_t *base_addr, 
    ivec_hp_t vindex, vec_t mask, const int scale ) noexcept{
    return _mm256_mask_i32gather_pd(src, base_addr, vindex, mask, scale);
}
#endif

inline void Packed<double,4>::store( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_store_pd(mem_addr, a);
}
inline void Packed<double,4>::storem( 
    scal_t *mem_addr, ivec_t mask, vec_t a ) noexcept{
    _mm256_maskstore_pd(mem_addr, mask, a);
}
inline void Packed<double,4>::storeu( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_storeu_pd(mem_addr, a);
}
inline void Packed<double,4>::stream( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_stream_pd(mem_addr, a);
}
#if defined(__AVX512F__) && defined(__AVX512VL__)
inline void Packed<double, 4>::scatter(void *base_addr, ivec_t vindex, 
    vec_t a, const int scale) noexcept{
    _mm256_i64scatter_pd(base_addr, vindex, a, scale);
}
inline void Packed<double, 4>::scatterm(void *base_addr, mask8_t k, 
    ivec_t vindex, vec_t a, const int scale) noexcept{
    _mm256_mask_i64scatter_pd( base_addr, k, vindex, a, scale );
}
inline void Packed<double, 4>::scatter_idxhp(void *base_addr, ivec_hp_t vindex, 
    vec_t a, const int scale) noexcept{
    _mm256_i32scatter_pd(base_addr, vindex, a, scale);
}
inline void Packed<double, 4>::scatterm_idxhp(void *base_addr, mask8_t k, 
    ivec_hp_t vindex, vec_t a, const int scale) noexcept{
    _mm256_mask_i32scatter_pd(base_addr, k, vindex, a, scale);
}
#endif
inline Packed<double,4>::vec_t Packed<double,4>::from_ivec_hp( ivec_hp_t a ) noexcept{
    return _mm256_cvtepi32_pd( a );
}
inline Packed<double,4>::vec_t Packed<double,4>::from_vec_hp( vec_hp_t a ) noexcept{
    return _mm256_cvtps_pd( a );
}
#ifdef __AVX2__
inline Packed<double,4>::scal_t Packed<double,4>::to_scal( vec_t a ) noexcept{
    return _mm256_cvtsd_f64( a );
}
#endif
inline int Packed<double,4>::movemask( vec_t a ) noexcept{
    return _mm256_movemask_pd(a);
}
inline Packed<double,4>::vec_t Packed<double,4>::movedup( vec_t a ) noexcept{
    return _mm256_movedup_pd(a);
}
inline Packed<double,4>::vec_t Packed<double,4>::set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept{
    return _mm256_set_pd( e3, e2, e1, e0 );
}
inline Packed<double,4>::vec_t Packed<double,4>::set1( scal_t a ) noexcept{
    return _mm256_set1_pd(a);
}
#ifdef __AVX2__
inline Packed<double,4>::vec_t Packed<double,4>::set1( vec_hc_t a ) noexcept{
    return _mm256_broadcastsd_pd( a );
}
#endif
inline Packed<double,4>::vec_t Packed<double,4>::set() noexcept{
    return _mm256_setzero_pd();
}
inline Packed<double,4>::vec_t Packed<double,4>::setzero() noexcept{
    return set();
}
inline Packed<double,4>::vec_t Packed<double,4>::undefined() noexcept{ 
    return _mm256_undefined_pd(); 
}
inline Packed<double,4>::vec_t Packed<double,4>::add( vec_t a, vec_t b ) noexcept{
    return _mm256_add_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::sub( vec_t a, vec_t b ) noexcept{
    return _mm256_sub_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::mul( vec_t a, vec_t b ) noexcept{
    return _mm256_mul_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::div( vec_t a, vec_t b ) noexcept{
    return _mm256_div_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::hadd( vec_t a, vec_t b ) noexcept{
    return _mm256_hadd_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::hsub( vec_t a, vec_t b ) noexcept{
    return _mm256_hsub_pd(a,b);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::eq( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_EQ);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::neq( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_NEQ);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::false_( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_FALSE);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::lt( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_LT);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::le( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_LE);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::gt( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_GT);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::ge( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_GE);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::cmp( vec_t a, vec_t b, const int op ) noexcept{
    return _mm256_cmp_pd(a,b,op);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::and_( vec_t a, vec_t b ) noexcept{
    return _mm256_and_pd(a,b);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::andnot( vec_t a, vec_t b ) noexcept{
    return _mm256_andnot_pd(a,b);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::or_( vec_t a, vec_t b ) noexcept{
    return _mm256_or_pd(a,b);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::xor_( vec_t a, vec_t b ) noexcept{
    return _mm256_xor_pd(a,b);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::blend( vec_t a, vec_t b, 
    const int imm8 ) noexcept{
    return _mm256_blend_pd(a,b,imm8);
}
inline Packed<double,4>::vec_t Packed<double,4>::blend( vec_t a, vec_t b, 
    vec_t mask ) noexcept{
    return _mm256_blendv_pd(a,b,mask);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::sqrt( vec_t a ) noexcept{
    return _mm256_sqrt_pd(a);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::ceil( vec_t a ) noexcept{
    return _mm256_ceil_pd(a);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::floor( vec_t a ) noexcept{
    return _mm256_floor_pd(a);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::round( vec_t a, const int rounding ) noexcept{
    return _mm256_round_pd(a, rounding);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::max( vec_t a, vec_t b ) noexcept{
    return _mm256_max_pd(a,b);
}
inline Packed<double,4>::vec_t 
Packed<double,4>::min( vec_t a, vec_t b ) noexcept{
    return _mm256_min_pd(a,b);
}

#endif  // __AVX__

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PD256_H_