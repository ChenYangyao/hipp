#ifndef _HIPPSIMD_PD128_H_
#define _HIPPSIMD_PD128_H_
#include "packedbase.h"
#include "../simdopcode/opcode.h"


namespace HIPP{
namespace SIMD{    
#ifdef __SSE__

namespace _pd128_helper {

class Packd2Base {
public:
    typedef double scal_t;
    typedef __m128d vec_t;

    typedef long long iscal_t;
    typedef typename TypeCvt<vec_t, 0, 0, 0>::ret ivec_t;
    
    enum: size_t { NPACK=2, 
        NBIT=128, 
        VECSIZE=sizeof(vec_t), 
        SCALSIZE=sizeof(scal_t) };
};


} // namespace _pd128_helper

template<> 
class Packed<double, 2>: public _pd128_helper::Packd2Base {
public:
    static vec_t add(vec_t a, vec_t b) noexcept                                 { return _mm_add_pd(a,b); }
    static vec_t sub(vec_t a, vec_t b) noexcept                                 { return _mm_sub_pd(a,b); }
    static vec_t mul(vec_t a, vec_t b) noexcept                                 { return _mm_mul_pd(a,b); }
    static vec_t div(vec_t a, vec_t b) noexcept                                 { return _mm_div_pd(a,b); }

    static vec_t adds(vec_t a, vec_t b) noexcept                                { return _mm_add_sd(a,b); }
    static vec_t subs(vec_t a, vec_t b) noexcept                                { return _mm_sub_sd(a,b); }
    static vec_t muls(vec_t a, vec_t b) noexcept                                { return _mm_mul_sd(a,b); }
    static vec_t divs(vec_t a, vec_t b) noexcept                                { return _mm_div_sd(a,b); }

    static scal_t to_scal(vec_t a) noexcept                                     { return _mm_cvtsd_f64(a); }
    static vec_t unpackhi(vec_t a, vec_t b) noexcept                            { return _mm_unpackhi_pd(a,b); }
    static vec_t unpacklo(vec_t a, vec_t b) noexcept                            { return _mm_unpacklo_pd(a,b); }
};


#endif  // __SSE__
} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_PD128_H_