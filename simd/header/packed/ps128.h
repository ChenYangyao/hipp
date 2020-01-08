#ifndef _HIPPSIMD_PS128_H_
#define _HIPPSIMD_PS128_H_
#include "packedbase.h"
#include "../opcode/opcode.h"
namespace HIPP{
namespace SIMD{    
template<> class Packed<float, 4>{
    typedef float scal_t;
    typedef __m128 vec_t;
    enum: size_t { 
        NPACK=4, 
        NBIT=128, 
        VECSIZE=sizeof(vec_t), 
        SCALSIZE=sizeof(scal_t) };
    typedef typename TypeCvt<vec_t, -1, 0, 1>::ret vec_hc_t;

    typedef typename TypeCvt<vec_t, 0, 0, 0>::ret ivec_t;
    typedef int32_t iscal_t;
    typedef int64_t iscal_dp_t;
    typedef ivec_t mask_t;
    typedef ivec_t index_t;
    typedef u_int32_t imm_t;

    /**
     * L/S operations
     * load() - load 4 packed single precision floating-point from `mem_addr` 
     *          into dst.
     * Post-fix can be:
     * r:   reverse order.
     * u:   not aligned to any.
     * s:   load single float into lowest 32-bit of dst, zero others; (can be 
     *      not aligned).
     * 1:   broadcast single 32-bit float into dst.
     * hi:  load into higher two float, copy others from a.
     * lo:  load into lower two float, copy others from a.
     * 
     * bcast() - same as load1().
     * 
     * store() - store `a` into `mem_addr`
     * Post-fix can be:
     * r:   reverse order.
     * u:   not aligned to any in memory.
     * s:   store only lowest single float.
     * 1:   store only lowest single float, 4-copy.
     * hi:  store higher two floats.
     * lo:  store lower two floats.
     * 
     * stream() - non-temporary memory hints.
     */
    static vec_t load( const scal_t *mem_addr ) noexcept;
    static vec_t loadr( const scal_t *mem_addr ) noexcept;
    static vec_t loadu( const scal_t *mem_addr ) noexcept;
    static vec_t loads( const scal_t *mem_addr ) noexcept; 
    static vec_t load1( const scal_t *mem_addr) noexcept; 
    static vec_t loadhi( vec_t a, const vec_hc_t *mem_addr) noexcept; 
    static vec_t loadlo( vec_t a, const vec_hc_t *mem_addr) noexcept; 
    static vec_t bcast( const scal_t *mem_addr ) noexcept;
    static void store( scal_t *mem_addr, vec_t a ) noexcept;
    static void storer( scal_t *mem_addr, vec_t a ) noexcept;
    static void storeu( scal_t *mem_addr, vec_t a ) noexcept;
    static void stores( scal_t *mem_addr, vec_t a ) noexcept;   
    static void store1( scal_t *mem_addr, vec_t a ) noexcept;   
    static void storehi( vec_hc_t *mem_addr, vec_t a ) noexcept;
    static void storelo( vec_hc_t *mem_addr, vec_t a ) noexcept;
    static void stream( scal_t *mem_addr, vec_t a ) noexcept;

    /**
     * arithmetic and logic operations
     * The post-fix `s` means only do the operation for the lowest 32-bit, 
     * and copy the upper bits from a into dst.
     */
    static vec_t add( vec_t a, vec_t b ) noexcept;
    static vec_t adds( vec_t a, vec_t b ) noexcept;
    static vec_t sub( vec_t a, vec_t b ) noexcept;
    static vec_t subs( vec_t a, vec_t b ) noexcept;
    static vec_t div( vec_t a, vec_t b ) noexcept;
    static vec_t divs( vec_t a, vec_t b ) noexcept;
    static vec_t mul( vec_t a, vec_t b ) noexcept;
    static vec_t muls( vec_t a, vec_t b ) noexcept;
    static vec_t rcp( vec_t a ) noexcept;
    static vec_t rcps( vec_t a ) noexcept;
    static vec_t rsqrt( vec_t a ) noexcept;
    static vec_t rsqrts( vec_t a ) noexcept;
    static vec_t sqrt( vec_t a ) noexcept;
    static vec_t sqrts( vec_t a ) noexcept;

    static vec_t and_( vec_t a, vec_t b ) noexcept;
    static vec_t andnot( vec_t a, vec_t b ) noexcept;
    static vec_t or_( vec_t a, vec_t b ) noexcept;
    static vec_t xor_( vec_t a, vec_t b ) noexcept;

    /**
     * relation operations
     * Post-fix can be:
     * s:   do the operation for the lowest 32-bit, and copy the upper bits 
     *      from a into dst.
     * si:  do the operation for the lowest 32-bit, and return a bool result.
     * sui: same as si, but un-signaling.
     */
    static vec_t eq( vec_t a, vec_t b ) noexcept;
    static vec_t eqs( vec_t a, vec_t b ) noexcept;
    static int eqsi( vec_t a, vec_t b ) noexcept;
    static int eqsui( vec_t a, vec_t b ) noexcept;
    
    static vec_t ge( vec_t a, vec_t b ) noexcept;
    static vec_t ges( vec_t a, vec_t b ) noexcept;
    static int gesi( vec_t a, vec_t b ) noexcept;
    static int gesui( vec_t a, vec_t b ) noexcept;
    
    static vec_t gt( vec_t a, vec_t b ) noexcept;
    static vec_t gts( vec_t a, vec_t b ) noexcept;
    static int gtsi( vec_t a, vec_t b ) noexcept;
    static int gtsui( vec_t a, vec_t b ) noexcept;
    
    static vec_t le( vec_t a, vec_t b ) noexcept;
    static vec_t les( vec_t a, vec_t b ) noexcept;
    static int lesi( vec_t a, vec_t b ) noexcept;
    static int lesui( vec_t a, vec_t b ) noexcept;
    
    static vec_t lt( vec_t a, vec_t b ) noexcept;
    static vec_t lts( vec_t a, vec_t b ) noexcept;
    static int ltsi( vec_t a, vec_t b ) noexcept;
    static int ltsui( vec_t a, vec_t b ) noexcept;
    
    static vec_t neq( vec_t a, vec_t b ) noexcept;
    static vec_t neqs( vec_t a, vec_t b ) noexcept;
    static int neqsi( vec_t a, vec_t b ) noexcept;
    static int neqsui( vec_t a, vec_t b ) noexcept;
    
    static vec_t nge( vec_t a, vec_t b ) noexcept;
    static vec_t nges( vec_t a, vec_t b ) noexcept;
    
    static vec_t ngt( vec_t a, vec_t b ) noexcept;
    static vec_t ngts( vec_t a, vec_t b ) noexcept;
    
    static vec_t nle( vec_t a, vec_t b ) noexcept;
    static vec_t nles( vec_t a, vec_t b ) noexcept;
    
    static vec_t nlt( vec_t a, vec_t b ) noexcept;
    static vec_t nlts( vec_t a, vec_t b ) noexcept;
    
    static vec_t ord( vec_t a, vec_t b ) noexcept;
    static vec_t ords( vec_t a, vec_t b ) noexcept;
    static vec_t unord( vec_t a, vec_t b ) noexcept;
    static vec_t unords( vec_t a, vec_t b ) noexcept;

    static vec_t max( vec_t a, vec_t b ) noexcept;
    static vec_t maxs( vec_t a, vec_t b ) noexcept;

    static vec_t min( vec_t a, vec_t b ) noexcept;
    static vec_t mins( vec_t a, vec_t b ) noexcept;

    /**
     * register move
     */
    static vec_t set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    static vec_t setr( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;   // reverse order.
    static vec_t sets( scal_t a ) noexcept;                                     // set lowest 32-bit to a, others to zero.
    static vec_t set1( scal_t a ) noexcept;                                     // broadcast.
    static vec_t set1( vec_t a ) noexcept;                                      // broadcast a[0].
    static vec_t set() noexcept;                                                // set all zeros.
    static vec_t setzero() noexcept;                                            // same as set().
    static vec_t shuffle( vec_t a, vec_t b, imm_t imm8) noexcept;
    static vec_t unpackhi( vec_t a, vec_t b) noexcept;                          // convert a[3:0], b[3:0] into [ b[3] a[3] b[2] a[2] ].
    static vec_t unpacklo( vec_t a, vec_t b) noexcept;                          // convert a[3:0], b[3:0] into [ b[1] a[1] b[0] a[0] ].
    static vec_t undefined() noexcept;                                          // undefined elements.

    /**
     * type cast
     */
    static vec_t from_iscal( vec_t a, iscal_t b) noexcept;      // cvt b into lower float-32, copy others from a
    static vec_t from_iscal( vec_t a, iscal_dp_t b) noexcept;   //                  ... b = int-64
    static iscal_t to_iscal( vec_t a ) noexcept;                // cvt lower float-32 into dst int-32
    static iscal_t tot_iscal( vec_t a ) noexcept;               //                  ... truncate
    static iscal_t to_iscal_dp( vec_t a ) noexcept;             // cvt lower float-32 into dst int-64
    static iscal_t tot_iscal_dp( vec_t a ) noexcept;            //                  ... truncate
    static scal_t to_scal( vec_t a ) noexcept;                  // cvt lowest float-32 into single float
    static vec_t moves( vec_t a, vec_t b ) noexcept;
    static vec_t movehl( vec_t a, vec_t b ) noexcept;           // higher 2 floats from a, lower from b
    static vec_t movelh( vec_t a, vec_t b ) noexcept;           // lower ... ,higher...
    static int movemask( vec_t a ) noexcept;                    // move highest bit of each float into lowest 4 bits of dst.
};

inline Packed<float, 4>::vec_t Packed<float, 4>::load( const scal_t *mem_addr ) noexcept{ return _mm_load_ps(mem_addr); }
inline Packed<float, 4>::vec_t Packed<float, 4>::loadr( const scal_t *mem_addr ) noexcept{ return _mm_loadr_ps(mem_addr); }
inline Packed<float, 4>::vec_t Packed<float, 4>::loadu( const scal_t *mem_addr ) noexcept{ return _mm_loadu_ps(mem_addr); }
inline Packed<float, 4>::vec_t Packed<float, 4>::loads( const scal_t *mem_addr ) noexcept{ return _mm_load_ss(mem_addr); }
inline Packed<float, 4>::vec_t Packed<float, 4>::load1( const scal_t *mem_addr ) noexcept{ return bcast(mem_addr); }
inline Packed<float, 4>::vec_t Packed<float, 4>::loadhi( vec_t a, const vec_hc_t *mem_addr) noexcept{ return _mm_loadh_pi(a, mem_addr); }
inline Packed<float, 4>::vec_t Packed<float, 4>::loadlo( vec_t a, const vec_hc_t *mem_addr) noexcept{ return _mm_loadl_pi(a, mem_addr); }
inline Packed<float, 4>::vec_t Packed<float, 4>::bcast( const scal_t *mem_addr ) noexcept{ return _mm_load_ps1(mem_addr); }
inline void Packed<float, 4>::store( scal_t *mem_addr, vec_t a ) noexcept{ _mm_store_ps(mem_addr, a); }
inline void Packed<float, 4>::storer( scal_t *mem_addr, vec_t a ) noexcept{ _mm_storer_ps(mem_addr, a); }
inline void Packed<float, 4>::storeu( scal_t *mem_addr, vec_t a ) noexcept{ _mm_storeu_ps(mem_addr, a); }
inline void Packed<float, 4>::stores( scal_t *mem_addr, vec_t a ) noexcept{ _mm_store_ss(mem_addr, a); }
inline void Packed<float, 4>::store1( scal_t *mem_addr, vec_t a ) noexcept{ _mm_store_ps1(mem_addr, a); }
inline void Packed<float, 4>::storehi( vec_hc_t *mem_addr, vec_t a ) noexcept{ _mm_storeh_pi(mem_addr, a); }
inline void Packed<float, 4>::storelo( vec_hc_t *mem_addr, vec_t a ) noexcept{ _mm_storel_pi(mem_addr, a); }
inline void Packed<float, 4>::stream( scal_t *mem_addr, vec_t a ) noexcept{ _mm_stream_ps(mem_addr, a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::add( vec_t a, vec_t b ) noexcept{ return _mm_add_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::adds( vec_t a, vec_t b ) noexcept{ return _mm_add_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::sub( vec_t a, vec_t b ) noexcept{ return _mm_sub_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::subs( vec_t a, vec_t b ) noexcept{ return _mm_sub_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::div( vec_t a, vec_t b ) noexcept{ return _mm_div_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::divs( vec_t a, vec_t b ) noexcept{ return _mm_div_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::mul( vec_t a, vec_t b ) noexcept{ return _mm_mul_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::muls( vec_t a, vec_t b ) noexcept{ return _mm_mul_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::rcp( vec_t a ) noexcept{ return _mm_rcp_ps(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::rcps( vec_t a ) noexcept{ return _mm_rcp_ss(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::rsqrt( vec_t a ) noexcept{ return _mm_rsqrt_ps(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::rsqrts( vec_t a ) noexcept{ return _mm_rsqrt_ss(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::sqrt( vec_t a ) noexcept{ return _mm_sqrt_ps(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::sqrts( vec_t a ) noexcept{ return _mm_sqrt_ss(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::and_( vec_t a, vec_t b ) noexcept{ return _mm_and_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::andnot( vec_t a, vec_t b ) noexcept{ return _mm_andnot_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::or_( vec_t a, vec_t b ) noexcept{ return _mm_or_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::xor_( vec_t a, vec_t b ) noexcept{ return _mm_xor_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::eq( vec_t a, vec_t b ) noexcept{ return _mm_cmpeq_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::eqs( vec_t a, vec_t b ) noexcept{ return _mm_cmpeq_ss(a,b); }
inline int Packed<float, 4>::eqsi( vec_t a, vec_t b ) noexcept{ return _mm_comieq_ss(a,b); }
inline int Packed<float, 4>::eqsui( vec_t a, vec_t b ) noexcept{ return _mm_ucomieq_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::ge( vec_t a, vec_t b ) noexcept{ return _mm_cmpge_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::ges( vec_t a, vec_t b ) noexcept{ return _mm_cmpge_ss(a,b); }
inline int Packed<float, 4>::gesi( vec_t a, vec_t b ) noexcept{ return _mm_comige_ss(a,b); }
inline int Packed<float, 4>::gesui( vec_t a, vec_t b ) noexcept{ return _mm_ucomige_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::gt( vec_t a, vec_t b ) noexcept{ return _mm_cmpgt_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::gts( vec_t a, vec_t b ) noexcept{ return _mm_cmpgt_ss(a,b); }
inline int Packed<float, 4>::gtsi( vec_t a, vec_t b ) noexcept{ return _mm_comigt_ss(a,b); }
inline int Packed<float, 4>::gtsui( vec_t a, vec_t b ) noexcept{  return _mm_ucomigt_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::le( vec_t a, vec_t b ) noexcept{ return _mm_cmple_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::les( vec_t a, vec_t b ) noexcept{ return _mm_cmple_ss(a,b); }
inline int Packed<float, 4>::lesi( vec_t a, vec_t b ) noexcept{ return _mm_comile_ss(a,b); }
inline int Packed<float, 4>::lesui( vec_t a, vec_t b ) noexcept{ return _mm_ucomile_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::lt( vec_t a, vec_t b ) noexcept{ return _mm_cmplt_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::lts( vec_t a, vec_t b ) noexcept{ return _mm_cmplt_ss(a,b); }
inline int Packed<float, 4>::ltsi( vec_t a, vec_t b ) noexcept{ return _mm_comilt_ss(a,b); }
inline int Packed<float, 4>::ltsui( vec_t a, vec_t b ) noexcept{ return _mm_ucomilt_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::neq( vec_t a, vec_t b ) noexcept{ return _mm_cmpneq_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::neqs( vec_t a, vec_t b ) noexcept{ return _mm_cmpneq_ss(a,b); }
inline int Packed<float, 4>::neqsi( vec_t a, vec_t b ) noexcept{ return _mm_comineq_ss(a,b); }
inline int Packed<float, 4>::neqsui( vec_t a, vec_t b ) noexcept{ return _mm_ucomineq_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::nge( vec_t a, vec_t b ) noexcept{ return _mm_cmpnge_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::nges( vec_t a, vec_t b ) noexcept{ return _mm_cmpnge_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::ngt( vec_t a, vec_t b ) noexcept{ return _mm_cmpngt_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::ngts( vec_t a, vec_t b ) noexcept{ return _mm_cmpngt_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::nle( vec_t a, vec_t b ) noexcept{ return _mm_cmpnle_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::nles( vec_t a, vec_t b ) noexcept{ return _mm_cmpnle_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::nlt( vec_t a, vec_t b ) noexcept{ return _mm_cmpnlt_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::nlts( vec_t a, vec_t b ) noexcept{ return _mm_cmpnlt_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::ord( vec_t a, vec_t b ) noexcept{ return _mm_cmpord_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::ords( vec_t a, vec_t b ) noexcept{ return _mm_cmpord_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::unord( vec_t a, vec_t b ) noexcept{ return _mm_cmpunord_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::unords( vec_t a, vec_t b ) noexcept{ return _mm_cmpunord_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::max( vec_t a, vec_t b ) noexcept{ return _mm_max_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::maxs( vec_t a, vec_t b ) noexcept{ return _mm_max_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::min( vec_t a, vec_t b ) noexcept{ return _mm_min_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::mins( vec_t a, vec_t b ) noexcept{ return _mm_min_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept{ return _mm_set_ps(e3,e2,e1,e0); }
inline Packed<float, 4>::vec_t Packed<float, 4>::setr( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept{ return _mm_setr_ps(e3,e2,e1,e0); }
inline Packed<float, 4>::vec_t Packed<float, 4>::sets( scal_t a ) noexcept{ return _mm_set_ss(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::set1( scal_t a ) noexcept{ return _mm_set_ps1(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::set1( vec_t a ) noexcept{ return _mm_broadcastss_ps(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::set() noexcept{ return _mm_setzero_ps(); }
inline Packed<float, 4>::vec_t Packed<float, 4>::setzero() noexcept{ return set(); }

inline Packed<float, 4>::vec_t Packed<float, 4>::shuffle( vec_t a, vec_t b, imm_t imm8) noexcept{ return _mm_shuffle_ps(a,b,imm8); }
inline Packed<float, 4>::vec_t Packed<float, 4>::unpackhi( vec_t a, vec_t b) noexcept{ return _mm_unpackhi_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::unpacklo( vec_t a, vec_t b) noexcept{ return _mm_unpacklo_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::undefined() noexcept{ return _mm_undefined_ps(); }
inline Packed<float, 4>::vec_t Packed<float, 4>::from_iscal( vec_t a, iscal_t b) noexcept{ return _mm_cvt_si2ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::from_iscal( vec_t a, iscal_dp_t b) noexcept{ return _mm_cvtsi64_ss(a,b); }
inline Packed<float, 4>::iscal_t Packed<float, 4>::to_iscal( vec_t a ) noexcept{ return _mm_cvt_ss2si(a); }
inline Packed<float, 4>::iscal_t Packed<float, 4>::tot_iscal( vec_t a ) noexcept{ return _mm_cvtt_ss2si(a); }
inline Packed<float, 4>::iscal_t Packed<float, 4>::to_iscal_dp( vec_t a ) noexcept{ return _mm_cvtss_si64(a); }
inline Packed<float, 4>::iscal_t Packed<float, 4>::tot_iscal_dp( vec_t a ) noexcept{ return _mm_cvtss_si64(a); }
inline Packed<float, 4>::scal_t Packed<float, 4>::to_scal( vec_t a ) noexcept{ return _mm_cvtss_f32(a); }
inline Packed<float, 4>::vec_t Packed<float, 4>::moves( vec_t a, vec_t b ) noexcept{ return _mm_move_ss(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::movehl( vec_t a, vec_t b ) noexcept{ return _mm_movehl_ps(a,b); }
inline Packed<float, 4>::vec_t Packed<float, 4>::movelh( vec_t a, vec_t b ) noexcept{ return _mm_movelh_ps(a,b); }
inline int Packed<float, 4>::movemask( vec_t a ) noexcept{ return _mm_movemask_ps(a); }

} // namespace SIMD
} // namespace HIPP



#endif	//_HIPPSIMD_PS128_H_