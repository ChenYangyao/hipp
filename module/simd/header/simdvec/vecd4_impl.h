#ifndef _HIPPSIMD_VECD4_IMPL_H_
#define _HIPPSIMD_VECD4_IMPL_H_
#include "vec_impl_base.h"
namespace HIPP::SIMD {
#ifdef __AVX__

inline Vec<double,4>::Vec( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept 
    :_val( pack_t::set(e3,e2,e1,e0) ){ }
inline Vec<double,4>::Vec( scal_t a ) noexcept :_val(pack_t::set1(a)){ }
inline Vec<double,4>::Vec( caddr_t mem_addr ) noexcept 
    :_val(pack_t::load(mem_addr._addr)){ }
inline Vec<double,4>::Vec( const vec_t &a ) noexcept :_val(a){ }
inline Vec<double,4>::Vec( const scal_t *base_addr, 
    const IntVec &vindex, const int scale ) noexcept 
    :_val(pack_t::gather(base_addr, vindex._val, scale)){ }
inline Vec<double,4>::Vec( const Vec &src, const scal_t *base_addr, 
    const IntVec &vindex, const Vec &mask, const int scale) noexcept 
    :_val(pack_t::gatherm(src._val, base_addr, vindex._val, mask._val, scale)){ }
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
    return v.info(os);
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
Vec<double,4>::loadm( caddr_t mem_addr, const IntVec &mask ) noexcept 
{ _val = pack_t::loadm(mem_addr._addr, mask._val); return *this; }

inline Vec<double,4> & Vec<double,4>::load1( const scal_t *mem_addr ) noexcept 
{ _val = pack_t::load1(mem_addr); return *this; }

inline Vec<double,4> & Vec<double,4>::bcast( const scal_t *mem_addr ) noexcept 
{ _val = pack_t::bcast(mem_addr); return *this; }

inline Vec<double,4> & Vec<double,4>::bcast( const vec_hc_t *mem_addr ) noexcept 
{ _val = pack_t::bcast(mem_addr); return *this; }

inline Vec<double,4> & 
Vec<double,4>::gather( const scal_t *base_addr, 
    const IntVec &vindex, const int scale ) noexcept{ 
#ifdef __AVX2__
    _val = pack_t::gather(base_addr, vindex._val, scale);
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::gather is implemented sequentially
#endif
    _u_vs_t val;
    const _u_ivs_t idx = {vindex._val};
    for(size_t i=0; i<NPACK; ++i){
        val.s[i] = *(const scal_t *)((const void *)base_addr + idx.s[i]*scale);
    }
    _val = val.f;
#endif
    return *this; 
}

inline Vec<double,4> & 
Vec<double,4>::gatherm( const Vec & src, const scal_t *base_addr, 
    const IntVec &vindex, const Vec & mask, const int scale ) noexcept { 
#ifdef __AVX2__
    _val = pack_t::gatherm(src._val, base_addr, vindex._val, mask._val, scale); 
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::gatherm is implemented sequentially
#endif
    _u_vs_t val;
    const _u_vs_t _src = {src._val};
    const _u_ivs_t idx = {vindex._val};
    const int _mask = pack_t::movemask(mask._val);
    for(size_t i=0; i<NPACK; ++i){
        val.s[i] = ( _mask & ( 1 << int(i) ) ) ? 
            *(const scal_t *)((const void *)base_addr + idx.s[i]*scale) :
            _src.s[i];
    }
    _val = val.f;
#endif
    return *this; 
}

inline Vec<double,4> & Vec<double,4>::gather( 
    const scal_t *base_addr, const IntVecHP &vindex, 
    const int scale ) noexcept{
#ifdef __AVX2__
    _val = pack_t::gather_idxhp( base_addr, vindex._val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::gather_idxhp is implemented sequentially
#endif
    _u_vs_t val;
    union u_t { IntVecHP i; iscal_hp_t s[NPACK];  ~u_t(){} } const idx = {vindex._val};
    for(size_t i=0; i<NPACK; ++i){
        val.s[i] = *(const scal_t *)((const void *)base_addr + idx.s[i]*scale);
    }
    _val = val.f;
#endif 
    return *this;
}
inline Vec<double,4> & Vec<double,4>::gatherm( const Vec &src, 
    const scal_t *base_addr, const IntVecHP &vindex, 
    const Vec &mask, const int scale ) noexcept{
#ifdef __AVX2__
    _val = pack_t::gatherm_idxhp(src._val, base_addr, vindex._val, mask._val, scale); 
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<double,4>::gatherm_idxhp is implemented sequentially
#endif
    _u_vs_t val;
    const _u_vs_t _src = {src._val};
    union u_t { IntVecHP i; iscal_hp_t s[NPACK];  ~u_t(){}} const idx = {vindex._val};
    const int _mask = pack_t::movemask(mask._val);
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
Vec<double,4>::storem( addr_t mem_addr, const IntVec &mask ) const noexcept 
{ pack_t::storem(mem_addr._addr, mask._val, _val); return *this; }

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
Vec<double,4>::storem( addr_t mem_addr, const IntVec &mask ) noexcept 
{ pack_t::storem(mem_addr._addr, mask._val, _val); return *this; }

inline Vec<double,4> & 
Vec<double,4>::storeu( addr_t mem_addr ) noexcept 
{ pack_t::storeu(mem_addr._addr, _val); return *this; }

inline Vec<double,4> & 
Vec<double,4>::stream( addr_t mem_addr ) noexcept 
{ pack_t::stream(mem_addr._addr, _val); return *this; }

inline const Vec<double, 4> & 
Vec<double,4>::scatter( void *base_addr, 
    const IntVec &vindex, int scale ) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatter( base_addr, vindex._val, _val, scale );
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
    const IntVec &vindex, int scale ) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatterm( base_addr, k, vindex._val, _val, scale );
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
Vec<double,4>::scatter( void *base_addr, 
    const IntVecHP &vindex, int scale ) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatter_idxhp( base_addr, vindex._val, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX512F + AVX512VL not enabled. HIPP::SIMD::Vec<double,4>::scatter_idxhp is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    union u_t { IntVecHP i; iscal_hp_t s[NPACK]; ~u_t(){} } const idx = {vindex};
    for(size_t i=0; i<NPACK; ++i)
        *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
#endif
    return *this;
}

inline const Vec<double, 4> & 
Vec<double,4>::scatterm( void *base_addr, mask8_t k,
    const IntVecHP &vindex, int scale ) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatterm_idxhp( base_addr, k, vindex._val, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX512F + AVX512VL not enabled. HIPP::SIMD::Vec<double,4>::scatterm_idxhp is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    union u_t { IntVecHP i; iscal_hp_t s[NPACK];  ~u_t(){} } const idx = {vindex};
    for(size_t i=0; i<NPACK; ++i)
        if( k & ( (mask8_t)1 << (mask8_t)i ) )
            *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
#endif
    return *this;
}

inline Vec<double, 4> & 
Vec<double,4>::scatter( void *base_addr, 
    const IntVec &vindex, int scale ) noexcept{
    return (Vec &)((const Vec *)this)->scatter(base_addr, vindex._val, scale);
}

inline Vec<double, 4> & 
Vec<double,4>::scatterm( void *base_addr, mask8_t k,
    const IntVec &vindex, int scale ) noexcept{
    return (Vec &)((const Vec *)this)->scatterm(base_addr, k, vindex._val, scale);
}

inline Vec<double, 4> & 
Vec<double,4>::scatter( void *base_addr, 
    const IntVecHP &vindex, int scale ) noexcept{
    return (Vec &)((const Vec *)this)->scatter(base_addr, vindex, scale);    
}

inline Vec<double, 4> & 
Vec<double,4>::scatterm( void *base_addr, mask8_t k,
    const IntVecHP &vindex, int scale ) noexcept{
    return (Vec &)((const Vec *)this)
        ->scatterm(base_addr, k, vindex, scale);
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

inline
auto Vec<double,4>::from_si(const IntVec &a) noexcept -> Vec & {
    _val = pack_t::from_si(a.val());
    return *this;
}

inline
auto Vec<double,4>::to_si()const noexcept -> IntVec {
    return pack_t::to_si(_val);
}

inline
auto Vec<double,4>::to_ivec_hp() const noexcept -> IntVecHP {
    return pack_t::to_ivec_hp(_val);
}
inline
auto Vec<double,4>::tot_ivec_hp() const noexcept -> IntVecHP {
    return pack_t::tot_ivec_hp(_val);
}
inline
auto Vec<double,4>::to_i32vec() const noexcept -> IntVecHP {
    return to_ivec_hp();
}
inline
auto Vec<double,4>::tot_i32vec() const noexcept -> IntVecHP {
    return tot_ivec_hp();
}
inline
auto Vec<double,4>::to_vec_hc() const noexcept -> VecHC {
    return pack_t::to_vec_hc(_val);
}

inline
auto Vec<double,4>::extract_hc(const int imm8) const noexcept -> VecHC { 
    return pack_t::extract_hc(_val, imm8); 
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
    union u_t { vec_hc_t f; scal_t s[NPACK/2] s;  ~u_t(){}} val = {a};
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

inline Vec<double,4> operator+( const Vec<double, 4> &a, const Vec<double, 4> &b ) noexcept{
    return Vec<double,4>::pack_t::add(a._val, b._val);
}
inline Vec<double,4> operator-( const Vec<double, 4> &a, const Vec<double, 4> &b ) noexcept{
    return Vec<double,4>::pack_t::sub(a._val, b._val);
}
inline Vec<double,4> operator*( const Vec<double, 4> &a, const Vec<double, 4> &b ) noexcept{
    return Vec<double,4>::pack_t::mul(a._val, b._val);
}
inline Vec<double,4> operator/( const Vec<double, 4> &a, const Vec<double, 4> &b ) noexcept{
    return Vec<double,4>::pack_t::div(a._val, b._val);
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
inline Vec<double,4> operator&( const Vec<double,4> &a, const Vec<double,4> &b ) noexcept{
    return Vec<double,4>::pack_t::and_(a._val, b._val);
}
inline Vec<double,4> Vec<double,4>::andnot( const Vec &a ) const noexcept{
    return pack_t::andnot(_val, a._val);
}
inline Vec<double,4> operator|( const Vec<double,4> &a, const Vec<double,4> &b ) noexcept{
    return Vec<double,4>::pack_t::or_(a._val, b._val);
}
inline Vec<double,4> Vec<double,4>::operator~()const noexcept{
    return pack_t::xor_(_val, pack_t::eq(_val, _val) );
}
inline Vec<double,4> operator^( const Vec<double,4> &a, const Vec<double,4> &b ) noexcept{
    return Vec<double,4>::pack_t::xor_(a._val, b._val);
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
inline Vec<double,4> operator==( const Vec<double,4> &a, const Vec<double,4> &b ) noexcept{
    return Vec<double,4>::pack_t::eq( a._val, b._val );
}
inline Vec<double,4> operator!=( const Vec<double,4> &a, const Vec<double,4> &b ) noexcept{
    return Vec<double,4>::pack_t::neq( a._val, b._val );
}
inline Vec<double,4> operator<( const Vec<double,4> &a, const Vec<double,4> &b ) noexcept{
    return Vec<double,4>::pack_t::lt( a._val, b._val );
}
inline Vec<double,4> operator<=( const Vec<double,4> &a, const Vec<double,4> &b ) noexcept{
    return Vec<double,4>::pack_t::le( a._val, b._val );
}
inline Vec<double,4> operator>( const Vec<double,4> &a, const Vec<double,4> &b ) noexcept{
    return Vec<double,4>::pack_t::gt( a._val, b._val );
}
inline Vec<double,4> operator>=( const Vec<double,4> &a, const Vec<double,4> &b ) noexcept{
    return Vec<double,4>::pack_t::ge( a._val, b._val );
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

inline   
auto Vec<double,4>::sum_all() const noexcept -> scal_t {
    VecHC vlow = to_vec_hc(), vhi = extract_hc(1);
    vlow += vhi;
    VecHC hi64 = vlow.unpackhi(vlow);
    return vlow.adds(hi64).to_scal();
}

#endif // __AVX__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECD4_IMPL_H_