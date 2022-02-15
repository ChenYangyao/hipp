/**
create: Yangyao CHEN, 2022/02/14
    [write   ] Sphere - a sphere in Euclid space.
*/

#ifndef _HIPPNUMERICAL_GEOMETRY_SPHERE_H_
#define _HIPPNUMERICAL_GEOMETRY_SPHERE_H_

#include "geometry_point.h"
namespace HIPP::NUMERICAL::GEOMETRY {

/**
A sphere in Euclid space.

The attributes and only attributes of ``Sphere`` are center and r, i.e., the 
center point and radius of the sphere, respectively.
*/
template<typename _FloatT, int _DIM> 
class Sphere {
public:
    inline static constexpr int DIM = _DIM;

    /**
    float_t: underlying floating-point scalar type for position and radius.
    point_t: type for the center point of the sphere.
    */
    using float_t = _FloatT;
    using point_t = Point<float_t, DIM>;
    
    /** 
    Constructors.
    (1): uninitialized.
    (2): set the center point ``center`` and radius ``r``.
    */
    Sphere() noexcept;
    Sphere(const point_t &center, float_t r) noexcept;

    ostream & info(ostream &os = cout, int  fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const Sphere &s) {
        return s.info(os);
    }

    /**
    Getters - get the center point and radius, respectively.
    */
    const point_t & center() const noexcept;
    point_t & center() noexcept;
    
    const float_t & r() const noexcept;
    float_t & r() noexcept;

    /**
    ``contains()``: find whether or not a point ``p`` is contained in the 
    sphere, i.e., the distance from ``p`` to the sphere is smaller than ``r``.
    */
    bool contains(const point_t &p) const noexcept;
protected:
    point_t _center;
    float_t _r;
};

#define _HIPP_TEMPHD template<typename _FloatT, int _DIM>
#define _HIPP_TEMPARG <_FloatT, _DIM>
#define _HIPP_TEMPCLS Sphere _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
Sphere() noexcept {}

_HIPP_TEMPNORET
Sphere(const point_t &center, float_t r) noexcept 
: _center(center), _r(r)
{}

_HIPP_TEMPRET
info(ostream &os, int  fmt_cntl, int level) const -> ostream & {
    PStream ps(os);
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Sphere),
        "{center=", _center, ", r=", _r, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(Sphere),
    ind, "Center = ", _center, ", r = ", _r, '\n';
    return os;
}

_HIPP_TEMPRET
center() const noexcept -> const point_t &  { return _center; }

_HIPP_TEMPRET
center() noexcept -> point_t & { return _center; }

_HIPP_TEMPRET
r() const noexcept -> const float_t & { return _r; }

_HIPP_TEMPRET
r() noexcept -> float_t & { return _r; }

_HIPP_TEMPRET
contains(const point_t &p) const noexcept -> bool {
    return (p - _center).r_sq() < _r * _r;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

} // namespace HIPP::NUMERICAL::GEOMETRY

#endif	//_HIPPNUMERICAL_GEOMETRY_SPHERE_H_