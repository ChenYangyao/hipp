#ifndef _HIPPSIMD_VECI32_4_H_
#define _HIPPSIMD_VECI32_4_H_
#include "vecbase.h"
#include <hippcntl.h>
namespace HIPP::SIMD {
#ifdef __SSE__
namespace _pi32_128_helper {
struct AddrAligned: public PackBase {
    typedef Vec<int32_t, 4> vec; 
    
    AddrAligned( vec *addr )
        : AddrAligned( reinterpret_cast<scal_t *>(addr) ){} 
    AddrAligned( scal_t *addr )
        : _addr(addr){}
    AddrAligned( vec_t *addr )
        : AddrAligned( reinterpret_cast<scal_t *>(addr) ){}

    scal_t * const _addr;
};
struct CAddrAligned: public PackBase {
    typedef Vec<int32_t, 4> vec; 
    
    CAddrAligned( const vec *addr )
        : CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){} 
    CAddrAligned( const scal_t *addr )
        : _addr(addr){}
    CAddrAligned( const vec_t *addr )
        : CAddrAligned( reinterpret_cast<const scal_t *>(addr) ){}

    const scal_t * const _addr;
};
} // namespace _pi32_128_helper


template<>
class Vec<int32_t, 4>: public _pi32_128_helper::PackBase {
public:
    typedef Packed<int32_t, 4> pack_t;
    typedef _pi32_128_helper::AddrAligned addr_t;
    typedef _pi32_128_helper::CAddrAligned caddr_t;

    Vec() noexcept                                                              {}
    Vec(const vec_t &a) noexcept                                                : _val(a) {}
    Vec(const Vec &a) noexcept                                                  : _val(a._val) {}
    Vec(Vec &&a) noexcept                                                       : _val(a._val) {}
    Vec & operator=(const Vec &a) noexcept                                      { _val = a._val; }
    Vec & operator=(Vec &&a) noexcept                                           { _val = a._val; }
    ~Vec() noexcept                                                             {}

    Vec & load(caddr_t mem_addr) noexcept                                       { _val = pack_si_t::load( (const vec_t *)mem_addr._addr); return *this; }
    Vec & loadu(caddr_t mem_addr) noexcept                                      { _val = pack_si_t::loadu( (const vec_t *)mem_addr._addr); return *this; }   
    const Vec & store(addr_t mem_addr) const noexcept                           { pack_si_t::store((vec_t *)mem_addr._addr, _val); return *this; }
    const Vec & storeu(addr_t mem_addr) const noexcept                          { pack_si_t::storeu((vec_t *)mem_addr._addr, _val); return *this; }
    Vec & store(addr_t mem_addr) noexcept                                       { pack_si_t::store((vec_t *)mem_addr._addr, _val); return *this; }
    Vec & storeu(addr_t mem_addr) noexcept                                      { pack_si_t::storeu((vec_t *)mem_addr._addr, _val); return *this; }

    vec_t & val() noexcept                                                      {return _val;}
    const vec_t & val() const noexcept                                          {return _val;}

#ifdef __SSE2__
    Vec & set() noexcept                                                        { _val = pack_si_t::set(); return *this; }
    Vec & setzero() noexcept                                                    { _val = pack_si_t::setzero(); return *this; }
    Vec & set1(scal_t a) noexcept                                               { _val = pack_t::set1(a); return *this; }

    friend Vec operator+(const Vec &a, const Vec &b) noexcept;
    friend Vec operator-(const Vec &a, const Vec &b) noexcept;
    Vec & operator+=(const Vec &b) noexcept                                     { _val = pack_t::add(_val, b._val); return *this; }
    Vec & operator-=(const Vec &b) noexcept                                     { _val = pack_t::sub(_val, b._val); return *this; }
#ifdef __SSE4_1__
    friend Vec operator*(const Vec &a, const Vec &b) noexcept;
    Vec & operator*=(const Vec &b) noexcept                                     { _val = pack_t::mul_as_lo(_val, b._val); return *this; }
#endif

    friend Vec operator&(const Vec &a, const Vec &b) noexcept;
    friend Vec operator|(const Vec &a, const Vec &b) noexcept;
    friend Vec operator^(const Vec &a, const Vec &b) noexcept;
    Vec andnot(const Vec &b) const noexcept                                     { return pack_si_t::andnot(_val, b._val); }
    Vec & operator&=(const Vec &b) noexcept                                     { _val = pack_si_t::and_(_val, b._val); return *this; }
    Vec & operator|=(const Vec &b) noexcept                                     { _val = pack_si_t::or_(_val, b._val); return *this; }
    Vec & operator^=(const Vec &b) noexcept                                     { _val = pack_si_t::xor_(_val, b._val); return *this; }

    friend Vec operator==(const Vec &a, const Vec &b) noexcept;
    friend Vec operator<(const Vec &a, const Vec &b) noexcept;
    friend Vec operator>(const Vec &a, const Vec &b) noexcept;

    Vec sli(const int imm8) noexcept                                            { return pack_t::sli(_val, imm8); }
    Vec sl(const Vec &count) noexcept                                           { return pack_t::sl(_val, count._val); }
    Vec sli_si(const int imm8) noexcept                                         { return pack_si_t::sli_si(_val, imm8); }

    Vec sri(const int imm8) noexcept                                            { return pack_t::sri(_val, imm8); }
    Vec sr(const Vec &count) noexcept                                           { return pack_t::sr(_val, count._val); }
    Vec sri_si(const int imm8) noexcept                                         { return pack_si_t::sri_si(_val, imm8); }

    Vec srai(const int imm8) noexcept                                           { return pack_t::srai(_val, imm8); }
    Vec sra(const Vec &count) noexcept                                          { return pack_t::sra(_val, count._val); }

#endif //__SSE2__

protected:
    template<typename T, size_t N> friend class Vec;
    typedef Packed<int8_t, 16> pack_si_t;
    vec_t _val;
};



#endif //__SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECI32_4_H_