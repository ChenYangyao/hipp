#ifndef _HIPPSIMD_VECBASE_H_
#define _HIPPSIMD_VECBASE_H_
#include "../hippsimd_simdpacked/packed.h"
namespace HIPP{
namespace SIMD{
template<typename ScaleT, size_t NPack> class Vec {
    ~Vec() = delete;
};

#define _HIPPSIMD_SERIES(func_name) \
    Vectorization is not enabled. Implement func_name sequentially

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_VECBASE_H_