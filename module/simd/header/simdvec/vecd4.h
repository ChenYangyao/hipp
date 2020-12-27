#ifndef _HIPPSIMD_VECD4_H_
#define _HIPPSIMD_VECD4_H_
#include "vecbase.h"
#include "math.h"
#include "hippcntl.h"
#include "veci32_8.h"
#include "veci64_4.h"

namespace HIPP{
namespace SIMD{

#ifdef __AVX__

namespace _pd256_helper{
struct AddrAligned: public Packd4Base {
    typedef Vec<double, 4> vec; 
    
    AddrAligned( vec *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){} 
    AddrAligned( scal_t *addr ): _addr(addr){}
    AddrAligned( vec_t *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){}

    scal_t * const _addr;
};
struct CAddrAligned: public Packd4Base {
    typedef Vec<double, 4> vec; 

    CAddrAligned( const vec *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){} 
    CAddrAligned( const scal_t *addr ): _addr(addr){}
    CAddrAligned( const vec_t *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){}

    const scal_t * const _addr;
};
} // namespace _pd256_helper

template<> 
class Vec<double, 4>: public _pd256_helper::Packd4Base {
public:
    typedef Packed<double, 4> pack_t;
    typedef Vec<iscal_t, 4> IntVec;
    typedef Vec<int32_t, 4> IntVecHP;
    typedef Vec<double, 2>  VecHC;
    typedef Vec<float, 4>  VecHP;
    
    typedef _pd256_helper::AddrAligned addr_t;
    typedef _pd256_helper::CAddrAligned caddr_t;
    
    Vec() noexcept{}
    Vec( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    explicit Vec( scal_t a ) noexcept;
    explicit Vec( caddr_t mem_addr ) noexcept;
    Vec( const vec_t &a ) noexcept;
    Vec( const scal_t *base_addr, 
        const IntVec &vindex, const int scale=SCALSIZE ) noexcept;
    Vec( const Vec &src, const scal_t *base_addr, 
        const IntVec &vindex, const Vec &mask, const int scale=SCALSIZE ) noexcept;
    Vec( const Vec &a ) noexcept;
    Vec( Vec &&a ) noexcept;
    ~Vec() noexcept {}
    Vec & operator=( const Vec &a ) noexcept;
    Vec & operator=( Vec &&a ) noexcept;

    ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Vec &v );

    const vec_t & val() const noexcept;
    vec_t & val() noexcept;
    const scal_t & operator[]( size_t n )const noexcept;
    scal_t & operator[]( size_t n ) noexcept;

    Vec & load( caddr_t mem_addr ) noexcept;
    Vec & loadu( caddr_t mem_addr ) noexcept;
    Vec & loadm( caddr_t mem_addr, const IntVec &mask ) noexcept;
    Vec & load1( const scal_t *mem_addr ) noexcept;
    Vec & bcast( const scal_t *mem_addr ) noexcept;
    Vec & bcast( const vec_hc_t *mem_addr ) noexcept;
    Vec & gather( const scal_t *base_addr, const IntVec &vindex, 
        const int scale=SCALSIZE ) noexcept;
    Vec & gatherm( const Vec &src, const scal_t *base_addr, const IntVec &vindex, 
        const Vec &mask, const int scale=SCALSIZE ) noexcept;
    Vec & gather( const scal_t *base_addr, const IntVecHP &vindex, 
        const int scale=SCALSIZE ) noexcept;
    Vec & gatherm( const Vec &src, const scal_t *base_addr, const IntVecHP &vindex, 
        const Vec &mask, const int scale=SCALSIZE ) noexcept;

    const Vec & store( addr_t mem_addr ) const noexcept;
    const Vec & storem( addr_t mem_addr, const IntVec &mask ) const noexcept;
    const Vec & storeu( addr_t mem_addr ) const noexcept;
    const Vec & stream( addr_t mem_addr ) const noexcept;
    Vec & store( addr_t mem_addr ) noexcept;
    Vec & storem( addr_t mem_addr, const IntVec &mask ) noexcept;
    Vec & storeu( addr_t mem_addr ) noexcept;
    Vec & stream( addr_t mem_addr ) noexcept;
    const Vec & scatter( void *base_addr, 
        const IntVec &vindex, int scale=SCALSIZE ) const noexcept;
    const Vec & scatterm( void *base_addr, mask8_t k,
        const IntVec &vindex, int scale=SCALSIZE ) const noexcept;
    const Vec & scatter( void *base_addr, 
        const IntVecHP &vindex, int scale=SCALSIZE ) const noexcept;
    const Vec & scatterm( void *base_addr, mask8_t k,
        const IntVecHP &vindex, int scale=SCALSIZE ) const noexcept;
    Vec & scatter( void *base_addr, 
        const IntVec &vindex, int scale=SCALSIZE ) noexcept;
    Vec & scatterm( void *base_addr, mask8_t k,
        const IntVec &vindex, int scale=SCALSIZE ) noexcept;
    Vec & scatter( void *base_addr, 
        const IntVecHP &vindex, int scale=SCALSIZE ) noexcept;
    Vec & scatterm( void *base_addr, mask8_t k,
        const IntVecHP &vindex, int scale=SCALSIZE ) noexcept;

    
    scal_t to_scal( ) const noexcept;
    Vec & from_si(const IntVec &a) noexcept;
    IntVec to_si() const noexcept;
    IntVecHP to_ivec_hp() const noexcept;
    IntVecHP tot_ivec_hp() const noexcept;
    IntVecHP to_i32vec() const noexcept;
    IntVecHP tot_i32vec() const noexcept;
    VecHC to_vec_hc() const noexcept;
    VecHC extract_hc(const int imm8) const noexcept;
    VecHP to_vec_hp() const noexcept;
    VecHP to_f32vec() const noexcept;
    Vec unpackhi(const Vec &b) const noexcept                                   { return pack_t::unpackhi(_val, b._val); }
    Vec unpacklo(const Vec &b) const noexcept                                   { return pack_t::unpacklo(_val, b._val); }

