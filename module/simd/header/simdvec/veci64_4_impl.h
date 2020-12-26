#ifndef _HIPPSIMD_VECI64_4_IMPL_H_
#define _HIPPSIMD_VECI64_4_IMPL_H_
#include "vec_impl_base.h"
namespace HIPP::SIMD {
#ifdef __AVX2__
Vec<long long, 4> & Vec<long long, 4>::set1(const VecHC &a) noexcept                          
{ _val = pack_t::set1(a._val); return *this; }
Vec<long long, 4> Vec<long long, 4>::sl(const VecHC &count) const noexcept                    
{ return pack_t::sl(_val, count._val); }
Vec<long long, 4> Vec<long long, 4>::sr(const VecHC &count) const noexcept                    
{ return pack_t::sr(_val, count._val); }
#endif //__AVX2__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECI64_4_IMPL_H_