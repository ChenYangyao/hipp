#ifndef _HIPPSIMD_VECI8_32_H_
#define _HIPPSIMD_VECI8_32_H_
#include "vecbase.h"
#include "hippcntl.h"
#include "veci16_16.h"
namespace HIPP {
namespace SIMD {
#ifdef __AVX2__

namespace _pi8_256_helper {

struct AddrAligned: public PackBase {
    typedef Vec<int8_t, 32> vec; 
    
    AddrAligned( vec *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){} 
    AddrAligned( scal_t *addr ): _addr(addr){}
    AddrAligned( vec_t *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){}

    scal_t * const _addr;
};
struct CAddrAligned: public PackBase {
    typedef Vec<int8_t, 32> vec; 
    
    CAddrAligned( const vec *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){} 
    CAddrAligned( const scal_t *addr ): _addr(addr){}
    CAddrAligned( const vec_t *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){}

    const scal_t * const _addr;
};
} // namespace _pi8_256_helper

template<> 
class Vec<int8_t, 32>: public _pi8_256_helper::PackBase {
public:
    typedef Packed<int8_t, 32> pack_t;
    typedef _pi8_256_helper::AddrAligned addr_t;
    typedef _pi8_256_helper::CAddrAligned caddr_t;
    typedef Vec<scal_t, 16> VecHC;
    typedef Vec<int16_t, 16> VecHC_DP;

    Vec() noexcept                                                              {}
    explicit Vec(char_t a) noexcept                                             : _val( pack_t::set1(a) ) {}
    explicit Vec(caddr_t mem_addr) noexcept                                     : _val( pack_t::load((const vec_t *)(mem_addr._addr)) ) {}
    Vec( const vec_t &a ) noexcept                                              : _val(a) {}
    Vec( const Vec &a ) noexcept                                                : _val(a._val) {}
    Vec( Vec &&a ) noexcept                                                     : _val(a._val) {}
    Vec & operator=( const Vec &a ) noexcept                                    { _val = a._val; }
    Vec & operator=( Vec &&a ) noexcept                                         { _val = a._val; }
    ~Vec() noexcept                                                             {}

    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<( ostream &os, const Vec &v ); 

    const vec_t & val() const noexcept                                          { return _val; }
    vec_t & val() noexcept                                                      { return _val; }
    const scal_t & operator[](int n) const noexcept                             { return ((const scal_t *)&_val)[n]; }
    scal_t & operator[](int n) noexcept                                         { return ((scal_t *)&_val)[n]; }

    Vec & load(caddr_t mem_addr) noexcept                                       { _val = pack_t::load( (const vec_t *)mem_addr._addr ); return *this; }
    Vec & loadu(caddr_t mem_addr) noexcept                                      { _val = pack_t::loadu( (const vec_t *)mem_addr._addr ); return *this; }
    Vec & loaddqu(caddr_t mem_addr) noexcept                                    { _val = pack_t::loaddqu( (const vec_t *)mem_addr._addr ); return *this; }
    Vec & loadstream(caddr_t mem_addr) noexcept                                 { _val = pack_t::loadstream( (const vec_t *)mem_addr._addr ); return *this; }

    const Vec & store(addr_t mem_addr) const noexcept                           { pack_t::store( (vec_t *)mem_addr._addr, _val ); return *this; }
    const Vec & storeu(addr_t mem_addr) const noexcept                          { pack_t::storeu( (vec_t *)mem_addr._addr, _val ); return *this; }
    const Vec & stream(addr_t mem_addr) const noexcept                          { pack_t::stream( (vec_t *)mem_addr._addr, _val ); return *this; }
    Vec & store(addr_t mem_addr) noexcept                                       { pack_t::store( (vec_t *)mem_addr._addr, _val ); return *this; }
    Vec & storeu(addr_t mem_addr) noexcept                                      { pack_t::storeu( (vec_t *)mem_addr._addr, _val ); return *this; }
    Vec & stream(addr_t mem_addr) noexcept                                      { pack_t::stream( (vec_t *)mem_addr._addr, _val ); return *this; }

