#ifndef _HIPPSIMD_VECS8_H_
#define _HIPPSIMD_VECS8_H_
#include "vecbase.h"
#include "../simdpacked/ps256.h"
#include "hippcntl.h"
namespace HIPP{
namespace SIMD{
    
#ifdef __AVX__

namespace _ps256_helper{
struct AddrAligned: public Packs8Base {
    typedef Vec<float, 8> vec; 
    
    AddrAligned( vec *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){} 
    AddrAligned( scal_t *addr ): _addr(addr){}
    AddrAligned( vec_t *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){}

    scal_t * const _addr;
};
struct CAddrAligned: public Packs8Base {
    typedef Vec<float, 8> vec; 
    
    CAddrAligned( const vec *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){} 
    CAddrAligned( const scal_t *addr ): _addr(addr){}
    CAddrAligned( const vec_t *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){}

    const scal_t * const _addr;
};
} // namespace _ps256_helper


template<>
class Vec<float, 8>: public _ps256_helper::Packs8Base {
public:
    typedef Packed<float, 8> pack_t;
    typedef _ps256_helper::AddrAligned addr_t;
    typedef _ps256_helper::CAddrAligned caddr_t;

    typedef Vec<iscal_t, 8> IntVec;
    typedef Vec<float, 4> VecHC;