    int movemask( ) const noexcept;
    Vec movedup( ) const noexcept;

    Vec & set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    Vec & set1( scal_t a ) noexcept;
    Vec & set1( vec_hc_t a ) noexcept;
    Vec & set() noexcept;
    Vec & setzero() noexcept;
    Vec & undefined() noexcept;

    /**
     * in all binary operator/blend, 
     * 'this' is the left operator, input argument `a` is the right operator.
     */
    friend Vec operator+( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator-( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator*( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator/( const Vec &a, const Vec &b ) noexcept;
    Vec operator++(int) noexcept;
    Vec & operator++() noexcept;
    Vec operator--(int) noexcept;
    Vec & operator--() noexcept;
    Vec & operator+=( const Vec &a ) noexcept;
    Vec & operator-=( const Vec &a ) noexcept;
    Vec & operator*=( const Vec &a ) noexcept;
    Vec & operator/=( const Vec &a ) noexcept;
    Vec hadd( const Vec &a ) const noexcept;
    Vec hsub( const Vec &a ) const noexcept;

    friend Vec operator&( const Vec &a, const Vec &b ) noexcept;
    Vec andnot( const Vec &a ) const noexcept;
    friend Vec operator|( const Vec &a, const Vec &b ) noexcept;
    Vec operator~()const noexcept;
    friend Vec operator^( const Vec &a, const Vec &b ) noexcept;
    Vec & operator&=( const Vec &a ) noexcept;
    Vec & operator|=( const Vec &a ) noexcept;
    Vec & operator^=( const Vec &a ) noexcept;
    
    friend Vec operator==( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator!=( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator<( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator<=( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator>( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator>=( const Vec &a, const Vec &b ) noexcept;
    int testz(const Vec &a) const noexcept                                      { return pack_t::testz(_val, a._val); }
    int testz() const noexcept                                                  { return pack_t::testz(_val, _val); }

    Vec blend( const Vec &a, const int imm8 ) const noexcept;
    Vec blend( const Vec &a, const Vec &mask ) const noexcept;

    Vec sqrt() const noexcept;
    Vec ceil() const noexcept;
    Vec floor() const noexcept;
    Vec round( const int rounding ) const noexcept;
    Vec max( const Vec &a ) const noexcept;
    Vec min( const Vec &a ) const noexcept;

    Vec sin() const noexcept;
    Vec cos() const noexcept;
    Vec log() const noexcept;
    Vec exp() const noexcept;
    Vec pow( const Vec &a ) const noexcept;

    scal_t sum_all() const noexcept;

    /**
     * implement after Vec<s,4>
    Vec log2_fast() const noexcept;
    Vec log_fast() const noexcept;
    Vec log10_fast() const noexcept;
    Vec log2_faster() const noexcept;
    Vec log_faster() const noexcept;
    Vec log10_faster() const noexcept;

    Vec pow2_fast() const noexcept;
    Vec exp_fast() const noexcept;
    Vec pow10_fast() const noexcept;
    Vec pow2_faster() const noexcept;
    Vec exp_faster() const noexcept;
    Vec pow10_faster() const noexcept;
    */
protected:
    vec_t _val;
    union _u_ivv_t{ IntVec i; vec_t f; ~_u_ivv_t(){} };
    union _u_viv_t{ vec_t f; IntVec i; ~_u_viv_t(){} };
    union _u_vs_t{ vec_t f; scal_t s[NPACK]; ~_u_vs_t(){} };
    union _u_sv_t{ scal_t s[NPACK]; vec_t f; ~_u_sv_t(){} };
    union _u_ivs_t{ IntVec i; iscal_t s[NPACK]; ~_u_ivs_t(){} };
    union _u_siv_t{ iscal_t s[NPACK]; IntVec i; ~_u_siv_t(){} };
};



#endif //__AVX__

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_VECD4_H_