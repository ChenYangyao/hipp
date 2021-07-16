#ifndef _HIPPSIMD_VECD2_IMPL_H_
#define _HIPPSIMD_VECD2_IMPL_H_
#include "vec_impl_base.h"
namespace HIPP::SIMD {

#ifdef __SSE__

inline Vec<double, 2> operator+( const Vec<double, 2> &a, const Vec<double, 2> &b ) noexcept {
    return Vec<double, 2>::pack_t::add(a._val, b._val);
}
inline Vec<double, 2> operator-( const Vec<double, 2> &a, const Vec<double, 2> &b ) noexcept {
    return Vec<double, 2>::pack_t::sub(a._val, b._val);
}
inline Vec<double, 2> operator*( const Vec<double, 2> &a, const Vec<double, 2> &b ) noexcept {
    return Vec<double, 2>::pack_t::mul(a._val, b._val);
}
inline Vec<double, 2> operator/( const Vec<double, 2> &a, const Vec<double, 2> &b ) noexcept {
    return Vec<double, 2>::pack_t::div(a._val, b._val);
}

#endif // __SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECD2_IMPL_H_