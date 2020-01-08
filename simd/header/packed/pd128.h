#ifndef _HIPPSIMD_PD128_H_
#define _HIPPSIMD_PD128_H_
#include "packedbase.h"
#include "../opcode/opcode.h"
namespace HIPP{
namespace SIMD{    
template<> class Packed<double, 2>{
    typedef double scal_t;
    typedef __m128d vec_t;
    enum: size_t { NPACK=2, NBIT=128, VECSIZE=sizeof(vec_t), SCALSIZE=sizeof(scal_t) };

    typedef typename TypeCvt<vec_t, 0, 0, 0>::ret ivec_t;
    typedef ivec_t mask_t;
    typedef ivec_t index_t;
};

} // namespace SIMD
} // namespace HIPP

#endif	//_HIPPSIMD_PD128_H_