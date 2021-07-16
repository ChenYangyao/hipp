#ifndef _HIPPSIMD_VECS4_H_
#define _HIPPSIMD_VECS4_H_
#include "vecbase.h"
#include "../hippsimd_simdpacked/ps128.h"
#include <hippcntl.h>
namespace HIPP::SIMD {
#ifdef __SSE__

namespace _ps128_helper {
struct AddrAligned: public Packs4Base {
    typedef Vec<float, 4> vec; 

    AddrAligned( vec *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){} 
    AddrAligned( scal_t *addr ): _addr(addr){}
    AddrAligned( vec_t *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){}

    scal_t * const _addr;
};
struct CAddrAligned: public Packs4Base {
    typedef Vec<float, 4> vec; 
    
    CAddrAligned( const vec *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){} 
    CAddrAligned( const scal_t *addr ): _addr(addr){}
    CAddrAligned( const vec_t *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){}

    const scal_t * const _addr;
};
} // namespace _ps128_helper


template<>
class Vec<float, 4>: public _ps128_helper::Packs4Base {
public:
    typedef Packed<float, 4> pack_t;
    typedef _ps128_helper::AddrAligned addr_t;
    typedef _ps128_helper::CAddrAligned caddr_t;

    typedef Vec<iscal_t, 4> IntVec;