    Vec & set(char_t e31, char_t e30, char_t e29, char_t e28, char_t e27, char_t e26, char_t e25, char_t e24, char_t e23, char_t e22, char_t e21, char_t e20, char_t e19, char_t e18, char_t e17, char_t e16, 
        char_t e15, char_t e14, char_t e13, char_t e12, char_t e11, char_t e10, char_t e9, char_t e8, char_t e7, char_t e6, char_t e5, char_t e4, char_t e3, char_t e2, char_t e1, char_t e0) noexcept 
        { _val = pack_t::set(e31, e30, e29, e28, e27, e26, e25, e24, e23, e22, e21, e20, e19, e18, e17, e16, e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0); return *this; }
    Vec & setr(char_t e31, char_t e30, char_t e29, char_t e28, char_t e27, char_t e26, char_t e25, char_t e24, char_t e23, char_t e22, char_t e21, char_t e20, char_t e19, char_t e18, char_t e17, char_t e16, 
        char_t e15, char_t e14, char_t e13, char_t e12, char_t e11, char_t e10, char_t e9, char_t e8, char_t e7, char_t e6, char_t e5, char_t e4, char_t e3, char_t e2, char_t e1, char_t e0) noexcept 
        { _val = pack_t::setr(e31, e30, e29, e28, e27, e26, e25, e24, e23, e22, e21, e20, e19, e18, e17, e16, e15, e14, e13, e12, e11, e10, e9, e8, e7, e6, e5, e4, e3, e2, e1, e0); return *this; }
    Vec & set(const VecHC &hi, const VecHC &lo) noexcept          { _val = pack_t::set(hi._val, lo._val); return *this; }
    Vec & setr(const VecHC &lo, const VecHC &hi) noexcept         { _val = pack_t::setr(lo._val, hi._val); return *this; }
    Vec & set1(char_t a) noexcept                               { _val = pack_t::set1(a); return *this; }
    Vec & set1(const VecHC &a) noexcept                          { _val = pack_t::set1(a._val); return *this; }
    Vec & set_hc(const VecHC &a) noexcept                        { _val = pack_t::bcast_hc(a._val); return *this; }      // repeat 128 bit
    Vec & from_hc(const VecHC &a) noexcept                       { _val = pack_t::from_si_hc(a._val); return *this; }    // upper 128 bit undefined
    VecHC to_hc() const noexcept                                 { return pack_t::to_si_hc(_val); }
    int movemask() const noexcept                               { return pack_t::movemask(_val); }
    Vec & packs(const VecHC_DP &a, const VecHC_DP &b) noexcept    { _val = pack_t::packs(a._val, b._val); return *this; }
    Vec & packus(const VecHC_DP &a, const VecHC_DP &b) noexcept   { _val = pack_t::packus(a._val, b._val); return *this; }
    Vec & unpackhi(const Vec &a, const Vec &b) noexcept         { _val = pack_t::unpackhi(a._val, b._val); return *this; }
    Vec & unpacklo(const Vec &a, const Vec &b) noexcept         { _val = pack_t::unpacklo(a._val, b._val); return *this; }
    Vec & set() noexcept                                        { _val = pack_t::set(); return *this; }
    Vec & setzero() noexcept                                    { _val = pack_t::setzero(); return *this; }
    Vec & undefined() noexcept                                  { _val = pack_t::undefined_si(); return *this; }

    Vec & insert(scal_t i, const int index) noexcept            { _val = pack_t::insert(_val, i, index); return *this; }
    Vec & insertf( const VecHC &b, int imm8 ) noexcept           { _val = pack_t::insertf(_val, b._val, imm8); return *this; }
    Vec & inserti( const VecHC &b, const int imm8 ) noexcept     { _val = pack_t::insertf(_val, b._val, imm8); return *this; }