    Vec() noexcept {}
    Vec( scal_t e7, scal_t e6, scal_t e5, scal_t e4, 
        scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    explicit Vec( scal_t a ) noexcept;
    explicit Vec( caddr_t mem_addr ) noexcept;
    Vec( const vec_t &a ) noexcept;
    Vec( const scal_t *base_addr, ivec_t vindex, 
        const int scale=SCALSIZE ) noexcept;
    Vec( vec_t src, const scal_t *base_addr, 
        ivec_t vindex, vec_t mask, const int scale ) noexcept;
    Vec( const Vec &a ) noexcept;
    Vec( Vec &&a ) noexcept;
    ~Vec() noexcept {}
    Vec & operator=( const Vec &a ) noexcept;
    Vec & operator=( Vec &&a ) noexcept;

    ostream & info( ostream &os = cout, int fmt_cntl = 1 ) const;
    friend ostream & operator<<( ostream &os, const Vec &v );

    const vec_t & val() const noexcept;
    vec_t & val() noexcept;
    const scal_t & operator[]( size_t n ) const noexcept;
    scal_t & operator[]( size_t n ) noexcept;

    Vec & load( caddr_t mem_addr ) noexcept;
    Vec & loadu( caddr_t mem_addr ) noexcept;
    Vec & loadm( caddr_t mem_addr, ivec_t mask ) noexcept;
    Vec & load1( const scal_t *mem_addr ) noexcept;
    Vec & bcast( const scal_t *mem_addr ) noexcept;
    Vec & bcast( const vec_hc_t *mem_addr ) noexcept;

    /* gather and _m need AVX2, otherwise serialized */
    Vec & gather( const scal_t *base_addr, 
        const IntVec &vindex, const int scale=SCALSIZE ) noexcept;
    Vec & gatherm(const Vec &src, const scal_t *base_addr, 
        const IntVec &vindex,const Vec &mask, const int scale=SCALSIZE ) noexcept;
    const Vec & store( addr_t mem_addr ) const noexcept;
    const Vec & storem( addr_t mem_addr, const IntVec &mask ) const noexcept;
    const Vec & storeu( addr_t mem_addr ) const noexcept;
    const Vec & stream( addr_t mem_addr ) const noexcept;
    Vec & store( addr_t mem_addr ) noexcept;
    Vec & storem( addr_t mem_addr, const IntVec &mask ) noexcept;
    Vec & storeu( addr_t mem_addr ) noexcept;
    Vec & stream( addr_t mem_addr ) noexcept;

    /* scatter and _m need AVX512F & AVX512VL, otherwise serialized */
    const Vec & scatter(void *base_addr, const IntVec &vindex, 
        int scale=SCALSIZE) const noexcept;
    const Vec & scatterm(void *base_addr, mask8_t k, const IntVec &vindex, 
        int scale=SCALSIZE) const noexcept;
    Vec & scatter(void *base_addr, const IntVec &vindex, 
        int scale=SCALSIZE) noexcept;
    Vec & scatterm(void *base_addr, mask8_t k, const IntVec &vindex, 
        int scale=SCALSIZE) noexcept;

    scal_t to_scal()const noexcept;
    IntVec to_ivec()const noexcept;
    IntVec tot_ivec() const noexcept;
    IntVec to_i32vec()const noexcept;       // same as to_ivec
    IntVec tot_i32vec() const noexcept;     // same as tot_ivec

    Vec & from_si(const IntVec &a) noexcept;
    IntVec to_si()const noexcept;
    VecHC to_vec_hc() const noexcept;
    VecHC extract_hc(const int imm8) const noexcept;
    // Does not change. Only for generic programming
    Vec to_f32vec() const noexcept                                              { return _val; }

    int movemask( ) const noexcept;
    Vec movehdup( ) const noexcept;
    Vec moveldup( ) const noexcept;
    Vec & set( scal_t e7, scal_t e6, scal_t e5, scal_t e4, 
        scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    Vec & set1( scal_t a ) noexcept;
    Vec & set1( vec_hc_t a ) noexcept;
    Vec & set() noexcept;
    Vec & setzero() noexcept;
    Vec & undefined() noexcept;

    friend Vec operator+( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator-( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator*( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator/( const Vec &a, const Vec &b ) noexcept;
    Vec operator++(int) noexcept;
    Vec & operator++() noexcept;
    Vec operator--(int) noexcept;
    Vec & operator--() noexcept;
    Vec & operator+=(const Vec &a) noexcept;
    Vec & operator-=(const Vec &a) noexcept;
    Vec & operator*=(const Vec &a) noexcept;
    Vec & operator/=(const Vec &a) noexcept;
    Vec hadd( const Vec &a ) const noexcept;
    Vec hsub( const Vec &a ) const noexcept;

    friend Vec operator&( const Vec &a, const Vec &b) noexcept;     // bitwise operation
    friend Vec operator|( const Vec &a, const Vec &b) noexcept;
    friend Vec operator^( const Vec &a, const Vec &b) noexcept;
    Vec andnot( const Vec &a ) const noexcept;
    Vec operator~()const noexcept;
    Vec & operator&=( const Vec &a ) noexcept;
    Vec & operator|=( const Vec &a ) noexcept;
    Vec & operator^=( const Vec &a ) noexcept;
    
    friend Vec operator==( const Vec &a, const Vec &b) noexcept;    // for each single float. set all bits if yes
    friend Vec operator!=( const Vec &a, const Vec &b) noexcept;
    friend Vec operator<( const Vec &a, const Vec &b) noexcept;
    friend Vec operator<=( const Vec &a, const Vec &b) noexcept;
    friend Vec operator>( const Vec &a, const Vec &b) noexcept;
    friend Vec operator>=( const Vec &a, const Vec &b) noexcept;
    int testz(const Vec &a) const noexcept                                      { return pack_t::testz(_val, a._val); }
    int testz() const noexcept                                                  { return pack_t::testz(_val, _val); }

    Vec blend( const Vec &a, const int imm8 ) const noexcept;
    Vec blend( const Vec &a, const Vec &mask ) const noexcept;

    Vec rcp() const noexcept;
    Vec sqrt() const noexcept;
    Vec rsqrt() const noexcept;
    Vec ceil() const noexcept;
    Vec floor() const noexcept;
    Vec round( const int rounding ) const noexcept;
    Vec max( const Vec &a ) const noexcept;
    Vec min( const Vec &a ) const noexcept;

    Vec log2_fast( ) const noexcept;
    Vec log_fast( ) const noexcept;
    Vec log10_fast( ) const noexcept;
    Vec log2_faster( ) const noexcept;
    Vec log_faster( ) const noexcept;
    Vec log10_faster( ) const noexcept;

    Vec pow2_fast() const noexcept;
    Vec exp_fast() const noexcept;
    Vec pow10_fast() const noexcept;
    Vec pow2_faster() const noexcept;
    Vec exp_faster() const noexcept;
    Vec pow10_faster() const noexcept;

    scal_t sum_all() const noexcept;
protected:
    vec_t _val;
    union _u_ivv_t{ ivec_t i; vec_t f; };
    union _u_viv_t{ vec_t f; ivec_t i; };
    union _u_vs_t{ vec_t f; scal_t s[NPACK]; };
    union _u_sv_t{ scal_t s[NPACK]; vec_t f; };
    union _u_ivs_t{ ivec_t i; iscal_t s[NPACK]; };
    union _u_siv_t{ iscal_t s[NPACK]; ivec_t i; };
};


#endif  // __AVX__

} // namespace SIMD
} // namespace HIPP

#endif	//_HIPPSIMD_VECS8_H_