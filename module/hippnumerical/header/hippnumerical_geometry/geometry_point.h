/**
create: Yangyao CHEN, 2022/02/14
    [write   ] Point, Offset - spatial point and difference vector of 
        two points.
*/

#ifndef _HIPPNUMERICAL_GEOMETRY_POINT_H_
#define _HIPPNUMERICAL_GEOMETRY_POINT_H_

#include "geometry_base.h"

namespace HIPP::NUMERICAL::GEOMETRY {

/**
The position vector difference of two spatial points.
Internally, the difference is represented by ``SVec<_FloatT, _DIM>``.
*/
template<typename _FloatT, int _DIM>
class Offset {
public:
    static_assert(std::is_floating_point_v<_FloatT>, 
        "argument _FloatT is not a float-point type");

    /* Dimensions of the space. */
    inline static constexpr int DIM = _DIM;

    using float_t = _FloatT;
    using value_t = SVec<float_t, DIM>;

    /**
    Constructors.
    (1): Uninitialized.
    (2): Vector elements are initialized by ``v``.
    (3): Vector elements are initialized by a initializer list.
    */
    Offset() noexcept;
    Offset(const value_t &v) noexcept;
    Offset(std::initializer_list<float_t> value) noexcept;

    /**
    Getters. 
    value(): return the internal vector representing the difference vector.
    operator[]: return the vector element at given index.
    r(), r_sq(): return the norm and squared-norm of the vector.
    operator
    */
    value_t & value() noexcept;
    const value_t & value() const noexcept;

    float_t & operator[](size_t i) noexcept;
    const float_t & operator[](size_t i) const noexcept;

    float_t r() const noexcept;
    float_t r_sq() const noexcept;

    Offset operator+(const Offset &off) const noexcept;
    Offset operator-(const Offset &off) const noexcept;
    Offset operator+(float_t off) const noexcept;
    Offset operator-(float_t off) const noexcept;
    Offset operator+() const noexcept;
    Offset operator-() const noexcept;
    Offset operator*(float_t scale) const noexcept;
    Offset operator/(float_t scale) const noexcept;

    Offset & operator+=(const Offset &off) noexcept;
    Offset & operator-=(const Offset &off) noexcept;
    Offset & operator+=(float_t off) noexcept;
    Offset & operator-=(float_t off) noexcept;
    Offset & operator*=(float_t scale) noexcept;
    Offset & operator/=(float_t scale) noexcept;
protected:
    value_t _value;
};

/**
A spatial point. Its only attribute is a position vector represented by 
``SVec<_FloatT, _DIM>``.
*/
template<typename _FloatT, int _DIM>
class Point {
public:
    /* Dimensions of the space. */
    inline static constexpr int DIM = _DIM;
    
    /**
    Internal scalar type, vector type, and the corresponding ``Offset`` type
    instantiation.
    */
    using float_t = _FloatT;
    using pos_t = SVec<float_t, DIM>;
    using offset_t = Offset<float_t, DIM>;

    /**
    Constructors.
    (1): Uninitialized.
    (2): Vector elements are initialized by ``v``.
    (3): Vector elements are initialized by a initializer list.
    */
    Point() noexcept;
    Point(const pos_t &v) noexcept;
    Point(std::initializer_list<float_t> value) noexcept;

    ostream & info(ostream &os = cout, int fmt_cntl = 0, int level = 0) const;
    friend ostream & operator<<(ostream &os, const Point &p) {
        return p.info(os);
    }

    /**
    Get the internal vector representing the point position.
    */
    pos_t & pos() noexcept;
    const pos_t & pos() const noexcept;

    /*
    Arithmetics:
    Point - Point -> Offset
    Point + Offset -> Point. Offset may be a ``Offset`` instance, or a scalar 
    value for every dimension.
    */
    offset_t operator-(const Point &p) const noexcept;
    
    Point operator+(const offset_t &off) const noexcept;
    Point operator-(const offset_t &off) const noexcept;
    Point operator+(float_t off) const noexcept;
    Point operator-(float_t off) const noexcept;

