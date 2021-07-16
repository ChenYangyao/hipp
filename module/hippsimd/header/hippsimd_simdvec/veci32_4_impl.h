#ifndef _HIPPSIMD_VECI32_4_IMPL_H_
#define _HIPPSIMD_VECI32_4_IMPL_H_
#include "vec_impl_base.h"
namespace HIPP::SIMD {
#ifdef __SSE__

#ifdef __SSE2__
inline Vec<int32_t, 4> operator+(const Vec<int32_t, 4> &a, const Vec<int32_t, 4> &b) noexcept {
    return Vec<int32_t, 4>::pack_t::add(a._val, b._val);
}
inline Vec<int32_t, 4> operator-(const Vec<int32_t, 4> &a, const Vec<int32_t, 4> &b) noexcept {
    return Vec<int32_t, 4>::pack_t::sub(a._val, b._val);
}
#ifdef __SSE4_1__
inline Vec<int32_t, 4> operator*(const Vec<int32_t, 4> &a, const Vec<int32_t, 4> &b) noexcept {
    return Vec<int32_t, 4>::pack_t::mul_as_lo(a._val, b._val);
}
#endif //__SSE4_1__
inline Vec<int32_t, 4> operator&(const Vec<int32_t, 4> &a, const Vec<int32_t, 4> &b) noexcept {
    return Vec<int32_t, 4>::pack_si_t::and_(a._val, b._val);
}
inline Vec<int32_t, 4> operator|(const Vec<int32_t, 4> &a, const Vec<int32_t, 4> &b) noexcept {
    return Vec<int32_t, 4>::pack_si_t::or_(a._val, b._val);
}
inline Vec<int32_t, 4> operator^(const Vec<int32_t, 4> &a, const Vec<int32_t, 4> &b) noexcept {
    return Vec<int32_t, 4>::pack_si_t::xor_(a._val, b._val);
}
inline Vec<int32_t, 4> operator==(const Vec<int32_t, 4> &a, const Vec<int32_t, 4> &b) noexcept{
    return Vec<int32_t, 4>::pack_t::eq(a._val, b._val);
}
inline Vec<int32_t, 4> operator<(const Vec<int32_t, 4> &a, const Vec<int32_t, 4> &b) noexcept{
    return Vec<int32_t, 4>::pack_t::lt(a._val, b._val);
}
inline Vec<int32_t, 4> operator>(const Vec<int32_t, 4> &a, const Vec<int32_t, 4> &b) noexcept{
    return Vec<int32_t, 4>::pack_t::gt(a._val, b._val);
}
#endif //__SSE2__

#endif //__SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECI32_4_IMPL_H_