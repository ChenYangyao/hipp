#ifndef _HIPPSIMD_VECS8_IMPL_H_
#define _HIPPSIMD_VECS8_IMPL_H_
#include "vec_impl_base.h"
namespace HIPP::SIMD {
#ifdef __AVX__

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
    const IntVec &vindex, const int scale ) noexcept{
#ifdef __AVX2__
    _val = pack_t::gather(base_addr, vindex.val(), scale);
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<float,8>::gather is implemented sequentially
#endif
    _u_vs_t val;
    const _u_ivs_t idx = { vindex.val() };
    for (size_t i = 0; i < NPACK; i++){
        val.s[i] = *(const scal_t *)((const void *)base_addr + idx.s[i]*scale);
    }
    _val = val.f;
#endif
    return *this;
}
inline Vec<float,8> & Vec<float,8>::gatherm( const Vec &src, const scal_t *base_addr, 
    const IntVec &vindex, const Vec &mask, const int scale ) noexcept{
#ifdef __AVX2__
    _val = pack_t::gatherm(src._val, base_addr, vindex._val, mask._val, scale);
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX2 not enabled. HIPP::SIMD::Vec<float,8>::gatherm is implemented sequentially
#endif
    _u_vs_t val;
    const _u_vs_t _src = { src._val };
    const _u_ivs_t idx = { vindex._val };
    const int _mask = pack_t::movemask( mask._val );
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
Vec<float,8>::storem( addr_t mem_addr, const IntVec &mask ) const noexcept{
    pack_t::storem(mem_addr._addr, mask._val, _val);
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
Vec<float,8>::storem( addr_t mem_addr, const IntVec &mask ) noexcept{
    pack_t::storem(mem_addr._addr, mask._val, _val);
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
inline const Vec<float,8> & Vec<float,8>::scatter(void *base_addr, const IntVec &vindex, 
    int scale) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatter( base_addr, vindex._val, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX512F + AVX512VL not enabled. HIPP::SIMD::Vec<float,8>::scatter is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    const _u_ivs_t idx = {vindex._val};
    for(size_t i=0; i<NPACK; ++i){
        *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
    }
#endif
    return *this;    
}
inline const Vec<float,8> & 
Vec<float,8>::scatterm(void *base_addr, mask8_t k, const IntVec &vindex, 
    int scale) const noexcept{
#if defined(__AVX512F__) && defined(__AVX512VL__)
    pack_t::scatterm( base_addr, k, vindex._val, _val, scale );
#else
#ifdef _HIPPSIMD_WSEQ
#warning AVX512F + AVX512VL not enabled. HIPP::SIMD::Vec<float,8>::scatterm is implemented sequentially
#endif
    const _u_vs_t val = {_val};
    const _u_ivs_t idx = {vindex._val};
    for(size_t i=0; i<NPACK; ++i){
        if( k & ( (mask8_t)1 << (mask8_t)i ) )
            *(scal_t *)(base_addr + idx.s[i]*scale) = val.s[i];
    }
#endif
    return *this;
}
inline Vec<float,8> & Vec<float,8>::scatter(void *base_addr, const IntVec &vindex, 
    int scale) noexcept{
    return (Vec &)((const Vec *)this)->scatter(base_addr, vindex, scale);
}
inline Vec<float,8> & Vec<float,8>::scatterm(void *base_addr, mask8_t k, const IntVec &vindex, 
    int scale) noexcept{
    return (Vec &)((const Vec *)this)->scatterm(base_addr, k, vindex, scale);
}

inline Vec<float,8>::scal_t 
Vec<float,8>::to_scal( )const noexcept{
    return pack_t::to_scal( _val );
}

inline
auto Vec<float,8>::to_ivec()const noexcept -> IntVec {
    return pack_t::to_ivec(_val);
}

inline 
auto Vec<float,8>::tot_ivec() const noexcept -> IntVec {
    return pack_t::tot_ivec(_val);
}

inline
auto Vec<float,8>::to_i32vec()const noexcept -> IntVec {
    return to_ivec();
}

inline 
auto Vec<float,8>::tot_i32vec() const noexcept -> IntVec {
    return tot_ivec();
}

inline auto Vec<float,8>::from_si(const IntVec &a) noexcept -> Vec & {
    _val = pack_t::from_si(a._val);
    return *this;
}

inline auto Vec<float,8>::to_si()const noexcept -> IntVec {
    return pack_t::to_si(_val);
}

inline auto Vec<float,8>::to_vec_hc() const noexcept -> VecHC { 
    return pack_t::to_vec_hc(_val); 
}

inline auto Vec<float,8>::extract_hc(const int imm8) const noexcept -> VecHC { 
    return pack_t::extract_hc(_val, imm8); 
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
inline Vec<float,8> operator|( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::or_(a._val, b._val);
}
inline Vec<float,8> operator^( const Vec<float,8> &a, const Vec<float,8> &b ) noexcept{
    return Vec<float,8>::pack_t::xor_(a._val, b._val);
}
inline Vec<float,8> Vec<float,8>::andnot( const Vec &a ) const noexcept{
    return pack_t::andnot(_val, a._val);
}
inline Vec<float,8> Vec<float,8>::operator~()const noexcept{
    return pack_t::xor_(_val, pack_t::eq(_val, _val) );
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
inline auto Vec<float,8>::sum_all() const noexcept -> scal_t {
    VecHC vlow = to_vec_hc(), vhi = extract_hc(1);
    vlow += vhi;
    return vlow.sum_all();
}


#endif // __AVX__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECS8_IMPL_H_