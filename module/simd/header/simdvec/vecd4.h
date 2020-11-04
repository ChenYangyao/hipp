#ifndef _HIPPSIMD_VECD4_H_
#define _HIPPSIMD_VECD4_H_
#include "vecbase.h"
#include "../simdpacked/pd256.h"
#include "math.h"
#include "hippcntl.h"

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
    typedef _pd256_helper::AddrAligned addr_t;
    typedef _pd256_helper::CAddrAligned caddr_t;
    
    Vec() noexcept{}
    Vec( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    explicit Vec( scal_t a ) noexcept;
    explicit Vec( caddr_t mem_addr ) noexcept;
    Vec( const vec_t &a ) noexcept;
    Vec( const scal_t *base_addr, 
        ivec_t vindex, const int scale=SCALSIZE ) noexcept;
    Vec( vec_t src, const scal_t *base_addr, 
        ivec_t vindex, vec_t mask, const int scale=SCALSIZE ) noexcept;
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
    Vec & loadm( caddr_t mem_addr, ivec_t mask ) noexcept;
    Vec & load1( const scal_t *mem_addr ) noexcept;
    Vec & bcast( const scal_t *mem_addr ) noexcept;
    Vec & bcast( const vec_hc_t *mem_addr ) noexcept;
    Vec & gather( const scal_t *base_addr, ivec_t vindex, 
        const int scale=SCALSIZE ) noexcept;
    Vec & gatherm( vec_t src, const scal_t *base_addr, ivec_t vindex, 
        vec_t mask, const int scale=SCALSIZE ) noexcept;
    Vec & gather_idxhp( const scal_t *base_addr, ivec_hp_t vindex, 
        const int scale=SCALSIZE ) noexcept;
    Vec & gatherm_idxhp( vec_t src, const scal_t *base_addr, ivec_hp_t vindex, 
        vec_t mask, const int scale=SCALSIZE ) noexcept;
    const Vec & store( addr_t mem_addr ) const noexcept;
    const Vec & storem( addr_t mem_addr, ivec_t mask ) const noexcept;
    const Vec & storeu( addr_t mem_addr ) const noexcept;
    const Vec & stream( addr_t mem_addr ) const noexcept;
    Vec & store( addr_t mem_addr ) noexcept;
    Vec & storem( addr_t mem_addr, ivec_t mask ) noexcept;
    Vec & storeu( addr_t mem_addr ) noexcept;
    Vec & stream( addr_t mem_addr ) noexcept;
    const Vec & scatter( void *base_addr, 
        ivec_t vindex, int scale=SCALSIZE ) const noexcept;
    const Vec & scatterm( void *base_addr, mask8_t k,
        ivec_t vindex, int scale=SCALSIZE ) const noexcept;
    const Vec & scatter_idxhp( void *base_addr, 
        ivec_hp_t vindex, int scale=SCALSIZE ) const noexcept;
    const Vec & scatterm_idxhp( void *base_addr, mask8_t k,
        ivec_hp_t vindex, int scale=SCALSIZE ) const noexcept;
    Vec & scatter( void *base_addr, 
        ivec_t vindex, int scale=SCALSIZE ) noexcept;
    Vec & scatterm( void *base_addr, mask8_t k,
        ivec_t vindex, int scale=SCALSIZE ) noexcept;
    Vec & scatter_idxhp( void *base_addr, 
        ivec_hp_t vindex, int scale=SCALSIZE ) noexcept;
    Vec & scatterm_idxhp( void *base_addr, mask8_t k,
        ivec_hp_t vindex, int scale=SCALSIZE ) noexcept;

    
    scal_t to_scal( )const noexcept;

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
    Vec operator+( const Vec &a ) const noexcept;
    Vec operator-( const Vec &a ) const noexcept;
    Vec operator*( const Vec &a ) const noexcept;
    Vec operator/( const Vec &a ) const noexcept;
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

    Vec operator&( const Vec &a ) const noexcept;
    Vec andnot( const Vec &a ) const noexcept;
    Vec operator|( const Vec &a ) const noexcept;
    Vec operator~()const noexcept;
    Vec operator^( const Vec &a ) const noexcept;
    Vec & operator&=( const Vec &a ) noexcept;
    Vec & operator|=( const Vec &a ) noexcept;
    Vec & operator^=( const Vec &a ) noexcept;
    
    Vec operator==( const Vec &a ) const noexcept;
    Vec operator!=( const Vec &a ) const noexcept;
    Vec operator<( const Vec &a ) const noexcept;
    Vec operator<=( const Vec &a ) const noexcept;
    Vec operator>( const Vec &a ) const noexcept;
    Vec operator>=( const Vec &a ) const noexcept;

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
    union _u_ivv_t{ ivec_t i; vec_t f; };
    union _u_viv_t{ vec_t f; ivec_t i; };
    union _u_vs_t{ vec_t f; scal_t s[NPACK]; };
    union _u_sv_t{ scal_t s[NPACK]; vec_t f; };
    union _u_ivs_t{ ivec_t i; iscal_t s[NPACK]; };
    union _u_siv_t{ iscal_t s[NPACK]; ivec_t i; };
};

