#ifndef _HIPPSIMD_VECI32_8_IMPL_H_
#define _HIPPSIMD_VECI32_8_IMPL_H_
#include "vec_impl_base.h"
namespace HIPP::SIMD {

#ifdef __AVX2__

Vec<int32_t, 8>& Vec<int32_t, 8>::set1(const VecHC &a) noexcept 
{ _val = pack_t::set1(a._val); return *this; }
Vec<int32_t, 8> Vec<int32_t, 8>::sl(const VecH4C_DP &count) const noexcept 
{ return pack_t::sl(_val, count._val); }
Vec<int32_t, 8> Vec<int32_t, 8>::sr(const VecH4C_DP &count) const noexcept 
{ return pack_t::sr(_val, count._val); }


#endif //__AVX2__


} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECI32_8_IMPL_H_