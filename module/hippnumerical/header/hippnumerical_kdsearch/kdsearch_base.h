/**
create: Yangyao CHEN, 2022/02/14
    [write   ] KDPoint - The spatial point type used as input of space-searching 
        algorithms.
*/

#ifndef _HIPPNUMERICAL_KDSEARCH_BASE_H_
#define _HIPPNUMERICAL_KDSEARCH_BASE_H_

#include "../hippnumerical_geometry/geometry.h"
#include <optional>

namespace HIPP::NUMERICAL {

/**
The spatial point type used as input of space-searching algorithms.

``KDPoint`` is derived from ``GEOMETRY::Point``, with an extra padding field
to hold user-defined information. Such information is kept in the node
of any space-searching class so that you can retrieve it from the query
result.
*/
template<typename _FloatT = float, int _DIM = 3, size_t _PADDING = 0> 
class KDPoint : public GEOMETRY::Point<_FloatT, _DIM> {
public:
    inline static constexpr int DIM = _DIM;    
    inline static constexpr size_t PADDING = _PADDING;    
    using point_t = GEOMETRY::Point<_FloatT, _DIM>;
    
    using float_t = typename point_t::float_t;
    using pos_t = typename point_t::pos_t;
    using offset_t = typename point_t::offset_t;

    /**
    Constructors.
    (1) undefined position and padding.
    (2) set the position only.
    (3) set both position and padding (as if constructing using (2) and then 
    calling ``fill_pad(pad)``).
    */
    KDPoint() noexcept;

    KDPoint(const pos_t &pos) noexcept;
    
    KDPoint(std::initializer_list<float_t> pos) noexcept;
    
    template<typename PadT>
    KDPoint(const pos_t &pos, const PadT &pad) noexcept;

    /**
    Fill the padding field of the instance.

    (1) copy ``pad`` object as ``n = sizeof pad`` bytes.
    (2) copy ``n`` bytes starting at ``p``.
    
    UB if ``n > PADDING``. If ``n < PADDING``, tail bytes are not modified.
    */
    template<typename PadT>
    void fill_pad(const PadT &pad) noexcept;
    void fill_pad(const char *p, size_t n) noexcept;

    /**
    Getters - return the padding field, reinterpreted as an object typed ``T``.
    */
    template<typename T = char[PADDING]>
    T & pad() noexcept;
    
    template<typename T = char[PADDING]>
    const T & pad() const noexcept;
protected:
    char _pad[PADDING];
};

#define _HIPP_TEMPHD template<typename _FloatT, int _DIM, size_t _PADDING>
#define _HIPP_TEMPARG <_FloatT, _DIM, _PADDING>
#define _HIPP_TEMPCLS KDPoint _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPNORET
KDPoint() noexcept {}

_HIPP_TEMPNORET
KDPoint(const pos_t &pos) noexcept : point_t(pos) {}

_HIPP_TEMPNORET
KDPoint(std::initializer_list<float_t> pos) noexcept : point_t(pos) {}

_HIPP_TEMPHD
template<typename PadT>
_HIPP_TEMPCLS::KDPoint(const pos_t &pos, const PadT &pad) noexcept
: point_t(pos) 
{
    fill_pad(pad);
}

_HIPP_TEMPHD
template<typename PadT>
void _HIPP_TEMPCLS::fill_pad(const PadT &pad) noexcept {
    fill_pad(reinterpret_cast<const char *>(&pad), sizeof(PadT));
}

_HIPP_TEMPRET
fill_pad(const char *p, size_t n) noexcept -> void {
    std::copy_n(p, n, _pad);
}

_HIPP_TEMPHD
template<typename T>
T & _HIPP_TEMPCLS::pad() noexcept {
    auto *p = reinterpret_cast<T *>(_pad);
    return *p;
}

_HIPP_TEMPHD
template<typename T>
const T & _HIPP_TEMPCLS::pad() const noexcept {
    const auto *p = reinterpret_cast<std::add_const_t<T> *>(_pad);
    return *p;
}

#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET

} // namespace HIPP::NUMERICAL

#endif	//_HIPPNUMERICAL_KDSEARCH_BASE_H_