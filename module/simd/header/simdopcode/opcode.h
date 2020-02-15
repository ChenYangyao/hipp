#ifndef _HIPPSIMD_OPCODE_H_
#define _HIPPSIMD_OPCODE_H_
#include <x86intrin.h>
namespace HIPP{
namespace SIMD{
enum class Mode : int{
    /**
     * round opt for float intrinsic
     */
    FROUND_NEAR = (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC),
    FROUND_NEGINF = (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC),
    FROUND_POSINF = (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC),
    FROUND_ZERO = (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC),
    FROUND_CUR_DIREC = (_MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC),

    /**
     * comparison opt
     */
    CMP_EQ = _CMP_EQ_OQ,
    CMP_FALSE = _CMP_FALSE_OQ,
    CMP_NEQ = _CMP_NEQ_OQ,
    CMP_LT = _CMP_LT_OQ,
    CMP_LE = _CMP_LE_OQ,
    CMP_GT = _CMP_GT_OQ,
    CMP_GE = _CMP_GE_OQ
};
} // namespace SIMD  
} // namespace HIPP

#endif	//_HIPPSIMD_OPCODE_H_