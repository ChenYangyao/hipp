#ifndef _HIPPSIMD_VECBASE_H_
#define _HIPPSIMD_VECBASE_H_
#include "../simdpacked/packed.h"
namespace HIPP{
namespace SIMD{
template<typename ScaleT, size_t NPack> class Vec {
    ~Vec() = delete;
};

template<> class Vec<int8_t, 16> {
public:
    Vec(__m128i val) noexcept : _val(val){}
protected:
    template<typename T, size_t N> friend class Vec;
    __m128i _val;
};
template<> class Vec<int16_t, 8> {
public:
    Vec(__m128i val) noexcept : _val(val){}
protected:
    template<typename T, size_t N> friend class Vec;
    __m128i _val;
};
template<> class Vec<int32_t, 4> {
public:
    Vec(__m128i val) noexcept : _val(val){}
protected:
    template<typename T, size_t N> friend class Vec;
    __m128i _val;
};
template<> class Vec<long long, 2> {
public:
    Vec(__m128i val) noexcept : _val(val){}
protected:
    template<typename T, size_t N> friend class Vec;
    __m128i _val;
};

#define _HIPPSIMD_SERIES(func_name) Vectorization is not enabled. Implement func_name sequentially

} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_VECBASE_H_