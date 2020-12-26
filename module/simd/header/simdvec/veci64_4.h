#ifndef _HIPPSIMD_VECI64_4_H_
#define _HIPPSIMD_VECI64_4_H_
#include "vecbase.h"
#include "hippcntl.h"
namespace HIPP {
namespace SIMD {
#ifdef __AVX2__

namespace _pi64_256_helper {
struct AddrAligned: public PackBase {
    typedef Vec<long long, 8> vec; 

    AddrAligned( vec *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){} 
    AddrAligned( scal_t *addr ): _addr(addr){}
    AddrAligned( vec_t *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){}

    scal_t * const _addr;
};
struct CAddrAligned: public PackBase {
    typedef Vec<long long, 8> vec; 
    
    CAddrAligned( const vec *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){} 
    CAddrAligned( const scal_t *addr ): _addr(addr){}
    CAddrAligned( const vec_t *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){}

    const scal_t * const _addr;
};
} // namespace _pi64_256_helper
template<>
class Vec<long long,4>: public _pi64_256_helper::PackBase {
public:
    typedef Packed<long long,4> pack_t;
    typedef _pi64_256_helper::AddrAligned addr_t;
    typedef _pi64_256_helper::CAddrAligned caddr_t;
    typedef Vec<scal_t, 2> VecHC;
    typedef Vec<int32_t, 4> VecHP;
    
    Vec() noexcept                                              {}
    explicit Vec(scal_t a) noexcept                             : _val( pack_t::set1(a) ) {}
    explicit Vec(caddr_t mem_addr) noexcept                     : _val( pack_si_t::load((const vec_t *)(mem_addr._addr)) ){}
    Vec( const vec_t &a ) noexcept                              : _val(a) {}
    Vec( const Vec &a ) noexcept                                : _val(a._val) {}
    Vec( Vec &&a ) noexcept                                     : _val(a._val) {}
    Vec & operator=( const Vec &a ) noexcept                    { _val = a._val; }
    Vec & operator=( Vec &&a ) noexcept                         { _val = a._val; }
    ~Vec() noexcept                                             {}

    ostream & info(ostream &os=cout, int fmt_cntl=1) const;
    friend ostream & operator<<( ostream &os, const Vec &v ); 

    const vec_t & val() const noexcept                          { return _val; }
    vec_t & val() noexcept                                      { return _val; }
    const scal_t & operator[](int n) const noexcept             { return ((const scal_t *)&_val)[n]; }
    scal_t & operator[](int n) noexcept                         { return ((scal_t *)&_val)[n]; }

    Vec & load(caddr_t mem_addr) noexcept                       { _val = pack_si_t::load( (const vec_t *)mem_addr._addr ); return *this; }
    Vec & loadu(caddr_t mem_addr) noexcept                      { _val = pack_si_t::loadu( (const vec_t *)mem_addr._addr ); return *this; }
    Vec & loadm(caddr_t mem_addr, const Vec &mask) noexcept     { _val = pack_t::loadm( mem_addr._addr, mask._val ); return *this; }
    Vec & loaddqu(caddr_t mem_addr) noexcept                    { _val = pack_si_t::loaddqu( (const vec_t *)mem_addr._addr ); return *this; }
    Vec & loadstream(caddr_t mem_addr) noexcept                 { _val = pack_si_t::loadstream( (const vec_t *)mem_addr._addr ); return *this; }
    Vec & gather( const scal_t *base_addr, const Vec &vindex, const int scale=SCALSIZE ) noexcept       { _val = pack_t::gather(base_addr, vindex._val, scale); return *this; }
    Vec & gather( const scal_t *base_addr, const VecHP &vindex, const int scale=SCALSIZE ) noexcept      { _val = pack_t::gather(base_addr, vindex._val, scale); return *this; }
    Vec & gatherm( const Vec &src, const scal_t *base_addr, const VecHP &vindex, const Vec &mask, const int scale=SCALSIZE ) noexcept    { _val = pack_t::gatherm(src._val, base_addr, vindex._val, mask._val, scale); return *this; }
    Vec & gatherm( const Vec &src, const scal_t *base_addr, const Vec &vindex, const Vec &mask, const int scale=SCALSIZE ) noexcept     { _val = pack_t::gatherm(src._val, base_addr, vindex._val, mask._val, scale); return *this; }

    const Vec & store(addr_t mem_addr) const noexcept           { pack_si_t::store( (vec_t *)mem_addr._addr, _val ); return *this; }
    const Vec & storeu(addr_t mem_addr) const noexcept          { pack_si_t::storeu( (vec_t *)mem_addr._addr, _val ); return *this; }
    const Vec & stream(addr_t mem_addr) const noexcept          { pack_si_t::stream( (vec_t *)mem_addr._addr, _val ); return *this; }
    const Vec & storem(addr_t mem_addr, const Vec &mask) const noexcept  { pack_t::storem( mem_addr._addr, mask._val, _val ); return *this; }
    Vec & store(addr_t mem_addr) noexcept                       { pack_si_t::store( (vec_t *)mem_addr._addr, _val ); return *this; }
    Vec & storeu(addr_t mem_addr) noexcept                      { pack_si_t::storeu( (vec_t *)mem_addr._addr, _val ); return *this; }
    Vec & storem(addr_t mem_addr, const Vec &mask) noexcept     { pack_t::storem( mem_addr._addr, mask._val, _val ); return *this; }
    Vec & stream(addr_t mem_addr) noexcept                      { pack_si_t::stream( (vec_t *)mem_addr._addr, _val ); return *this; }

