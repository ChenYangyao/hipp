#ifndef _HIPPSIMD_PD256_H_
#define _HIPPSIMD_PD256_H_
#include "packedbase.h"
#include "../opcode/opcode.h"
namespace HIPP{
namespace SIMD{
#ifdef __AVX__
template<> class Packed<double, 4>{
public:
    typedef double scal_t;
    typedef __m256d vec_t;

    typedef __m128d hvec_t;
    typedef __m128 svec_t;

    typedef __m256i epi_t;
    typedef __m128i epi32v_t;
    
    enum: size_t { NPACK=4, NBIT=256, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };
    /**
     * LOAD instrinsics
     * maskload     - elements are zeroed out when the highest bit of corresponding element in mask
     *                is not set.
     * bcasts       - broadcast the low double-precision element (64-bit) from `a` into dst.
     * maskgather   - elements are gather from src if the highest bit of corresponding element in 
     *                mask is not set.
     * maskstore    - elements are save only if the highest bit of corresponding element in mask
     *                is not set.
     */
    static vec_t load( const scal_t *mem_addr ) noexcept;
    static vec_t loadu( const scal_t *mem_addr ) noexcept;
    static vec_t maskload( const scal_t *mem_addr, epi_t mask ) noexcept;
    static vec_t bcast( const scal_t *mem_addr ) noexcept;
    static vec_t bcast( const hvec_t *mem_addr ) noexcept;
#ifdef __AVX2__
    static vec_t bcasts( hvec_t a ) noexcept;
    static vec_t gather( const scal_t *base_addr, epi_t vindex, const int scale=8 ) noexcept;
    static vec_t maskgather( vec_t src, const scal_t *base_addr, epi_t vindex, vec_t mask, const int scale=8 ) noexcept;
#endif
    static void store( scal_t *mem_addr, vec_t a ) noexcept;
    static void maskstore( scal_t *mem_addr, epi_t mask, vec_t a ) noexcept;
    static void storeu( scal_t *mem_addr, vec_t a ) noexcept;
    static void stream( scal_t *mem_addr, vec_t a ) noexcept;

    static vec_t from_epi( epi32v_t a ) noexcept;
    static epi32v_t cvt_epi( vec_t a ) noexcept;
    static epi32v_t cvtt_epi( vec_t a ) noexcept;
    static vec_t from_ps( svec_t a ) noexcept;
    static svec_t cvt_ps( vec_t a ) noexcept;
#ifdef __AVX2__
    static scal_t cvt_scal( vec_t a ) noexcept;
#endif
    static int movemask( vec_t a ) noexcept;
    static vec_t movehdup( vec_t a ) noexcept;
    static vec_t moveldup( vec_t a ) noexcept;
    static vec_t set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    static vec_t set( scal_t a ) noexcept;
    static vec_t set() noexcept;
    static vec_t setzero() noexcept;
    static vec_t undefined() noexcept;

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

    static vec_t blend( vec_t a, vec_t b, int mask ) noexcept;
    static vec_t blend( vec_t a, vec_t b, vec_t mask ) noexcept;
   
    static vec_t sqrt( vec_t a ) noexcept;
    static vec_t ceil( vec_t a ) noexcept;
    static vec_t floor( vec_t a ) noexcept;
    static vec_t round( vec_t a, const int rounding ) noexcept;
    static vec_t max( vec_t a, vec_t b ) noexcept;
    static vec_t min( vec_t a, vec_t b ) noexcept;
};

inline Packed<double,4>::vec_t Packed<double,4>::load( const scal_t *mem_addr ) noexcept{ 
    return _mm256_load_pd(mem_addr); 
}
inline Packed<double,4>::vec_t Packed<double,4>::loadu( const scal_t *mem_addr ) noexcept{
    return _mm256_loadu_pd(mem_addr);
}
inline Packed<double,4>::vec_t Packed<double,4>::maskload( const scal_t *mem_addr, epi_t mask ) noexcept{
    return _mm256_maskload_pd( mem_addr, mask );
}
inline Packed<double,4>::vec_t Packed<double,4>::bcast( const scal_t *mem_addr ) noexcept{
    return _mm256_broadcast_sd(mem_addr);
}
inline Packed<double,4>::vec_t Packed<double,4>::bcast( const hvec_t *mem_addr ) noexcept{
    return _mm256_broadcast_pd(mem_addr);
}
#ifdef __AVX2__
inline Packed<double,4>::vec_t Packed<double,4>::bcasts( hvec_t a ) noexcept{
    return _mm256_broadcastsd_pd( a );
}
#endif
#ifdef __AVX2__
inline Packed<double,4>::vec_t Packed<double,4>::gather( const scal_t *base_addr, epi_t vindex, const int scale ) noexcept{
    return _mm256_i64gather_pd(base_addr, vindex, scale);
}
#endif
#ifdef __AVX2__
inline Packed<double,4>::vec_t Packed<double,4>::maskgather( vec_t src, const scal_t *base_addr, epi_t vindex, vec_t mask, const int scale ) noexcept{
    return _mm256_mask_i64gather_pd(src, base_addr, vindex, mask, scale);
}
#endif

inline void Packed<double,4>::store( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_store_pd(mem_addr, a);
}
inline void Packed<double,4>::maskstore( scal_t *mem_addr, epi_t mask, vec_t a ) noexcept{
    _mm256_maskstore_pd(mem_addr, mask, a);
}
inline void Packed<double,4>::storeu( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_storeu_pd(mem_addr, a);
}
inline void Packed<double,4>::stream( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_stream_pd(mem_addr, a);
}
inline Packed<double,4>::vec_t Packed<double,4>::from_epi( epi32v_t a ) noexcept{
    return _mm256_cvtepi32_pd( a );
}
inline Packed<double,4>::epi32v_t Packed<double,4>::cvt_epi( vec_t a ) noexcept{
    return _mm256_cvtpd_epi32(a);
}
inline Packed<double,4>::epi32v_t Packed<double,4>::cvtt_epi( vec_t a ) noexcept{
    return _mm256_cvttpd_epi32(a);
}
inline Packed<double,4>::vec_t Packed<double,4>::from_ps( svec_t a ) noexcept{
    return _mm256_cvtps_pd( a );
}
inline Packed<double,4>::svec_t Packed<double,4>::cvt_ps( vec_t a ) noexcept{
    return _mm256_cvtpd_ps(a);
}
#ifdef __AVX2__
inline Packed<double,4>::scal_t Packed<double,4>::cvt_scal( vec_t a ) noexcept{
    return _mm256_cvtsd_f64( a );
}
#endif
inline int Packed<double,4>::movemask( vec_t a ) noexcept{
    return _mm256_movemask_pd(a);
}
inline Packed<double,4>::vec_t Packed<double,4>::moveldup( vec_t a ) noexcept{
    return _mm256_movedup_pd(a);
}
inline Packed<double,4>::vec_t Packed<double,4>::set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept{
    return _mm256_set_pd( e3, e2, e1, e0 );
}
inline Packed<double,4>::vec_t Packed<double,4>::set( scal_t a ) noexcept{
    return _mm256_set1_pd(a);
}
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
inline Packed<double,4>::vec_t Packed<double,4>::eq( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_EQ);
}
inline Packed<double,4>::vec_t Packed<double,4>::neq( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_NEQ);
}
inline Packed<double,4>::vec_t Packed<double,4>::false_( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_FALSE);
}
inline Packed<double,4>::vec_t Packed<double,4>::lt( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_LT);
}
inline Packed<double,4>::vec_t Packed<double,4>::le( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_LE);
}
inline Packed<double,4>::vec_t Packed<double,4>::gt( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_GT);
}
inline Packed<double,4>::vec_t Packed<double,4>::ge( vec_t a, vec_t b ) noexcept{
    return _mm256_cmp_pd(a,b,(int)Mode::CMP_GE);
}
inline Packed<double,4>::vec_t Packed<double,4>::cmp( vec_t a, vec_t b, const int op ) noexcept{
    return _mm256_cmp_pd(a,b,op);
}
inline Packed<double,4>::vec_t Packed<double,4>::and_( vec_t a, vec_t b ) noexcept{
    return _mm256_and_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::andnot( vec_t a, vec_t b ) noexcept{
    return _mm256_andnot_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::or_( vec_t a, vec_t b ) noexcept{
    return _mm256_or_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::xor_( vec_t a, vec_t b ) noexcept{
    return _mm256_xor_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::blend( vec_t a, vec_t b, int mask ) noexcept{
    return _mm256_blend_pd(a,b,mask);
}
inline Packed<double,4>::vec_t Packed<double,4>::blend( vec_t a, vec_t b, vec_t mask ) noexcept{
    return _mm256_blendv_pd(a,b,mask);
}
inline Packed<double,4>::vec_t Packed<double,4>::sqrt( vec_t a ) noexcept{
    return _mm256_sqrt_pd(a);
}
inline Packed<double,4>::vec_t Packed<double,4>::ceil( vec_t a ) noexcept{
    return _mm256_ceil_pd(a);
}
inline Packed<double,4>::vec_t Packed<double,4>::floor( vec_t a ) noexcept{
    return _mm256_floor_pd(a);
}
inline Packed<double,4>::vec_t Packed<double,4>::round( vec_t a, const int rounding ) noexcept{
    return _mm256_round_pd(a, rounding);
}
inline Packed<double,4>::vec_t Packed<double,4>::max( vec_t a, vec_t b ) noexcept{
    return _mm256_max_pd(a,b);
}
inline Packed<double,4>::vec_t Packed<double,4>::min( vec_t a, vec_t b ) noexcept{
    return _mm256_min_pd(a,b);
}
#endif

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PD256_H_