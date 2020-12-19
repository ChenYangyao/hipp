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
    Vec & gather( const scal_t *base_addr, 
        ivec_t vindex, const int scale=SCALSIZE ) noexcept;
    Vec & gatherm( vec_t src, const scal_t *base_addr, 
        ivec_t vindex, vec_t mask, const int scale=SCALSIZE ) noexcept;
    const Vec & store( addr_t mem_addr ) const noexcept;
    const Vec & storem( addr_t mem_addr, ivec_t mask ) const noexcept;
    const Vec & storeu( addr_t mem_addr ) const noexcept;
    const Vec & stream( addr_t mem_addr ) const noexcept;
    Vec & store( addr_t mem_addr ) noexcept;
    Vec & storem( addr_t mem_addr, ivec_t mask ) noexcept;
    Vec & storeu( addr_t mem_addr ) noexcept;
    Vec & stream( addr_t mem_addr ) noexcept;
    const Vec & scatter(void *base_addr, ivec_t vindex, 
        int scale=SCALSIZE) const noexcept;
    const Vec & scatterm(void *base_addr, mask8_t k, ivec_t vindex, 
        int scale=SCALSIZE) const noexcept;
    Vec & scatter(void *base_addr, ivec_t vindex, 
        int scale=SCALSIZE) noexcept;
    Vec & scatterm(void *base_addr, mask8_t k, ivec_t vindex, 
        int scale=SCALSIZE) noexcept;

    scal_t to_scal()const noexcept;
    IntVec to_ivec()const noexcept;

    Vec & from_si(const IntVec &a) noexcept;
    IntVec to_si()const noexcept;

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
    Vec andnot( const Vec &a ) const noexcept;
    friend Vec operator|( const Vec &a, const Vec &b) noexcept;
    Vec operator~()const noexcept;
    friend Vec operator^( const Vec &a, const Vec &b) noexcept;
    Vec & operator&=( const Vec &a ) noexcept;
    Vec & operator|=( const Vec &a ) noexcept;
    Vec & operator^=( const Vec &a ) noexcept;
    
    friend Vec operator==( const Vec &a, const Vec &b) noexcept;    // for each single float. set all bits if yes
    friend Vec operator!=( const Vec &a, const Vec &b) noexcept;
    friend Vec operator<( const Vec &a, const Vec &b) noexcept;
    friend Vec operator<=( const Vec &a, const Vec &b) noexcept;
    friend Vec operator>( const Vec &a, const Vec &b) noexcept;
    friend Vec operator>=( const Vec &a, const Vec &b) noexcept;

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
protected:
    vec_t _val;
    union _u_ivv_t{ ivec_t i; vec_t f; };
    union _u_viv_t{ vec_t f; ivec_t i; };
    union _u_vs_t{ vec_t f; scal_t s[NPACK]; };
    union _u_sv_t{ scal_t s[NPACK]; vec_t f; };
    union _u_ivs_t{ ivec_t i; iscal_t s[NPACK]; };
    union _u_siv_t{ iscal_t s[NPACK]; ivec_t i; };
};