    Vec() noexcept                                                              {}
    Vec( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept                  :_val(pack_t::set(e3, e2, e1, e0)){ }
    explicit Vec( scal_t a ) noexcept                                           : _val(pack_t::set1(a)){}
    explicit Vec( caddr_t mem_addr ) noexcept                                   : _val(pack_t::load(mem_addr._addr)){}
    Vec( const vec_t &a ) noexcept                                              : _val(a){}
    
    Vec( const Vec &a ) noexcept                                                : _val(a._val){}
    Vec( Vec &&a ) noexcept                                                     : _val(a._val){}
    Vec & operator=( const Vec &a ) noexcept                                    { _val = a._val; return *this; }
    Vec & operator=( Vec &&a ) noexcept                                         { _val = a._val; return *this; }
    ~Vec(){}

    ostream & info( ostream &os = cout, int fmt_cntl=1 ) const;
    friend ostream & operator<<( ostream &os, const Vec &v );

    const vec_t & val() const noexcept                                          { return _val; }
    vec_t & val() noexcept                                                      { return _val; }
    const scal_t & operator[]( size_t n ) const noexcept                        { return ((scal_t *)&_val)[n]; }
    scal_t & operator[]( size_t n ) noexcept                                    { return ((scal_t *)&_val)[n]; }

    Vec & load( caddr_t mem_addr ) noexcept                                     { _val = pack_t::load(mem_addr._addr); return *this; }
    Vec & loadr( caddr_t mem_addr ) noexcept                                    { _val = pack_t::loadr(mem_addr._addr); return *this; }
    Vec & loadu( caddr_t mem_addr ) noexcept                                    { _val = pack_t::loadu(mem_addr._addr); return *this; }
    Vec & loads( caddr_t mem_addr ) noexcept                                    { _val = pack_t::loads(mem_addr._addr); return *this; }
    Vec & load1( caddr_t mem_addr ) noexcept                                    { _val = pack_t::load1(mem_addr._addr); return *this; }
    Vec & bcast( caddr_t mem_addr ) noexcept                                    { _val = pack_t::bcast(mem_addr._addr); return *this; }

    const Vec & store( addr_t mem_addr ) const noexcept                         { pack_t::store(mem_addr._addr, _val); return *this; }
    const Vec & storer( addr_t mem_addr ) const noexcept                        { pack_t::storer(mem_addr._addr, _val); return *this; }
    const Vec & storeu( addr_t mem_addr ) const noexcept                        { pack_t::storeu(mem_addr._addr, _val); return *this; }
    const Vec & stores( addr_t mem_addr ) const noexcept                        { pack_t::stores(mem_addr._addr, _val); return *this; }
    const Vec & store1( addr_t mem_addr ) const noexcept                        { pack_t::store1(mem_addr._addr, _val); return *this; }
    const Vec & stream( addr_t mem_addr ) const noexcept                        { pack_t::stream(mem_addr._addr, _val); return *this; }


    scal_t to_scal() const noexcept                                             { return pack_t::to_scal(_val); }
#ifdef __SSE2__
    IntVec to_si() const noexcept;
#endif //__SSE2__

    Vec & set(scal_t e3, scal_t e2, scal_t e1, scal_t e0) noexcept              { _val = pack_t::set(e3, e2, e1, e0); return *this; }
    Vec & setr(scal_t e3, scal_t e2, scal_t e1, scal_t e0) noexcept             { _val = pack_t::setr(e3, e2, e1, e0); return *this; }
    Vec & sets(scal_t a) noexcept                                               { _val = pack_t::sets(a); return *this; }
    Vec & set1(scal_t a) noexcept                                               { _val = pack_t::set1(a); return *this; }
    Vec & set1(const Vec &a) noexcept                                           { _val = pack_t::set1(a._val); return *this; }
    Vec & set() noexcept                                                        { _val = pack_t::set(); return *this; }
    Vec & setzero() noexcept                                                    { _val = pack_t::setzero(); return *this; }
    Vec & undefined() noexcept                                                  { _val = pack_t::undefined(); return *this; }
    
    Vec shuffle(const Vec &b, unsigned int imm8) const noexcept                 { return pack_t::shuffle(_val, b._val, imm8); }
    Vec unpackhi(const Vec &b) const noexcept                                   { return pack_t::unpackhi(_val, b._val); }
    Vec unpacklo(const Vec &b) const noexcept                                   { return pack_t::unpacklo(_val, b._val); }  
    Vec movehl(const Vec &b) const noexcept                                     { return pack_t::movehl(_val, b._val); }
    Vec movelh(const Vec &b) const noexcept                                     { return pack_t::movelh(_val, b._val); }
    int movemask() const noexcept                                               { return pack_t::movemask(_val); }
#ifdef __SSE3__
    Vec movehdup() const noexcept                                               { return pack_t::movehdup(_val); }
    Vec moveldup() const noexcept                                               { return pack_t::moveldup(_val); }
#endif 
    
    friend Vec operator+( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator-( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator*( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator/( const Vec &a, const Vec &b ) noexcept;
    Vec adds(const Vec &b) const noexcept                                       { return pack_t::adds(_val, b._val); }
    Vec subs(const Vec &b) const noexcept                                       { return pack_t::subs(_val, b._val); }
    Vec muls(const Vec &b) const noexcept                                       { return pack_t::muls(_val, b._val); }
    Vec divs(const Vec &b) const noexcept                                       { return pack_t::divs(_val, b._val); }
    Vec & operator+=(const Vec &a) noexcept                                     { _val = pack_t::add(_val, a._val); return *this; }
    Vec & operator-=(const Vec &a) noexcept                                     { _val = pack_t::sub(_val, a._val); return *this; }
    Vec & operator*=(const Vec &a) noexcept                                     { _val = pack_t::mul(_val, a._val); return *this; }
    Vec & operator/=(const Vec &a) noexcept                                     { _val = pack_t::div(_val, a._val); return *this; }
    Vec hadd( const Vec &a ) const noexcept                                     { return pack_t::hadd(_val, a._val); }
    Vec hsub( const Vec &a ) const noexcept                                     { return pack_t::hsub(_val, a._val); }

    friend Vec operator&( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator|( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator^( const Vec &a, const Vec &b ) noexcept;
    Vec andnot( const Vec &a ) const noexcept                                   { return pack_t::andnot(_val, a._val); }
    Vec operator~() const noexcept                                              { return pack_t::xor_(_val, pack_t::eq(_val, _val)); }
    Vec & operator&=(const Vec &a) noexcept                                     { _val = pack_t::and_(_val, a._val); return *this; }
    Vec & operator|=(const Vec &a) noexcept                                     { _val = pack_t::or_(_val, a._val); return *this; }
    Vec & operator^=(const Vec &a) noexcept                                     { _val = pack_t::xor_(_val, a._val); return *this; }

    friend Vec operator==(const Vec &a, const Vec &b) noexcept;
    friend Vec operator!=(const Vec &a, const Vec &b) noexcept;
    friend Vec operator>(const Vec &a, const Vec &b) noexcept;
    friend Vec operator>=(const Vec &a, const Vec &b) noexcept;
    friend Vec operator<(const Vec &a, const Vec &b) noexcept;
    friend Vec operator<=(const Vec &a, const Vec &b) noexcept;
#ifdef __AVX__
    int testz(const Vec &a) const noexcept                                      { return pack_t::testz(_val, a._val); }                  
    int testz() const noexcept                                                  { return pack_t::testz(_val, _val); }
#endif 
    Vec min(const Vec &a) const noexcept                                        { return pack_t::min(_val, a._val); }
    Vec max(const Vec &a) const noexcept                                        { return pack_t::max(_val, a._val); }

#ifdef __SSE3__
    scal_t sum_all() const noexcept;
#endif
    Vec rcp() const noexcept                                                    { return pack_t::rcp(_val); }
    Vec rsqrt() const noexcept                                                  { return pack_t::rsqrt(_val); }
    Vec sqrt() const noexcept                                                   { return pack_t::sqrt(_val); }
    Vec rcps() const noexcept                                                   { return pack_t::rcps(_val); }
    Vec rsqrts() const noexcept                                                 { return pack_t::rsqrts(_val); }
    Vec sqrts() const noexcept                                                  { return pack_t::sqrts(_val); }
protected:
    vec_t _val;

    union _u_vs_t { vec_t f; scal_t s[NPACK]; };
    union _u_sv_t { scal_t s[NPACK]; vec_t f; };

};

inline
ostream & Vec<float, 4>::info( ostream &os, int fmt_cntl ) const {
    PStream ps(os);
    _u_vs_t u = { _val };
    ps << "HIPP::SIMD::Vec(float, 4>(", ps(u.s, u.s+NPACK), ")";
    if( fmt_cntl >= 1 ) ps << endl;
    return os;
}
inline
ostream & operator<<( ostream &os, const Vec<float, 4> &v ){
    return v.info(os);
}

inline Vec<float, 4> operator+( const Vec<float, 4> &a, const Vec<float,4> &b ) noexcept {
    return Vec<float, 4>::pack_t::add(a._val, b._val);
}
inline Vec<float, 4> operator-( const Vec<float, 4> &a, const Vec<float,4> &b ) noexcept {
    return Vec<float, 4>::pack_t::sub(a._val, b._val);
}
inline Vec<float, 4> operator*( const Vec<float, 4> &a, const Vec<float,4> &b ) noexcept {
    return Vec<float, 4>::pack_t::mul(a._val, b._val);
}
inline Vec<float, 4> operator/( const Vec<float, 4> &a, const Vec<float,4> &b ) noexcept {
    return Vec<float, 4>::pack_t::div(a._val, b._val);
}
inline Vec<float, 4> operator&( const Vec<float, 4> &a, const Vec<float, 4> &b ) noexcept {
    return Vec<float, 4>::pack_t::and_(a._val, b._val);
}
inline Vec<float, 4> operator|( const Vec<float, 4> &a, const Vec<float, 4> &b ) noexcept {
    return Vec<float, 4>::pack_t::or_(a._val, b._val);
}
inline Vec<float, 4> operator^( const Vec<float, 4> &a, const Vec<float, 4> &b ) noexcept {
    return Vec<float, 4>::pack_t::xor_(a._val, b._val);
}
inline Vec<float, 4> operator==(const Vec<float, 4> &a, const Vec<float, 4> &b) noexcept {
    return Vec<float, 4>::pack_t::eq(a._val, b._val);
}
inline Vec<float, 4> operator!=(const Vec<float, 4> &a, const Vec<float, 4> &b) noexcept {
    return Vec<float, 4>::pack_t::neq(a._val, b._val);
}
inline Vec<float, 4> operator>(const Vec<float, 4> &a, const Vec<float, 4> &b) noexcept {
    return Vec<float, 4>::pack_t::gt(a._val, b._val);
}
inline Vec<float, 4> operator>=(const Vec<float, 4> &a, const Vec<float, 4> &b) noexcept {
    return Vec<float, 4>::pack_t::ge(a._val, b._val);
}
inline Vec<float, 4> operator<(const Vec<float, 4> &a, const Vec<float, 4> &b) noexcept {
    return Vec<float, 4>::pack_t::lt(a._val, b._val);
}
inline Vec<float, 4> operator<=(const Vec<float, 4> &a, const Vec<float, 4> &b) noexcept {
    return Vec<float, 4>::pack_t::le(a._val, b._val);
}

#ifdef __SSE3__
inline auto Vec<float, 4>::sum_all() const noexcept -> scal_t {
    Vec shuf = movehdup(),
        sum = *this + shuf;
    shuf = shuf.movehl(sum);
    sum = sum.adds(shuf);
    return sum.to_scal();
}
#endif 

#endif
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECS4_H_