    Point & operator+=(const offset_t &off) noexcept;
    Point & operator-=(const offset_t &off) noexcept;
    Point & operator+=(float_t off) noexcept;
    Point & operator-=(float_t off) noexcept;
protected:
    pos_t _pos;
};

#define _HIPP_TEMPHD template<typename _FloatT, int _DIM>
#define _HIPP_TEMPARG <_FloatT, _DIM>
#define _HIPP_TEMPCLS Offset _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
Offset() noexcept {}

_HIPP_TEMPNORET
Offset(const value_t &value) noexcept 
: _value(value)
{}

_HIPP_TEMPNORET
Offset(std::initializer_list<float_t> value) noexcept
: _value(value)
{}

_HIPP_TEMPRET value() noexcept -> value_t &
{
    return _value;
}

_HIPP_TEMPRET value() const noexcept -> const value_t &
{
    return _value;
}

_HIPP_TEMPRET operator[](size_t i) noexcept -> float_t &
{
    return _value[i];
};

_HIPP_TEMPRET operator[](size_t i) const noexcept -> const float_t &
{
    return _value[i];
};

_HIPP_TEMPRET r() const noexcept -> float_t 
{
    return _value.norm();
}

_HIPP_TEMPRET r_sq() const noexcept -> float_t 
{
    return _value.squared_norm();
}

_HIPP_TEMPRET operator+(const Offset &off) const noexcept -> Offset 
{
    return Offset(_value + off._value);
}

_HIPP_TEMPRET operator-(const Offset &off) const noexcept -> Offset 
{
    return Offset(_value - off._value);
}

_HIPP_TEMPRET operator+(float_t off) const noexcept -> Offset 
{
    return Offset(_value + off);
}

_HIPP_TEMPRET operator-(float_t off) const noexcept -> Offset 
{
    return Offset(_value - off);
}

_HIPP_TEMPRET operator+() const noexcept -> Offset 
{
    return _value;
}

_HIPP_TEMPRET operator-() const noexcept -> Offset 
{
    return -_value;
}

_HIPP_TEMPRET operator*(float_t scale) const noexcept -> Offset 
{
    return Offset(_value * scale);
}

_HIPP_TEMPRET operator/(float_t scale) const noexcept -> Offset 
{
    return Offset(_value / scale);
}

_HIPP_TEMPHD
_HIPP_TEMPCLS operator+(typename _HIPP_TEMPCLS::float_t off, 
    const _HIPP_TEMPCLS &that) noexcept
{
    return _HIPP_TEMPCLS(off + that.value());
}

_HIPP_TEMPHD
_HIPP_TEMPCLS operator-(typename _HIPP_TEMPCLS::float_t off, 
    const _HIPP_TEMPCLS &that) noexcept
{
    return _HIPP_TEMPCLS(off - that.value());
}

_HIPP_TEMPHD
_HIPP_TEMPCLS operator*(typename _HIPP_TEMPCLS::float_t off, 
    const _HIPP_TEMPCLS &that) noexcept
{
    return _HIPP_TEMPCLS(off * that.value());
}

_HIPP_TEMPHD
_HIPP_TEMPCLS operator/(typename _HIPP_TEMPCLS::float_t off, 
    const _HIPP_TEMPCLS &that) noexcept
{
    return _HIPP_TEMPCLS(off / that.value());
}

_HIPP_TEMPRET operator+=(const Offset &off) noexcept -> Offset & 
{
    _value += off._value;
    return *this;
}

_HIPP_TEMPRET operator-=(const Offset &off) noexcept -> Offset & 
{
    _value -= off._value;
    return *this;
}

_HIPP_TEMPRET operator+=(float_t off) noexcept -> Offset & 
{
    _value += off;
    return *this;
}

_HIPP_TEMPRET operator-=(float_t off) noexcept -> Offset & 
{
    _value -= off;
    return *this;
}

_HIPP_TEMPRET operator*=(float_t scale) noexcept -> Offset & 
{
    _value *= scale;
    return *this;
}

_HIPP_TEMPRET operator/=(float_t scale) noexcept -> Offset & 
{
    _value /= scale;
    return *this;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

#define _HIPP_TEMPHD template<typename _FloatT, int _DIM>
#define _HIPP_TEMPARG <_FloatT, _DIM>
#define _HIPP_TEMPCLS Point _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
Point() noexcept {}

_HIPP_TEMPNORET
Point(const pos_t &pos) noexcept
: _pos(pos) {}

_HIPP_TEMPNORET
Point(std::initializer_list<float_t> pos) noexcept
: _pos(pos)
{}

_HIPP_TEMPRET
info(ostream &os, int fmt_cntl, int level) const -> ostream & {
    PStream ps{os};
    if( fmt_cntl < 1 ) {
        ps << HIPPCNTL_CLASS_INFO_INLINE(Point),
        "{", ps(_pos.begin(), _pos.end()), "}";
        return os;
    }
    auto ind = HIPPCNTL_CLASS_INFO_INDENT_STR(level);
    ps << HIPPCNTL_CLASS_INFO(Point),
    ind, "Pos = {", ps(_pos.begin(), _pos.end()), "}\n";
    return os;
}

_HIPP_TEMPRET pos() noexcept -> pos_t &
{
    return _pos;
}

_HIPP_TEMPRET pos() const noexcept -> const pos_t &
{
    return _pos;
}

_HIPP_TEMPRET operator-(const Point &p) const noexcept -> offset_t
{
    auto dx = pos() - p.pos();
    return offset_t(dx);
}

_HIPP_TEMPRET operator+(const offset_t &off) const noexcept -> Point
{
    auto x = pos() + off.value();
    return Point(x);
}

_HIPP_TEMPRET operator-(const offset_t &off) const noexcept -> Point
{
    auto x = pos() - off.value();
    return Point(x);
}

_HIPP_TEMPRET operator+(float_t off) const noexcept -> Point
{
    auto x = pos() + off;
    return Point(x);
}

_HIPP_TEMPRET operator-(float_t off) const noexcept -> Point
{
    auto x = pos() - off;
    return Point(x);
}

_HIPP_TEMPHD
_HIPP_TEMPCLS operator+(const typename _HIPP_TEMPCLS::offset_t &off,
    const _HIPP_TEMPCLS &that) noexcept
{
    auto x =  off.value() + that.pos();
    return _HIPP_TEMPCLS(x);
}

_HIPP_TEMPHD
_HIPP_TEMPCLS operator+(typename _HIPP_TEMPCLS::float_t off,
    const _HIPP_TEMPCLS &that) noexcept
{
    auto x =  off + that.pos();
    return _HIPP_TEMPCLS(x);
}

_HIPP_TEMPRET 
operator+=(const offset_t &off) noexcept -> Point & 
{
    pos() += off.value();
    return *this;
}

_HIPP_TEMPRET 
operator-=(const offset_t &off) noexcept -> Point & 
{
    pos() -= off.value();
    return *this;
}

_HIPP_TEMPRET 
operator+=(float_t off) noexcept -> Point & 
{
    pos() += off;
    return *this;
}

_HIPP_TEMPRET 
operator-=(float_t off) noexcept -> Point & 
{
    pos() -= off;
    return *this;
}


#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET


} // namespace HIPP::NUMERICAL::GEOMETRY

#endif	//_HIPPNUMERICAL_GEOMETRY_POINT_H_