#ifndef _HIPPSIMD_VECI8_16_H_
#define _HIPPSIMD_VECI8_16_H_
#include "vecbase.h"
#include <hippcntl.h>
namespace HIPP::SIMD {
#ifdef __SSE__
namespace _pi8_128_helper {
struct AddrAligned: public PackBase {
    typedef Vec<int8_t, 16> vec; 
    
    AddrAligned( vec *addr )
        : AddrAligned( reinterpret_cast<scal_t *>(addr) ){} 
    AddrAligned( scal_t *addr )
        : _addr(addr){}
    AddrAligned( vec_t *addr )
        : AddrAligned( reinterpret_cast<scal_t *>(addr) ){}

    scal_t * const _addr;
};
struct CAddrAligned: public PackBase {
    typedef Vec<int8_t, 16> vec; 
    
    CAddrAligned( const vec *addr )
        : CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){} 
    CAddrAligned( const scal_t *addr )
        : _addr(addr){}
    CAddrAligned( const vec_t *addr )
        : CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){}

    const scal_t * const _addr;
};
} // namespace _pi8_128_helper


template<>
class Vec<int8_t, 16>: public _pi8_128_helper::PackBase {
public:
    typedef Packed<int8_t, 16> pack_t;
    typedef _pi8_128_helper::AddrAligned addr_t;
    typedef _pi8_128_helper::CAddrAligned caddr_t;

    Vec() noexcept                                                              {}
    Vec(const vec_t &a) noexcept                                                : _val(a) {}
    Vec(const Vec &a) noexcept                                                  : _val(a._val) {}
    Vec(Vec &&a) noexcept                                                       : _val(a._val) {}
    Vec & operator=(const Vec &a) noexcept                                      { _val = a._val; }
    Vec & operator=(Vec &&a) noexcept                                           { _val = a._val; }
    ~Vec() noexcept                                                             {}
    
    Vec & load(caddr_t mem_addr) noexcept                                       { _val = pack_t::load( (const vec_t *)mem_addr._addr); return *this; }
    Vec & loadu(caddr_t mem_addr) noexcept                                      { _val = pack_t::loadu( (const vec_t *)mem_addr._addr); return *this; }   
    const Vec & store(addr_t mem_addr) const noexcept                           { pack_t::store((vec_t *)mem_addr._addr, _val); return *this; }
    const Vec & storeu(addr_t mem_addr) const noexcept                          { pack_t::storeu((vec_t *)mem_addr._addr, _val); return *this; }
    Vec & store(addr_t mem_addr) noexcept                                       { pack_t::store((vec_t *)mem_addr._addr, _val); return *this; }
    Vec & storeu(addr_t mem_addr) noexcept                                      { pack_t::storeu((vec_t *)mem_addr._addr, _val); return *this; }



    vec_t & val() noexcept                                                      {return _val;}
    const vec_t & val() const noexcept                                          {return _val;}

#ifdef __SSE2__
    Vec & set() noexcept                                                        { _val = pack_t::set(); return *this; }
    Vec & setzero() noexcept                                                    { _val = pack_t::setzero(); return *this; }
    Vec & set1(char_t a) noexcept                                               { _val = pack_t::set1(a); return *this; }

    friend Vec operator+(const Vec &a, const Vec &b) noexcept;
    friend Vec operator-(const Vec &a, const Vec &b) noexcept;
    
    friend Vec operator&(const Vec &a, const Vec &b) noexcept;
    friend Vec operator|(const Vec &a, const Vec &b) noexcept;
    friend Vec operator^(const Vec &a, const Vec &b) noexcept;
    Vec andnot(const Vec &b) const noexcept                                     { return pack_t::andnot(_val, b._val); }

    friend Vec operator==(const Vec &a, const Vec &b) noexcept;
    friend Vec operator<(const Vec &a, const Vec &b) noexcept;
    friend Vec operator>(const Vec &a, const Vec &b) noexcept;
#endif //__SSE2__

protected:
    template<typename T, size_t N> friend class Vec;
    vec_t _val;
};

#ifdef __SSE2__

inline Vec<int8_t, 16> operator+(const Vec<int8_t, 16> &a, const Vec<int8_t, 16> &b) noexcept {
    return Vec<int8_t, 16>::pack_t::add(a._val, b._val);
}
inline Vec<int8_t, 16> operator-(const Vec<int8_t, 16> &a, const Vec<int8_t, 16> &b) noexcept {
    return Vec<int8_t, 16>::pack_t::sub(a._val, b._val);
}
inline Vec<int8_t, 16> operator&(const Vec<int8_t, 16> &a, const Vec<int8_t, 16> &b) noexcept {
    return Vec<int8_t, 16>::pack_t::and_(a._val, b._val);
}
inline Vec<int8_t, 16> operator|(const Vec<int8_t, 16> &a, const Vec<int8_t, 16> &b) noexcept {
    return Vec<int8_t, 16>::pack_t::or_(a._val, b._val);
}
inline Vec<int8_t, 16> operator^(const Vec<int8_t, 16> &a, const Vec<int8_t, 16> &b) noexcept {
    return Vec<int8_t, 16>::pack_t::xor_(a._val, b._val);
}
inline Vec<int8_t, 16> operator==(const Vec<int8_t, 16> &a, const Vec<int8_t, 16> &b) noexcept{
    return Vec<int8_t, 16>::pack_t::eq(a._val, b._val);
}
inline Vec<int8_t, 16> operator<(const Vec<int8_t, 16> &a, const Vec<int8_t, 16> &b) noexcept{
    return Vec<int8_t, 16>::pack_t::lt(a._val, b._val);
}
inline Vec<int8_t, 16> operator>(const Vec<int8_t, 16> &a, const Vec<int8_t, 16> &b) noexcept{
    return Vec<int8_t, 16>::pack_t::gt(a._val, b._val);
}
#endif //__SSE2__


#endif //__SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECI8_16_H_