    Vec & set(scal_t e3, scal_t e2, scal_t e1, scal_t e0) noexcept 
        { _val = pack_t::set(e3, e2, e1, e0); return *this; }
    Vec & setr(scal_t e3, scal_t e2, scal_t e1, scal_t e0) noexcept 
        { _val = pack_t::setr(e3, e2, e1, e0); return *this; }
    Vec & set1(scal_t a) noexcept                               { _val = pack_t::set1(a); return *this; }
    Vec & set1(const VecHC &a) noexcept                          { _val = pack_t::set1(a._val); return *this; }
    Vec & unpackhi(const Vec &a, const Vec &b) noexcept         { _val = pack_t::unpackhi(a._val, b._val); return *this; }
    Vec & unpacklo(const Vec &a, const Vec &b) noexcept         { _val = pack_t::unpacklo(a._val, b._val); return *this; }
    Vec & insert(scal_t i, const int index) noexcept            { _val = pack_t::insert(_val, i, index); return *this; }

    friend Vec operator+(const Vec &a, const Vec &b) noexcept;
    friend Vec operator-(const Vec &a, const Vec &b) noexcept;

    Vec & operator+=(const Vec &a) noexcept                     { _val = pack_t::add(_val, a._val); return *this; }
    Vec & operator-=(const Vec &a) noexcept                     { _val = pack_t::sub(_val, a._val); return *this; }

    friend Vec operator==(const Vec &a, const Vec &b) noexcept;
    friend Vec operator>(const Vec &a, const Vec &b) noexcept;
    friend Vec operator<(const Vec &a, const Vec &b) noexcept;
    int testz(const Vec &a) const noexcept                      { return pack_si_t::testz(_val, a._val); }
    int testz() const noexcept                                  { return pack_si_t::testz(_val, _val); }

    friend Vec operator&(const Vec &a, const Vec &b) noexcept;
    friend Vec operator|(const Vec &a, const Vec &b) noexcept;
    friend Vec operator^(const Vec &a, const Vec &b) noexcept;
    Vec andnot(const Vec &b) const noexcept                     { return pack_si_t::andnot(_val, b._val); }

    Vec & operator&=(const Vec &a) noexcept                     { _val = pack_si_t::and_(_val, a._val); return *this; }
    Vec & operator|=(const Vec &a) noexcept                     { _val = pack_si_t::or_(_val, a._val); return *this; }
    Vec & operator^=(const Vec &a) noexcept                     { _val = pack_si_t::xor_(_val, a._val); return *this; }

    Vec sli_si(const int imm8) const noexcept                   { return pack_si_t::sli_si(_val, imm8); }
    Vec sli(const int imm8) const noexcept                      { return pack_t::sli(_val, imm8); }
    Vec sl(const VecHC &count) const noexcept                    { return pack_t::sl(_val, count._val); }
    Vec sl(const Vec &count) const noexcept                     { return pack_t::sl(_val, count._val); }
    Vec sri_si(const int imm8) const noexcept                   { return pack_si_t::sri_si(_val, imm8); }
    Vec sri(const int imm8) const noexcept                      { return pack_t::sri(_val, imm8); }
    Vec sr(const VecHC &count) const noexcept                    { return pack_t::sr(_val, count._val); }
    Vec sr(const Vec &count) const noexcept                     { return pack_t::sr(_val, count._val); }
protected:
    vec_t _val;
    typedef Packed<int8_t, 32> pack_si_t;
    template<typename T, size_t N> friend class Vec;
};
inline ostream & Vec<long long,4>::info(ostream &os, int fmt_cntl) const{
    prt( os,  "HIPP::SIMD::Vec<long long,4>("); 
    const scal_t *p = (const scal_t *)&_val;
    prt_a(os, p, p+NPACK) << ")";
    if( fmt_cntl >= 1 ) os << endl;
    return os;
}
inline ostream & operator<<( ostream &os, const Vec<long long,4> &v ) {
    return v.info(os);
}

inline Vec<long long,4> operator+(const Vec<long long,4> &a, const Vec<long long,4> &b) noexcept{
    return Vec<long long,4>::pack_t::add(a._val, b._val);
}
inline Vec<long long,4> operator-(const Vec<long long,4> &a, const Vec<long long,4> &b) noexcept{
    return Vec<long long,4>::pack_t::sub(a._val, b._val);
}
inline Vec<long long,4> operator==(const Vec<long long,4> &a, const Vec<long long,4> &b) noexcept{
    return Vec<long long,4>::pack_t::eq(a._val, b._val);
}
inline Vec<long long,4> operator>(const Vec<long long,4> &a, const Vec<long long,4> &b) noexcept{
    return Vec<long long,4>::pack_t::gt(a._val, b._val);
}
inline Vec<long long,4> operator<(const Vec<long long,4> &a, const Vec<long long,4> &b) noexcept{
    return b>a;
}
inline Vec<long long,4> operator&(const Vec<long long,4> &a, const Vec<long long,4> &b) noexcept{
    return Vec<long long,4>::pack_si_t::and_(a._val, b._val);
}
inline Vec<long long,4> operator|(const Vec<long long,4> &a, const Vec<long long,4> &b) noexcept{
    return Vec<long long,4>::pack_si_t::or_(a._val, b._val);
}
inline Vec<long long,4> operator^(const Vec<long long,4> &a, const Vec<long long,4> &b) noexcept{
    return Vec<long long,4>::pack_si_t::xor_(a._val, b._val);
}

#endif

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_VECI64_4_H_