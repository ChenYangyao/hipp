#ifndef _HIPPSIMD_PD256_H_
#define _HIPPSIMD_PD256_H_
#include "packedbase.h"
#include "../opcode/opcode.h"
namespace HIPP{
namespace SIMD{

#ifdef __AVX__

namespace _pd256_helper{
class Packd4Base{
public:
    typedef double scal_t;
    typedef __m256d vec_t;
    enum: size_t { NPACK=4, NBIT=256, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };

    typedef typename TypeCvt<vec_t, -1, 0, 1>::ret vec_hc_t;
    typedef typename TypeCvt<vec_t, 0, -1, 1>::ret vec_hp_t;

    typedef typename TypeCvt<vec_t, 0, 0, 0>::ret ivec_t;
    typedef ivec_t mask_t;
    typedef ivec_t index_t;
    typedef typename TypeCvt<vec_t, 0, -1, 0>::ret ivec_hp_t;    
};
} // namespace _pd256_helper

template<> 
class Packed<double, 4>: public _pd256_helper::Packd4Base{
public:
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
    static vec_t loadm( const scal_t *mem_addr, mask_t mask ) noexcept;
    static vec_t load1( const scal_t *mem_addr ) noexcept;
    static vec_t bcast( const scal_t *mem_addr ) noexcept;
    static vec_t bcast( const vec_hc_t *mem_addr ) noexcept;
#ifdef __AVX2__
    static vec_t gather( const scal_t *base_addr, 
        index_t vindex, const int scale=SCALSIZE ) noexcept;
    static vec_t gatherm( vec_t src, const scal_t *base_addr, 
        index_t vindex, vec_t mask, const int scale=SCALSIZE ) noexcept;
#endif
    static void store( scal_t *mem_addr, vec_t a ) noexcept;
    static void storem( scal_t *mem_addr, mask_t mask, vec_t a ) noexcept;
    static void storeu( scal_t *mem_addr, vec_t a ) noexcept;
    static void stream( scal_t *mem_addr, vec_t a ) noexcept;

    static vec_t from_ivec_hp( ivec_hp_t a ) noexcept;
    static ivec_hp_t to_ivec_hp( vec_t a ) noexcept;
    static ivec_hp_t tot_ivec_hp( vec_t a ) noexcept;
    static vec_t from_vec_hp( vec_hp_t a ) noexcept;
    static vec_hp_t to_vec_hp( vec_t a ) noexcept;
#ifdef __AVX2__
    static scal_t to_scal( vec_t a ) noexcept;
#endif
    static int movemask( vec_t a ) noexcept;
    static vec_t movehdup( vec_t a ) noexcept;
    static vec_t moveldup( vec_t a ) noexcept;
    static vec_t set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    static vec_t set1( scal_t a ) noexcept;
#ifdef __AVX2__
    static vec_t set1( vec_hc_t a ) noexcept;
#endif
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
inline Packed<double,4>::vec_t Packed<double,4>::loadm( const scal_t *mem_addr, mask_t mask ) noexcept{
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
inline Packed<double,4>::vec_t Packed<double,4>::gather( const scal_t *base_addr, index_t vindex, const int scale ) noexcept{
    return _mm256_i64gather_pd(base_addr, vindex, scale);
}
inline Packed<double,4>::vec_t Packed<double,4>::gatherm( vec_t src, const scal_t *base_addr, index_t vindex, vec_t mask, const int scale ) noexcept{
    return _mm256_mask_i64gather_pd(src, base_addr, vindex, mask, scale);
}
#endif

inline void Packed<double,4>::store( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_store_pd(mem_addr, a);
}
inline void Packed<double,4>::storem( scal_t *mem_addr, mask_t mask, vec_t a ) noexcept{
    _mm256_maskstore_pd(mem_addr, mask, a);
}
inline void Packed<double,4>::storeu( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_storeu_pd(mem_addr, a);
}
inline void Packed<double,4>::stream( scal_t *mem_addr, vec_t a ) noexcept{
    _mm256_stream_pd(mem_addr, a);
}
inline Packed<double,4>::vec_t Packed<double,4>::from_ivec_hp( ivec_hp_t a ) noexcept{
    return _mm256_cvtepi32_pd( a );
}
inline Packed<double,4>::ivec_hp_t Packed<double,4>::to_ivec_hp( vec_t a ) noexcept{
    return _mm256_cvtpd_epi32(a);
}
inline Packed<double,4>::ivec_hp_t Packed<double,4>::tot_ivec_hp( vec_t a ) noexcept{
    return _mm256_cvttpd_epi32(a);
}
inline Packed<double,4>::vec_t Packed<double,4>::from_vec_hp( vec_hp_t a ) noexcept{
    return _mm256_cvtps_pd( a );
}
inline Packed<double,4>::vec_hp_t Packed<double,4>::to_vec_hp( vec_t a ) noexcept{
    return _mm256_cvtpd_ps(a);
}
#ifdef __AVX2__
inline Packed<double,4>::scal_t Packed<double,4>::to_scal( vec_t a ) noexcept{
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