    friend Vec operator+(const Vec &a, const Vec &b) noexcept;
    friend Vec operator-(const Vec &a, const Vec &b) noexcept;

    friend Vec operator==(const Vec &a, const Vec &b) noexcept;
    friend Vec operator>(const Vec &a, const Vec &b) noexcept;
    friend Vec operator<(const Vec &a, const Vec &b) noexcept;
    int testz(const Vec &a) const noexcept                      { return pack_t::testz(_val, a._val); }
    int testz() const noexcept                                  { return pack_t::testz(_val, _val); }

    friend Vec operator&(const Vec &a, const Vec &b) noexcept;
    Vec andnot(const Vec &b) const noexcept                     { return pack_t::andnot(_val, b._val); }
    friend Vec operator|(const Vec &a, const Vec &b) noexcept;
    friend Vec operator^(const Vec &a, const Vec &b) noexcept;

    Vec sli_si(const int imm8) const noexcept                   { return pack_t::sli_si(_val, imm8); }
    Vec sri_si(const int imm8) const noexcept                   { return pack_t::sri_si(_val, imm8); }

    Vec blend(const Vec &b, const Vec &mask) const noexcept     { return pack_t::blend(_val, b._val, mask._val); }
    Vec abs() const noexcept                                    { return pack_t::abs(_val); }
    Vec max(const Vec &b) const noexcept                        { return pack_t::max(_val, b._val); }
    Vec min(const Vec &b) const noexcept                        { return pack_t::min(_val, b._val); }
protected:
    vec_t _val;
} ;

inline ostream & Vec<int8_t,32>::info(ostream &os, int fmt_cntl) const{
    prt( os,  "HIPP::SIMD::Vec<int8_t,32>("); 
    const scal_t *p = (const scal_t *)&_val;
    prt_a(os, p, p+NPACK) << ")";
    if( fmt_cntl >= 1 ) os << endl;
    return os;
}
inline ostream & operator<<( ostream &os, const Vec<int8_t,32> &v ) {
    return v.info(os);
}
inline Vec<int8_t, 32> operator+(const Vec<int8_t, 32> &a, const Vec<int8_t, 32> &b) noexcept{
    return Vec<int8_t, 32>::pack_t::add(a._val, b._val);
}
inline Vec<int8_t, 32> operator-(const Vec<int8_t, 32> &a, const Vec<int8_t, 32> &b) noexcept{
    return Vec<int8_t, 32>::pack_t::sub(a._val, b._val);
}
inline Vec<int8_t, 32> operator==(const Vec<int8_t, 32> &a, const Vec<int8_t, 32> &b) noexcept{
    return Vec<int8_t, 32>::pack_t::eq(a._val, b._val);
}
inline Vec<int8_t, 32> operator>(const Vec<int8_t, 32> &a, const Vec<int8_t, 32> &b) noexcept{
    return Vec<int8_t, 32>::pack_t::gt(a._val, b._val);
}
inline Vec<int8_t, 32> operator<(const Vec<int8_t, 32> &a, const Vec<int8_t, 32> &b) noexcept{
    return b>a;
}
inline Vec<int8_t, 32> operator&(const Vec<int8_t, 32> &a, const Vec<int8_t, 32> &b) noexcept{
    return Vec<int8_t, 32>::pack_t::and_(a._val, b._val);
}
inline Vec<int8_t, 32> operator|(const Vec<int8_t, 32> &a, const Vec<int8_t, 32> &b) noexcept{
    return Vec<int8_t, 32>::pack_t::or_(a._val, b._val);
}
inline Vec<int8_t, 32> operator^(const Vec<int8_t, 32> &a, const Vec<int8_t, 32> &b) noexcept{
    return Vec<int8_t, 32>::pack_t::xor_(a._val, b._val);
}


#endif
} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_VECI8_32_H_