inline Vec<double,4>::Vec( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept 
    :_val( pack_t::set(e3,e2,e1,e0) ){ }
inline Vec<double,4>::Vec( scal_t a ) noexcept :_val(pack_t::set1(a)){ }
inline Vec<double,4>::Vec( caddr_t mem_addr ) noexcept 
    :_val(pack_t::load(mem_addr._addr)){ }
inline Vec<double,4>::Vec( const vec_t &a ) noexcept :_val(a){ }
inline Vec<double,4>::Vec( const scal_t *base_addr, 
    pack_t::ivec_t vindex, const int scale ) noexcept 
    :_val(pack_t::gather(base_addr, vindex, scale)){ }
inline Vec<double,4>::Vec( vec_t src, const scal_t *base_addr, 
    pack_t::ivec_t vindex, vec_t mask, const int scale) noexcept 
    :_val(pack_t::gatherm(src, base_addr, vindex, mask, scale)){ }
inline Vec<double,4>::Vec( const Vec &a )noexcept :_val(a._val) { }
inline Vec<double,4>::Vec( Vec &&a )noexcept :_val(a._val) { }
inline Vec<double,4> & Vec<double,4>::operator=( const Vec &a ) noexcept{
    _val = a._val; return *this;
}
inline Vec<double,4> & Vec<double,4>::operator=( Vec &&a ) noexcept{
    _val = a._val; return *this;
}
inline ostream & Vec<double,4>::info( ostream &os, int fmt_cntl ) const{
    prt( os,  "HIPP::SIMD::Vec<double,4>("); 
    const scal_t *p = (const scal_t *)&_val;
    prt_a(os, p, p+NPACK) << ")";
    if( fmt_cntl >= 1 ) os << endl;
    return os;
}
inline ostream & operator<<( ostream &os, const Vec<double,4> &v ){
    return v.info();
}
inline const Vec<double,4>::vec_t & 
Vec<double,4>::val() const noexcept{ return _val; }

inline Vec<double,4>::vec_t & 
Vec<double,4>::val() noexcept{ return _val; }

inline const Vec<double,4>::scal_t & Vec<double,4>::operator[]( size_t n )const noexcept{
    return ( (const scal_t *)&_val )[n];
}
inline Vec<double,4>::scal_t & Vec<double,4>::operator[]( size_t n ) noexcept{
    return ( (scal_t *)&_val )[n];
}

inline Vec<double,4> & Vec<double,4>::load( caddr_t mem_addr ) noexcept 
{ _val = pack_t::load(mem_addr._addr); return *this; }

inline Vec<double,4> & Vec<double,4>::loadu( caddr_t mem_addr ) noexcept 
{ _val = pack_t::loadu(mem_addr._addr); return *this; }

inline Vec<double,4> & 
Vec<double,4>::loadm( caddr_t mem_addr, ivec_t mask ) noexcept 
{ _val = pack_t::loadm(mem_addr._addr, mask); return *this; }

inline Vec<double,4> & Vec<double,4>::load1( const scal_t *mem_addr ) noexcept 
{ _val = pack_t::load1(mem_addr); return *this; }

inline Vec<double,4> & Vec<double,4>::bcast( const scal_t *mem_addr ) noexcept 
{ _val = pack_t::bcast(mem_addr); return *this; }

inline Vec<double,4> & Vec<double,4>::bcast( const vec_hc_t *mem_addr ) noexcept 
{ _val = pack_t::bcast(mem_addr); return *this; }

inline Vec<double,4> & 
Vec<double,4>::gather( const scal_t *base_addr, 
    ivec_t vindex, const int scale ) noexcept{ 
#ifdef __AVX2__
    _val = pack_t::gather(base_addr, vindex, scale);
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::gather is implemented sequentially
#endif
    _u_vs_t val;
    const _u_ivs_t idx = {vindex};
    for(size_t i=0; i<NPACK; ++i){
        val.s[i] = *(const scal_t *)((const void *)base_addr + idx.s[i]*scale);
    }
    _val = val.f;
#endif
    return *this; 
}

inline Vec<double,4> & 
Vec<double,4>::gatherm( vec_t src, const scal_t *base_addr, 
    ivec_t vindex, vec_t mask, const int scale ) noexcept { 
#ifdef __AVX2__
    _val = pack_t::gatherm(src, base_addr, vindex, mask, scale); 
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::gatherm is implemented sequentially
#endif
    _u_vs_t val;
    const _u_vs_t _src = {src};
    const _u_ivs_t idx = {vindex};
    const int _mask = pack_t::movemask(mask);
    for(size_t i=0; i<NPACK; ++i){
        val.s[i] = ( _mask & ( 1 << int(i) ) ) ? 
            *(const scal_t *)((const void *)base_addr + idx.s[i]*scale) :
            _src.s[i];
    }
    _val = val.f;
#endif
    return *this; 
}

inline Vec<double,4> & Vec<double,4>::gather_idxhp( 
    const scal_t *base_addr, ivec_hp_t vindex, 
    const int scale ) noexcept{
#ifdef __AVX2__
    _val = pack_t::gather_idxhp( base_addr, vindex, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::gather_idxhp is implemented sequentially
#endif
    _u_vs_t val;
    union { ivec_hp_t i; iscal_hp_t s[NPACK]; } const idx = {vindex};
    for(size_t i=0; i<NPACK; ++i){
        val.s[i] = *(const scal_t *)((const void *)base_addr + idx.s[i]*scale);
    }
    _val = val.f;
#endif 
    return *this;
}
inline Vec<double,4> & Vec<double,4>::gatherm_idxhp( vec_t src, 
    const scal_t *base_addr, ivec_hp_t vindex, 
    vec_t mask, const int scale ) noexcept{
#ifdef __AVX2__
    _val = pack_t::gatherm_idxhp(src, base_addr, vindex, mask, scale); 
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::gatherm_idxhp is implemented sequentially
#endif
    _u_vs_t val;
    const _u_vs_t _src = {src};
    union { ivec_hp_t i; iscal_hp_t s[NPACK]; } const idx = {vindex};
    const int _mask = pack_t::movemask(mask);
    for(size_t i=0; i<NPACK; ++i){
        val.s[i] = ( _mask & ( 1 << int(i) ) ) ? 
            *(const scal_t *)((const void *)base_addr + idx.s[i]*scale) :
            _src.s[i];
    }
    _val = val.f;
#endif
    return *this; 
}
inline const Vec<double,4> & 
Vec<double,4>::store( addr_t mem_addr ) const noexcept 
{ pack_t::store(mem_addr._addr, _val); return *this; }

inline const Vec<double,4> & 
Vec<double,4>::storem( addr_t mem_addr, ivec_t mask ) const noexcept 
{ pack_t::storem(mem_addr._addr, mask, _val); return *this; }

inline const Vec<double,4> & 
Vec<double,4>::storeu( addr_t mem_addr ) const noexcept 
{ pack_t::storeu(mem_addr._addr, _val); return *this; }

inline const Vec<double,4> & 
Vec<double,4>::stream( addr_t mem_addr ) const noexcept 
{ pack_t::stream(mem_addr._addr, _val); return *this; }

inline Vec<double,4> & 
Vec<double,4>::store( addr_t mem_addr ) noexcept 
{ pack_t::store(mem_addr._addr, _val); return *this; }

inline Vec<double,4> & 
Vec<double,4>::storem( addr_t mem_addr, ivec_t mask ) noexcept 
{ pack_t::storem(mem_addr._addr, mask, _val); return *this; }

inline Vec<double,4> & 
Vec<double,4>::storeu( addr_t mem_addr ) noexcept 
{ pack_t::storeu(mem_addr._addr, _val); return *this; }

inline Vec<double,4> & 
Vec<double,4>::stream( addr_t mem_addr ) noexcept 
{ pack_t::stream(mem_addr._addr, _val); return *this; }

inline const Vec<double, 4> & 
Vec<double,4>::scatter( void *base_addr, 
    ivec_t vindex, int scale ) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatter( base_addr, vindex, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning HAVX512F + AVX512VL not enabled. PP::SIMD::Vec<double,4>::scatter is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    const _u_ivs_t idx = {vindex};
    for(size_t i=0; i<NPACK; ++i){
        *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
    }
#endif
    return *this;
}

inline const Vec<double, 4> & 
Vec<double,4>::scatterm( void *base_addr, mask8_t k,
    ivec_t vindex, int scale ) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatterm( base_addr, k, vindex, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX512F + AVX512VL not enabled. HIPP::SIMD::Vec<double,4>::scatterm is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    const _u_ivs_t idx = {vindex};
    for(size_t i=0; i<NPACK; ++i)
        if( k & ( (mask8_t)1 << (mask8_t)i ) )
            *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
#endif
    return *this;
}

inline const Vec<double, 4> & 
Vec<double,4>::scatter_idxhp( void *base_addr, 
    ivec_hp_t vindex, int scale ) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatter_idxhp( base_addr, vindex, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX512F + AVX512VL not enabled. HIPP::SIMD::Vec<double,4>::scatter_idxhp is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    union { ivec_hp_t i; iscal_hp_t s[NPACK]; } const idx = {vindex};
    for(size_t i=0; i<NPACK; ++i)
        *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
#endif
    return *this;
}

inline const Vec<double, 4> & 
Vec<double,4>::scatterm_idxhp( void *base_addr, mask8_t k,
    ivec_hp_t vindex, int scale ) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatterm_idxhp( base_addr, k, vindex, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX512F + AVX512VL not enabled. HIPP::SIMD::Vec<double,4>::scatterm_idxhp is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    union { ivec_hp_t i; iscal_hp_t s[NPACK]; } const idx = {vindex};
    for(size_t i=0; i<NPACK; ++i)
        if( k & ( (mask8_t)1 << (mask8_t)i ) )
            *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
#endif
    return *this;
}

inline Vec<double, 4> & 
Vec<double,4>::scatter( void *base_addr, 
    ivec_t vindex, int scale ) noexcept{
    return (Vec &)((const Vec *)this)->scatter(base_addr, vindex, scale);
}

inline Vec<double, 4> & 
Vec<double,4>::scatterm( void *base_addr, mask8_t k,
    ivec_t vindex, int scale ) noexcept{
    return (Vec &)((const Vec *)this)->scatterm(base_addr, k, vindex, scale);
}

inline Vec<double, 4> & 
Vec<double,4>::scatter_idxhp( void *base_addr, 
    ivec_hp_t vindex, int scale ) noexcept{
    return (Vec &)((const Vec *)this)->scatter_idxhp(base_addr, vindex, scale);    
}

inline Vec<double, 4> & 
Vec<double,4>::scatterm_idxhp( void *base_addr, mask8_t k,
    ivec_hp_t vindex, int scale ) noexcept{
    return (Vec &)((const Vec *)this)
        ->scatterm_idxhp(base_addr, k, vindex, scale);
}

inline Vec<double,4>::scal_t 
Vec<double,4>::to_scal( )const noexcept{
#ifdef __AVX2__
    return pack_t::to_scal(_val);
#else 
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::to_scal is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    return val.s[0];
#endif
}

inline int Vec<double,4>::movemask( ) const noexcept 
{ return pack_t::movemask(_val); }

inline Vec<double,4> Vec<double,4>::movedup( ) const noexcept 
{ return pack_t::movedup(_val); }

inline Vec<double,4> & 
Vec<double,4>::set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept 
{ _val = pack_t::set(e3,e2,e1,e0); return *this; }

inline Vec<double,4> & 
Vec<double,4>::set1( scal_t a ) noexcept 
{ _val = pack_t::set1(a); return *this; }

inline Vec<double,4> & 
Vec<double,4>::set1( vec_hc_t a ) noexcept { 
#if defined (__AVX2__)
    _val = pack_t::set1(a); 
#elif defined (__SSE2__)
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::set1(vec_hc_t) is implemented by SSE2
#endif
    set1( _mm_cvtsd_f64(a) )
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 or SSE2 not enabled. HIPP::SIMD::Vec<double,4>::set1(vec_hc_t) is implemented sequentially
#endif
    union { vec_hc_t f; scal_t s[NPACK/2] s; } val = {a};
    set1( val.s[0] );    
#endif
    return *this; 
}

inline Vec<double,4> & 
Vec<double,4>::set() noexcept 
{ _val = pack_t::set(); return *this; }

inline Vec<double,4> & 
Vec<double,4>::setzero() noexcept 
{ _val = pack_t::setzero(); return *this; }

inline Vec<double,4> & 
Vec<double,4>::undefined() noexcept 
{ _val = pack_t::undefined(); return *this; }


inline Vec<double,4> Vec<double,4>::operator+( const Vec &a ) const noexcept{
    return pack_t::add(_val, a._val);
}
inline Vec<double,4> Vec<double,4>::operator-( const Vec &a ) const noexcept{
    return pack_t::sub(_val, a._val);
}
inline Vec<double,4> Vec<double,4>::operator*( const Vec &a ) const noexcept{
    return pack_t::mul(_val, a._val);
}
inline Vec<double,4> Vec<double,4>::operator/( const Vec &a ) const noexcept{
    return pack_t::div(_val, a._val);
}
inline Vec<double,4> Vec<double,4>::operator++(int) noexcept{
    vec_t tempval = _val;
    _val = pack_t::add( _val, pack_t::set1(1.0) );
    return tempval;
}
inline Vec<double,4> & Vec<double,4>::operator++() noexcept{
    _val = pack_t::add( _val, pack_t::set1(1.0) );
    return *this;
}
inline Vec<double,4> Vec<double,4>::operator--(int) noexcept{
    vec_t tempval = _val;
    _val = pack_t::sub( _val, pack_t::set1(1.0) );
    return tempval;
}
inline Vec<double,4> & Vec<double,4>::operator--() noexcept{
    _val = pack_t::sub( _val, pack_t::set1(1.0) );
    return *this;
}
inline Vec<double,4> & Vec<double,4>::operator+=( const Vec &a ) noexcept{
    _val = pack_t::add(_val, a._val);
    return *this;
}
inline Vec<double,4> & Vec<double,4>::operator-=( const Vec &a ) noexcept{
    _val = pack_t::sub(_val, a._val);
    return *this;
}
inline Vec<double,4> & Vec<double,4>::operator*=( const Vec &a ) noexcept{
    _val = pack_t::mul(_val, a._val);
    return *this;
}
inline Vec<double,4> & Vec<double,4>::operator/=( const Vec &a ) noexcept{
    _val = pack_t::div(_val, a._val);
    return *this;
}
inline Vec<double, 4> Vec<double, 4>::hadd( const Vec &a ) const noexcept{
    return pack_t::hadd(_val, a._val);
}
inline Vec<double, 4> Vec<double, 4>::hsub( const Vec &a ) const noexcept{
    return pack_t::hsub(_val, a._val);
}
inline Vec<double,4> Vec<double,4>::operator&( const Vec &a ) const noexcept{
    return pack_t::and_(_val, a._val);
}
inline Vec<double,4> Vec<double,4>::andnot( const Vec &a ) const noexcept{
    return pack_t::andnot(_val, a._val);
}
inline Vec<double,4> Vec<double,4>::operator|( const Vec &a ) const noexcept{
    return pack_t::or_(_val, a._val);
}
inline Vec<double,4> Vec<double,4>::operator~()const noexcept{
    return pack_t::xor_(_val, pack_t::eq(_val, _val) );
}
inline Vec<double,4> Vec<double,4>::operator^( const Vec &a ) const noexcept{
    return pack_t::xor_(_val, a._val);
}
Vec<double,4> & Vec<double,4>::operator&=( const Vec &a ) noexcept{
    _val = pack_t::and_(_val, a._val);
    return *this;
}
Vec<double,4> & Vec<double,4>::operator|=( const Vec &a ) noexcept{
    _val = pack_t::or_(_val, a._val);
    return *this;
}
Vec<double,4> & Vec<double,4>::operator^=( const Vec &a ) noexcept{
    _val = pack_t::xor_(_val, a._val);
    return *this;
}
inline Vec<double,4> Vec<double,4>::operator==( const Vec &a ) const noexcept{
    return pack_t::eq( _val, a._val );
}
inline Vec<double,4> Vec<double,4>::operator!=( const Vec &a ) const noexcept{
    return pack_t::neq( _val, a._val );
}
inline Vec<double,4> Vec<double,4>::operator<( const Vec &a ) const noexcept{
    return pack_t::lt( _val, a._val );
}
inline Vec<double,4> Vec<double,4>::operator<=( const Vec &a ) const noexcept{
    return pack_t::le( _val, a._val );
}
inline Vec<double,4> Vec<double,4>::operator>( const Vec &a ) const noexcept{
    return pack_t::gt( _val, a._val );
}
inline Vec<double,4> Vec<double,4>::operator>=( const Vec &a ) const noexcept{
    return pack_t::ge( _val, a._val );
}

inline Vec<double,4> 
Vec<double,4>::blend( const Vec &a, const int imm8 ) const noexcept{
    return pack_t::blend( _val, a._val, imm8 );
}

inline Vec<double,4> 
Vec<double,4>::blend( const Vec &a, const Vec &mask ) const noexcept{
    return pack_t::blend( _val, a._val, mask._val );
}

inline Vec<double,4> 
Vec<double,4>::sqrt() const noexcept 
{ return pack_t::sqrt(_val); }

inline Vec<double,4> 
Vec<double,4>::ceil() const noexcept 
{ return pack_t::ceil(_val); }

inline Vec<double,4> 
Vec<double,4>::floor() const noexcept 
{ return pack_t::floor(_val); }

inline Vec<double,4> 
Vec<double,4>::round( const int rounding ) const noexcept 
{ return pack_t::round(_val, rounding); }

inline Vec<double,4> 
Vec<double,4>::max( const Vec &a ) const noexcept 
{ return pack_t::max(_val, a._val); }

inline Vec<double,4> 
Vec<double,4>::min( const Vec &a ) const noexcept 
{ return pack_t::min(_val, a._val); }

#define _HIPPSIMD_ARITH_OP_BIN(op)\
    vec_t ans;\
    const scal_t *src = (const scal_t *)&_val;\
    scal_t *dst = (scal_t *)&ans;\
    for(int i=0; i<NPACK; ++i){\
        dst[i] = ::op(src[i]); \      
    }\
    return ans;

inline Vec<double,4>  
Vec<double,4>::sin( ) const noexcept{
#ifndef __FAST_MATH__
#ifdef _HIPPSIMD_WSEQ
#warning FAST_MATH not enabled. HIPP::SIMD::Vec<double,4>::sin may be not vectorized.
#endif
#endif
    _HIPPSIMD_ARITH_OP_BIN(sin)
}
inline Vec<double,4>  
Vec<double,4>::cos( ) const noexcept{
#ifndef __FAST_MATH__
#ifdef _HIPPSIMD_WSEQ
#warning FAST_MATH not enabled. HIPP::SIMD::Vec<double,4>::cos may be not vectorized.
#endif
#endif
    _HIPPSIMD_ARITH_OP_BIN(cos)
}
inline Vec<double,4>  
Vec<double,4>::log( ) const noexcept{
#ifndef __FAST_MATH__
#ifdef _HIPPSIMD_WSEQ
#warning FAST_MATH not enabled. HIPP::SIMD::Vec<double,4>::log may be not vectorized.
#endif
#endif
    _HIPPSIMD_ARITH_OP_BIN(log)
}
inline Vec<double,4>  
Vec<double,4>::exp( ) const noexcept{
#ifndef __FAST_MATH__
#ifdef _HIPPSIMD_WSEQ
#warning FAST_MATH not enabled. HIPP::SIMD::Vec<double,4>::exp may be not vectorized.
#endif
#endif
    _HIPPSIMD_ARITH_OP_BIN(exp)
}
inline Vec<double,4>  
Vec<double,4>::pow( const Vec &a ) const noexcept{
#ifndef __FAST_MATH__
#ifdef _HIPPSIMD_WSEQ
#warning FAST_MATH not enabled. HIPP::SIMD::Vec<double,4>::pow may be not vectorized.
#endif
#endif
    vec_t ans;
    const scal_t *src = (const scal_t *)&_val, *ind = (const scal_t *)&a._val;
    scal_t *dst = (scal_t *)&ans;
    for(int i=0; i<NPACK; ++i){
        dst[i] = ::pow(src[i], ind[i]);
    }
    return ans;
}
#undef _HIPPSIMD_ARITH_OP_BIN

#endif //__AVX__

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_VECD4_H_