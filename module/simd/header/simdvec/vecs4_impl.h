#ifndef _HIPPSIMD_VECS4_IMPL_H_
#define _HIPPSIMD_VECS4_IMPL_H_
#include "vec_impl_base.h"
namespace HIPP::SIMD {
#ifdef __SSE__

#ifdef __SSE2__
inline 
auto Vec<float, 4>::to_si() const noexcept -> IntVec {
    return pack_t::to_si(_val);
}
#endif //__SSE2__

#endif //__SSE__
} // namespace HIPP::SIMD
#endif	//_HIPPSIMD_VECS4_IMPL_H_