inline Vec<float,8>::Vec( scal_t e7, scal_t e6, scal_t e5, scal_t e4, 
    scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept 
    : _val( pack_t::set(e7, e6, e5, e4, e3, e2, e1, e0) ){ }
inline Vec<float,8>::Vec( scal_t a ) noexcept
    : _val( pack_t::set1(a) ){ }
inline Vec<float,8>::Vec( caddr_t mem_addr ) noexcept
    : _val( pack_t::load(mem_addr._addr) ){ }
inline Vec<float,8>::Vec( const vec_t &a ) noexcept
    : _val(a){ }
inline Vec<float,8>::Vec( const scal_t *base_addr, ivec_t vindex, 
    const int scale ) noexcept
    : _val( pack_t::gather(base_addr, vindex, scale) ){ }
inline Vec<float,8>::Vec( vec_t src, const scal_t *base_addr, 
    ivec_t vindex, vec_t mask, const int scale ) noexcept
    : _val( pack_t::gatherm(src, base_addr, vindex, mask, scale) ){ }
inline Vec<float,8>::Vec( const Vec &a ) noexcept
    : _val( a._val ){ }
inline Vec<float,8>::Vec( Vec &&a ) noexcept
    : _val( a._val ){ }
inline Vec<float,8> & Vec<float,8>::operator=( const Vec &a ) noexcept{
    _val = a._val;
    return *this;
}
inline Vec<float,8> & Vec<float,8>::operator=( Vec &&a ) noexcept{
    _val = a._val;
    return *this;
}
inline ostream & Vec<float,8>::info( ostream &os, int fmt_cntl ) const{
    prt( os,  "HIPP::SIMD::Vec<float,8>("); 
    const scal_t *p = (const scal_t *)&_val;
    prt_a(os, p, p+NPACK) << ")";
    if( fmt_cntl >= 1 ) os << endl;
    return os;
}
inline ostream & operator<<( ostream &os, const Vec<float,8> &v ){
    return v.info(os);
}
inline const Vec<float,8>::vec_t & Vec<float,8>::val() const noexcept{
    return _val;
}
inline Vec<float,8>::vec_t & Vec<float,8>::val() noexcept{
    return _val;
}
inline const Vec<float,8>::scal_t & 
Vec<float,8>::operator[]( size_t n ) const noexcept{
    return ((const scal_t *)&_val)[n];
}
inline Vec<float,8>::scal_t & Vec<float,8>::operator[]( size_t n ) noexcept{
    return ((scal_t *)&_val)[n];
}
inline Vec<float,8> & Vec<float,8>::load( caddr_t mem_addr ) noexcept{
    _val = pack_t::load(mem_addr._addr);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::loadu( caddr_t mem_addr ) noexcept{
    _val = pack_t::loadu(mem_addr._addr);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::loadm( caddr_t mem_addr, 
    ivec_t mask ) noexcept{
    _val = pack_t::loadm(mem_addr._addr, mask);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::load1( const scal_t *mem_addr ) noexcept{
    _val = pack_t::load1(mem_addr);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::bcast( const scal_t *mem_addr ) noexcept{
    _val = pack_t::bcast(mem_addr);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::bcast( const vec_hc_t *mem_addr ) noexcept{
    _val = pack_t::bcast(mem_addr);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::gather( const scal_t *base_addr, 
    ivec_t vindex, const int scale ) noexcept{
#ifdef __AVX2__
    _val = pack_t::gather(base_addr, vindex, scale);
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<float,8>::gather is implemented sequentially
#endif
    _u_vs_t val;
    const _u_ivs_t idx = { vindex };
    for (size_t i = 0; i < NPACK; i++){
        val.s[i] = *(const scal_t *)((const void *)base_addr + idx.s[i]*scale);
    }
    _val = val.f;
#endif
    return *this;
}
inline Vec<float,8> & Vec<float,8>::gatherm( vec_t src, const scal_t *base_addr, 
    ivec_t vindex, vec_t mask, const int scale ) noexcept{
#ifdef __AVX2__
    _val = pack_t::gatherm(src, base_addr, vindex, mask, scale);
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<float,8>::gatherm is implemented sequentially
#endif
    _u_vs_t val;
    const _u_vs_t _src = { src };
    const _u_ivs_t idx = { vindex };
    const int _mask = pack_t::movemask( mask );
    for (size_t i = 0; i < NPACK; i++){
        val.s[i] = ( _mask & ( 1 << int(i) ) ) ? 
            *(const scal_t *)((const void *)base_addr + idx.s[i]*scale) : 
            _src.s[i];
    }
    _val = val.f;
#endif
    return *this;
}
inline const Vec<float,8> & 
Vec<float,8>::store( addr_t mem_addr ) const noexcept{
    pack_t::store(mem_addr._addr, _val);
    return *this;
}
inline const Vec<float,8> & 
Vec<float,8>::storem( addr_t mem_addr, ivec_t mask ) const noexcept{
    pack_t::storem(mem_addr._addr, mask, _val);
    return *this;
}
inline const Vec<float,8> & 
Vec<float,8>::storeu( addr_t mem_addr ) const noexcept{
    pack_t::storeu(mem_addr._addr, _val);
    return *this;
}
inline const Vec<float,8> & 
Vec<float,8>::stream( addr_t mem_addr ) const noexcept{
    pack_t::stream(mem_addr._addr, _val);
    return *this;
}
inline Vec<float,8> & 
Vec<float,8>::store( addr_t mem_addr ) noexcept{
    pack_t::store(mem_addr._addr, _val);
    return *this;
}
inline Vec<float,8> & 
Vec<float,8>::storem( addr_t mem_addr, ivec_t mask ) noexcept{
    pack_t::storem(mem_addr._addr, mask, _val);
    return *this;
}
inline Vec<float,8> & 
Vec<float,8>::storeu( addr_t mem_addr ) noexcept{
    pack_t::storeu(mem_addr._addr, _val);
    return *this;
}
inline Vec<float,8> & 
Vec<float,8>::stream( addr_t mem_addr ) noexcept{
    pack_t::stream(mem_addr._addr, _val);
    return *this;
}
inline const Vec<float,8> & Vec<float,8>::scatter(void *base_addr, ivec_t vindex, 
    int scale) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatter( base_addr, vindex, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX512F + AVX512VL not enabled. HIPP::SIMD::Vec<float,8>::scatter is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    const _u_ivs_t idx = {vindex};
    for(size_t i=0; i<NPACK; ++i){
        *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
    }
#endif
    return *this;    
}
inline const Vec<float,8> & 
Vec<float,8>::scatterm(void *base_addr, mask8_t k, ivec_t vindex, 
    int scale) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatterm( base_addr, k, vindex, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX512F + AVX512VL not enabled. HIPP::SIMD::Vec<float,8>::scatterm is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    const _u_ivs_t idx = {vindex};
    for(size_t i=0; i<NPACK; ++i){
        if( k & ( (mask8_t)1 << (mask8_t)i ) )
            *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
    }
#endif
    return *this;
}
inline Vec<float,8> & Vec<float,8>::scatter(void *base_addr, ivec_t vindex, 
    int scale) noexcept{
    return (Vec &)((const Vec *)this)->scatter(base_addr, vindex, scale);
}
inline Vec<float,8> & Vec<float,8>::scatterm(void *base_addr, mask8_t k, ivec_t vindex, 
    int scale) noexcept{
    return (Vec &)((const Vec *)this)->scatterm(base_addr, k, vindex, scale);
}

inline Vec<float,8>::scal_t 
Vec<float,8>::to_scal( )const noexcept{
    return pack_t::to_scal( _val );
}

inline auto Vec<float,8>::from_si(const IntVec &a) noexcept -> Vec & {
    _val = pack_t::from_si(a.val());
    return *this;
}

inline auto Vec<float,8>::to_si()const noexcept -> IntVec {
    return pack_t::to_si(_val);
}

inline int Vec<float,8>::movemask( ) const noexcept{
    return pack_t::movemask( _val );
}
inline Vec<float,8> Vec<float,8>::movehdup( ) const noexcept{
    return pack_t::movehdup( _val );
}
inline Vec<float,8> Vec<float,8>::moveldup( ) const noexcept{
    return pack_t::moveldup( _val );
}
inline Vec<float, 8> & Vec<float, 8>::set( scal_t e7, scal_t e6, scal_t e5, scal_t e4, 
    scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept{
    _val = pack_t::set( e7,e6,e5,e4,e3,e2,e1,e0 );
    return *this;
}
inline Vec<float, 8> & Vec<float, 8>::set1( scal_t a ) noexcept{
    _val = pack_t::set1(a);
    return *this;
}
inline Vec<float, 8> & Vec<float, 8>::set1( vec_hc_t a ) noexcept{
#if defined (__AVX2__)
    _val = pack_t::set1(a);
#elif defined (__SSE__)
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<float,8>::set1(vec_hc_t) is implemented by SSE
#endif
    set1( _mm_cvtss_f32(a) );
#else 
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 or SSE not enabled. HIPP::SIMD::Vec<float,8>::set1(vec_hc_t) is implemented sequentially
#endif
    union { vec_hc_t f; scal_t s[NPACK/2]; } val = {a};
    set1( val.s[0] );
#endif
    return *this;
}
inline Vec<float, 8> & Vec<float, 8>::set() noexcept{
    _val = pack_t::set();
    return *this;
}
inline Vec<float, 8> & Vec<float, 8>::setzero() noexcept{
    _val = pack_t::setzero();
    return *this;
}
inline Vec<float, 8> & Vec<float, 8>::undefined() noexcept{
    _val = pack_t::undefined();
    return *this;
}

inline Vec<float,8> operator+( const Vec<float,8> &a, const Vec<float,8> &b) noexcept{
    return Vec<float,8>::pack_t::add(a._val, b._val);
}
inline Vec<float,8> operator-( const Vec<float,8> &a, const Vec<float,8> &b) noexcept{
    return Vec<float,8>::pack_t::sub(a._val, b._val);
}
inline Vec<float,8> operator*( const Vec<float,8> &a, const Vec<float,8> &b) noexcept{
    return Vec<float,8>::pack_t::mul(a._val, b._val);
}
inline Vec<float,8> operator/( const Vec<float,8> &a, const Vec<float,8> &b) noexcept{
    return Vec<float,8>::pack_t::div(a._val, b._val);
}
inline Vec<float,8> Vec<float,8>::operator++(int) noexcept{
    vec_t tempval = _val;
    _val = pack_t::add( _val, pack_t::set1(1.0) );
    return tempval;
}
inline Vec<float,8> & Vec<float,8>::operator++() noexcept{
    _val = pack_t::add( _val, pack_t::set1(1.0) );
    return *this;
}
inline Vec<float,8> Vec<float,8>::operator--(int) noexcept{
    vec_t tempval = _val;
    _val = pack_t::sub( _val, pack_t::set1(1.0) );
    return tempval;
}
inline Vec<float,8> & Vec<float,8>::operator--() noexcept{
    _val = pack_t::sub( _val, pack_t::set1(1.0) );
    return *this;
}
inline Vec<float,8> & Vec<float,8>::operator+=(const Vec &a) noexcept{
    _val = pack_t::add( _val, a._val );
    return *this;
}
inline Vec<float,8> & Vec<float,8>::operator-=(const Vec &a) noexcept{
    _val = pack_t::sub(_val, a._val);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::operator*=(const Vec &a) noexcept{
    _val = pack_t::mul(_val, a._val);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::operator/=(const Vec &a) noexcept{
    _val = pack_t::div(_val, a._val);
    return *this;
}
inline Vec<float,8> Vec<float,8>::hadd( const Vec &a ) const noexcept{
    return pack_t::hadd(_val, a._val);
}
inline Vec<float,8> Vec<float,8>::hsub( const Vec &a ) const noexcept{
    return pack_t::hsub(_val, a._val);
}
inline Vec<float,8> operator&( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::and_(a._val, b._val);
}
inline Vec<float,8> Vec<float,8>::andnot( const Vec &a ) const noexcept{
    return pack_t::andnot(_val, a._val);
}
inline Vec<float,8> operator|( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::or_(a._val, b._val);
}
inline Vec<float,8> Vec<float,8>::operator~()const noexcept{
    return pack_t::xor_(_val, pack_t::eq(_val, _val) );
}
inline Vec<float,8> operator^( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::xor_(a._val, b._val);
}
inline Vec<float,8> & Vec<float,8>::operator&=( const Vec &a ) noexcept{
    _val = pack_t::and_(_val, a._val);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::operator|=( const Vec &a ) noexcept{
    _val = pack_t::or_(_val, a._val);
    return *this;
}
inline Vec<float,8> & Vec<float,8>::operator^=( const Vec &a ) noexcept{
    _val = pack_t::xor_(_val, a._val);
    return *this;
}
inline Vec<float,8> operator==( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::eq( a._val, b._val );
}
inline Vec<float,8> operator!=( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::neq( a._val, b._val );
}
inline Vec<float,8> operator<( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::lt( a._val, b._val );
}
inline Vec<float,8> operator<=( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::le( a._val, b._val );
}
inline Vec<float,8> operator>( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::gt( a._val, b._val );
}
inline Vec<float,8> operator>=( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::ge( a._val, b._val );
}
inline Vec<float,8> Vec<float,8>::blend( const Vec &a, 
    const int imm8 ) const noexcept{
    return pack_t::blend( _val, a._val, imm8 );
}
inline Vec<float,8> Vec<float,8>::blend( const Vec &a, 
    const Vec &mask ) const noexcept{
    return pack_t::blend(_val, a._val, mask._val);
}
inline Vec<float,8> Vec<float,8>::rcp() const noexcept{
    return pack_t::rcp(_val);
}
inline Vec<float,8> Vec<float,8>::sqrt() const noexcept{
    return pack_t::sqrt(_val);
}
inline Vec<float,8> Vec<float,8>::rsqrt() const noexcept{
    return pack_t::rsqrt(_val);
}
inline Vec<float,8> Vec<float,8>::ceil() const noexcept{
    return pack_t::ceil(_val);
}
inline Vec<float,8> Vec<float,8>::floor() const noexcept{
    return pack_t::floor(_val);
}
inline Vec<float,8> Vec<float,8>::round( const int rounding ) const noexcept{
    return pack_t::round(_val, rounding);
}
inline Vec<float,8> Vec<float,8>::max( const Vec &a ) const noexcept{
    return pack_t::max(_val, a._val);
}
inline Vec<float,8> Vec<float,8>::min( const Vec &a ) const noexcept{
    return pack_t::min(_val, a._val);
}
inline Vec<float,8> Vec<float,8>::log2_fast( ) const noexcept{
    const _u_viv_t vx = {_val};
    const _u_ivv_t mx = { (vx.i & _mm256_set1_epi32(0x007FFFFF)) | 
        _mm256_set1_epi32(0x3f000000)  };
    return Vec(1.1920928955078125e-7f) * _mm256_cvtepi32_ps( vx.i )
        - Vec( 124.22551499f ) - Vec( 1.498030302f ) * mx.f 
        - Vec( 1.72587999f ) / ( Vec(0.3520887068f) + mx.f );
}
inline Vec<float,8> Vec<float,8>::log_fast( ) const noexcept{
    return Vec( 0.69314718f )*log2_fast();
}
inline Vec<float,8> Vec<float,8>::log10_fast( ) const noexcept{
    return Vec( 0.301029995663981f )*log2_fast();
}
inline Vec<float,8> Vec<float,8>::log2_faster( ) const noexcept{
    const _u_viv_t vx = {_val};
    return Vec(1.1920928955078125e-7f) * pack_t::from_ivec(vx.i)
        - Vec( 126.94269504f );
}
inline Vec<float,8> Vec<float,8>::log_faster( ) const noexcept{
    const _u_viv_t vx = {_val};
    return Vec(8.2629582881927490e-8f) * pack_t::from_ivec(vx.i)
        - Vec( 87.989971088f );
}
inline Vec<float,8> Vec<float,8>::log10_faster( ) const noexcept{
    const _u_viv_t vx = {_val};
    return Vec(3.58855719165780e-8f) * pack_t::from_ivec(vx.i)
        - Vec( 38.2135589374653f );
}
inline Vec<float,8> Vec<float,8>::pow2_fast() const noexcept{
    Vec v0 = pack_t::setzero(), v1 = pack_t::set1(1.0f),
        v126 = pack_t::set1(-126.0);
    Vec is_offset = (*this) < v0,
        is_clipp = (*this) < v126;
    Vec offset = v0.blend( v1._val, is_offset ),
        clipp = blend( v126._val, is_clipp ),
        w = clipp.round( (int)Mode::FROUND_ZERO );
    Vec z = clipp - w + offset;
    Vec ans = (clipp + Vec(121.2740575f) + Vec(27.7280233f) / 
                (Vec(4.84252568f) - z) - Vec(1.49012907f) * z) *
                Vec(float(1 << 23));
    _u_ivv_t v = { pack_t::tot_ivec( ans._val ) };
    return v.f;
}
inline Vec<float,8> Vec<float,8>::exp_fast() const noexcept{
    return (Vec(1.442695040f) * (*this)).pow2_fast();
}
inline Vec<float,8> Vec<float,8>::pow10_fast() const noexcept{
    return (Vec(3.32192809488736f) * (*this)).pow2_fast();
}
inline Vec<float,8> Vec<float,8>::pow2_faster() const noexcept{
    Vec v126(-126.0),
        is_clipp = (*this) < v126,
        clipp = blend( v126._val, is_clipp._val );
    Vec ans = Vec(float(1 << 23)) * (clipp + Vec(126.94269504f));
    _u_ivv_t v = { pack_t::tot_ivec( ans._val ) };
    return v.f;
}
inline Vec<float,8> Vec<float,8>::exp_faster() const noexcept{
    return (Vec(1.442695040f) * (*this)).pow2_faster();
}
inline Vec<float,8> Vec<float,8>::pow10_faster() const noexcept{
    return (Vec(3.32192809488736f) * (*this)).pow2_faster();
}

#endif  // __AVX__

} // namespace SIMD
} // namespace HIPP

#endif	//_HIPPSIMD_VECS8_H_