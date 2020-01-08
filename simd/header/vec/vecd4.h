#ifndef _HIPPSIMD_VECD4_H_
#define _HIPPSIMD_VECD4_H_
#include "vecbase.h"
#include "../packed/pd256.h"
namespace HIPP{
namespace SIMD{
template<> 
class Vec<double, 4>: public _pd256_helper::Packd4Base {
public:
    typedef Packed<double, 4> pack_t;
    
    Vec() noexcept{}
    Vec( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept;
    explicit Vec( scal_t a ) noexcept;
    explicit Vec( const scal_t *mem_addr ) noexcept;
    Vec( const vec_t &a ) noexcept;
    Vec( const scal_t *base_addr, 
        index_t vindex, const int scale=SCALSIZE ) noexcept;
    Vec( vec_t src, const scal_t *base_addr, 
        index_t vindex, vec_t mask, const int scale=SCALSIZE ) noexcept;
    Vec( const Vec &a ) noexcept;
    Vec( Vec &&a ) noexcept;
    ~Vec(){}
    Vec & operator=( const Vec &a ) noexcept;
    Vec & operator=( Vec &&a ) noexcept;

    const vec_t &val() const noexcept{ return _val; }
    vec_t &val() noexcept{ return _val; }
    const scal_t & operator[]( std::size_t n )const noexcept;
    scal_t & operator[]( std::size_t n ) noexcept;

    Vec & load( const scal_t *mem_addr ) noexcept { _val = pack_t::load(mem_addr); return *this; }
    Vec & loadu( const scal_t *mem_addr ) noexcept { _val = pack_t::loadu(mem_addr); return *this; }
    Vec & loadm( const scal_t *mem_addr, mask_t mask ) noexcept { _val = pack_t::loadm(mem_addr, mask); return *this; }
    Vec & load1( const scal_t *mem_addr ) noexcept { _val = pack_t::load1(mem_addr); return *this; }
    Vec & bcast( const scal_t *mem_addr ) noexcept { _val = pack_t::bcast(mem_addr); return *this; }
    Vec & bcast( const vec_hc_t *mem_addr ) noexcept { _val = pack_t::bcast(mem_addr); return *this; }
    Vec & gather( const scal_t *base_addr, index_t vindex, const int scale=SCALSIZE ) noexcept { _val = pack_t::gather(base_addr, vindex, scale); return *this; }
    Vec & gatherm( vec_t src, const scal_t *base_addr, index_t vindex, vec_t mask, const int scale=SCALSIZE ) noexcept { _val = pack_t::gatherm(src, base_addr, vindex, mask, scale); return *this; }
    const Vec & store( scal_t *mem_addr ) const noexcept { pack_t::store(mem_addr, _val); return *this; }
    const Vec & storem( scal_t *mem_addr, mask_t mask ) const noexcept { pack_t::storem(mem_addr, mask, _val); return *this; }
    const Vec & storeu( scal_t *mem_addr ) const noexcept { pack_t::storeu(mem_addr, _val); return *this; }
    const Vec & stream( scal_t *mem_addr ) const noexcept { pack_t::stream(mem_addr, _val); return *this; }
    Vec & store( scal_t *mem_addr ) noexcept { pack_t::store(mem_addr, _val); return *this; }
    Vec & storem( scal_t *mem_addr, mask_t mask ) noexcept { pack_t::storem(mem_addr, mask, _val); return *this; }
    Vec & storeu( scal_t *mem_addr ) noexcept { pack_t::storeu(mem_addr, _val); return *this; }
    Vec & stream( scal_t *mem_addr ) noexcept { pack_t::stream(mem_addr, _val); return *this; }

    int movemask( ) const noexcept { return pack_t::movemask(_val); }
    Vec movehdup( ) const noexcept { return pack_t::movehdup(_val); }
    Vec moveldup( ) const noexcept { return pack_t::moveldup(_val); }

    Vec & set( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept { _val = pack_t::set(e3,e2,e1,e0); return *this; }
    Vec & set1( scal_t a ) noexcept { _val = pack_t::set1(a); return *this; }
    Vec & set1( vec_hc_t a ) noexcept { _val = pack_t::set1(a); return *this; }
    Vec & set() noexcept { _val = pack_t::set(); return *this; }
    Vec & setzero() noexcept { _val = pack_t::setzero(); return *this; }
    Vec & undefined() noexcept { _val = pack_t::undefined(); return *this; }

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
    Vec & operator/=( const Vec &a ) noexcept;
    Vec & operator*=( const Vec &a ) noexcept;

    Vec operator&( const Vec &a ) const noexcept;
    Vec operator|( const Vec &a ) const noexcept;
    Vec operator~()const noexcept;
    Vec operator^( const Vec &a ) const noexcept;
    
    Vec operator==( const Vec &a ) const noexcept;
    Vec operator!=( const Vec &a ) const noexcept;
    Vec operator<( const Vec &a ) const noexcept;
    Vec operator<=( const Vec &a ) const noexcept;
    Vec operator>( const Vec &a ) const noexcept;
    Vec operator>=( const Vec &a ) const noexcept;

    Vec sqrt() const noexcept { return pack_t::sqrt(_val); }
    Vec ceil() const noexcept { return pack_t::ceil(_val); }
    Vec floor() const noexcept { return pack_t::floor(_val); }
    Vec round( const int rounding ) const noexcept { return pack_t::round(_val, rounding); }
    Vec max( const Vec &a ) const noexcept { return pack_t::max(_val, a._val); }
    Vec min( const Vec &a ) const noexcept { return pack_t::min(_val, a._val); }
protected:
    vec_t _val;
};

inline Vec<double,4>::Vec( scal_t e3, scal_t e2, scal_t e1, scal_t e0 ) noexcept :_val( pack_t::set(e3,e2,e1,e0) ){}
inline Vec<double,4>::Vec( scal_t a ) noexcept :_val(pack_t::set1(a)){}
inline Vec<double,4>::Vec( const scal_t *mem_addr ) noexcept :_val(pack_t::load(mem_addr)){}
inline Vec<double,4>::Vec( const vec_t &a ) noexcept :_val(a){}
inline Vec<double,4>::Vec( const scal_t *base_addr, pack_t::index_t vindex, const int scale ) noexcept :_val(pack_t::gather(base_addr, vindex, scale)){}
inline Vec<double,4>::Vec( vec_t src, const scal_t *base_addr, pack_t::index_t vindex, vec_t mask, const int scale) noexcept :_val(pack_t::gatherm(src, base_addr, vindex, mask, scale)){}
inline Vec<double,4>::Vec( const Vec &a )noexcept :_val(a._val) {}
inline Vec<double,4>::Vec( Vec &&a )noexcept :_val(a._val) {}
inline Vec<double,4> & Vec<double,4>::operator=( const Vec &a ) noexcept{
    _val = a._val; return *this;
}
inline Vec<double,4> & Vec<double,4>::operator=( Vec &&a ) noexcept{
    _val = a._val; return *this;
}
inline const Vec<double,4>::scal_t & Vec<double,4>::operator[]( std::size_t n )const noexcept{
    return ( (const scal_t *)&_val )[n];
}
inline Vec<double,4>::scal_t & Vec<double,4>::operator[]( std::size_t n ) noexcept{
    return ( (scal_t *)&_val )[n];
}
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
inline Vec<double,4> & Vec<double,4>::operator/=( const Vec &a ) noexcept{
    _val = pack_t::div(_val, a._val);
    return *this;
}
inline Vec<double,4> & Vec<double,4>::operator*=( const Vec &a ) noexcept{
    _val = pack_t::mul(_val, a._val);
    return *this;
}
inline Vec<double,4> Vec<double,4>::operator&( const Vec &a ) const noexcept{
    return pack_t::and_(_val, a._val);
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

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_VECD4_H_