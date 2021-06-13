#ifndef _HIPPSIMD_VECD2_H_
#define _HIPPSIMD_VECD2_H_
#include "vecbase.h"
#include <hippcntl.h>
namespace HIPP::SIMD {

#ifdef __SSE__

namespace _pd128_helper {
struct AddrAligned: public Packd2Base {
    typedef Vec<double, 2> vec; 
    
    AddrAligned( vec *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){} 
    AddrAligned( scal_t *addr ): _addr(addr){}
    AddrAligned( vec_t *addr ): AddrAligned( reinterpret_cast<scal_t *>(addr) ){}

    scal_t * const _addr;
};
struct CAddrAligned: public Packd2Base {
    typedef Vec<double, 2> vec;

    CAddrAligned( const vec *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){} 
    CAddrAligned( const scal_t *addr ): _addr(addr){}
    CAddrAligned( const vec_t *addr ): CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){}

    const scal_t * const _addr;
};
} // namespace _pd128_helper

template<>
class Vec<double, 2>: public _pd128_helper::Packd2Base {
public:
    typedef Packed<double, 2> pack_t;
    typedef Vec<iscal_t, 2> IntVec;
    typedef Vec<int32_t, 2> IntVecHP;
    
    Vec() noexcept                                                              {}
    Vec(const vec_t & a) noexcept                                               : _val(a) {}
    
    Vec(const Vec &a) noexcept                                                  : _val(a._val) {}
    Vec(const Vec &&a) noexcept                                                 : _val(a._val) {}
    Vec & operator=(const Vec &a) noexcept                                      { _val = a._val; }
    Vec & operator=(const Vec &&a) noexcept                                     { _val = a._val; }
    ~Vec() noexcept                                                             {}

    vec_t & val() noexcept { return _val; }
    const vec_t & val() const noexcept { return _val; }

    friend Vec operator+( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator-( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator*( const Vec &a, const Vec &b ) noexcept;
    friend Vec operator/( const Vec &a, const Vec &b ) noexcept;
    Vec adds(const Vec &b) const noexcept                                       { return pack_t::adds(_val, b._val); }
    Vec subs(const Vec &b) const noexcept                                       { return pack_t::subs(_val, b._val); }
    Vec muls(const Vec &b) const noexcept                                       { return pack_t::muls(_val, b._val); }
    Vec divs(const Vec &b) const noexcept                                       { return pack_t::divs(_val, b._val); }
    Vec & operator+=(const Vec &b) noexcept                                     { _val = pack_t::add(_val, b._val); return *this; }
    Vec & operator-=(const Vec &b) noexcept                                     { _val = pack_t::sub(_val, b._val); return *this; }
    Vec & operator*=(const Vec &b) noexcept                                     { _val = pack_t::mul(_val, b._val); return *this; }
    Vec & operator/=(const Vec &b) noexcept                                     { _val = pack_t::div(_val, b._val); return *this; }

    scal_t to_scal() const noexcept                                             { return pack_t::to_scal(_val); }
    Vec unpackhi(const Vec &b) const noexcept                                   { return pack_t::unpackhi(_val, b._val); }
    Vec unpacklo(const Vec &b) const noexcept                                   { return pack_t::unpacklo(_val, b._val); }
protected:
    vec_t _val;
};



#endif // __SSE__

} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECD2_H_