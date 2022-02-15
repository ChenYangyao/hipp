/**
create: Yangyao CHEN, 2022/02/14
    [write   ] Rect - a rectangle in Euclid space.
*/

#ifndef _HIPPNUMERICAL_GEOMETRY_RECT_H_
#define _HIPPNUMERICAL_GEOMETRY_RECT_H_

#include "geometry_point.h"

namespace HIPP::NUMERICAL::GEOMETRY {
    
/**
A rectangle in Euclid space.

The attributes and only attributes of ``Rect`` are ``low`` and ``high``, which 
are two ``point_t`` objects specifying the lower corner and upper corner, 
respectively. 
``low`` may be larger than ``high`` at any dimension.
*/
template<typename _FloatT, int _DIM> 
class Rect {
public:
    inline static constexpr int DIM = _DIM;

    /**
    float_t: underlying floating-point scalar type for position.
    point_t: type for the lower and upper corners of the rectangle.
    offset_t: the difference vector between two points.
    coord_t: the local frame coordinate of a point w.r.t. the rectangle.
    rect_size_t: a ``SVec`` instantiation that represent the size of the 
        rectangle.
    */
    using float_t     = _FloatT;
    using point_t     = Point<float_t, DIM>;
    using offset_t    = typename point_t::offset_t;
    using coord_t     = offset_t;
    using rect_size_t = typename offset_t::value_t;

    /** 
    Constructors.
    (1): uninitialized.
    (2): set the lower corner ``low`` and upper corner ``high``.
    */
    Rect() noexcept;
    Rect(const point_t &low, const point_t &high) noexcept;

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const Rect &rect) {
        return rect.info(os);
    }

    /**
    Getters:
    low(), high(): return the two corners, respectively.
    size(): get the size of the rectangle at each dimensions as a ``Svec`` 
        instance.
    center(): get the center point of the rectangle.
    */
    const point_t & low() const noexcept;
    point_t & low() noexcept;
    
    const point_t & high() const noexcept;
    point_t & high() noexcept;

    rect_size_t size() const noexcept;

    point_t center() const noexcept;

    /**
    Coordinate conversions between a spatial point ``p`` in global frame and 
    its relative coordinate in the local frame spanned by the rectangle, i.e., 
    ``low() + coord == p``.
    */
    coord_t coord_of(const point_t &p) const noexcept;
    point_t point_at(const coord_t &coord) const noexcept;

    /**
    ``contains()``: find whether or not a point ``p`` is contained in the 
    rectangle along all dimensions. ``contains_along_axis()`` tells only in 
    a given dimension ``axis``.
    */
    bool contains(const point_t &p) const noexcept;
    bool contains_along_axis(int axis, float_t p) const noexcept;
protected:
    point_t _low, _high;
};


/* Implementation */

#define _HIPP_TEMPHD template<typename _FloatT, int _DIM>
#define _HIPP_TEMPARG <_FloatT, _DIM>
#define _HIPP_TEMPCLS Rect _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
Rect() noexcept {}

_HIPP_TEMPNORET
Rect(const point_t &low, const point_t &high) noexcept
: _low(low), _high(high) {}

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream & {
    PStream ps{os};
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Rect),
        "{low=", _low, ", high=", _high, "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(Rect),
    ind, "Low = ", _low, ", high = ", _high, ", size = ", size(), '\n';
    return os;
}

_HIPP_TEMPRET
low() const noexcept -> const point_t & { return _low; }

_HIPP_TEMPRET
low() noexcept -> point_t & { return _low; }

_HIPP_TEMPRET
high() const noexcept -> const point_t & { return _high; }

_HIPP_TEMPRET
high() noexcept -> point_t & { return _high; }

_HIPP_TEMPRET
size() const noexcept -> rect_size_t { return (_high - _low).value(); }

_HIPP_TEMPRET
center() const noexcept -> point_t {
    auto half_sz = 0.5 * size();
    return point_t( _low.pos() + half_sz );
}

_HIPP_TEMPRET
coord_of(const point_t &p) const noexcept -> coord_t { 
    return p - _low; 
}

_HIPP_TEMPRET
point_at(const coord_t &coord) const noexcept -> point_t {
    return _low + coord;
}

_HIPP_TEMPRET
contains(const point_t &p) const noexcept -> bool {
    return (p.pos() > _low.pos()).all() && (p.pos() < _high.pos()).all();
}

_HIPP_TEMPRET
contains_along_axis(int axis, float_t p) const noexcept -> bool {
    return _low.pos()[axis] < p && p < _high.pos()[axis];
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET



} // namespace HIPP::NUMERICAL::GEOMETRY

#endif	//_HIPPNUMERICAL_GEOMETRY_RECT_H_