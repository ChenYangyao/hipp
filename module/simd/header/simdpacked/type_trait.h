#ifndef _HIPPSIMD_TYPE_TRAIT_H_
#define _HIPPSIMD_TYPE_TRAIT_H_
#include <x86intrin.h>
namespace HIPP{
namespace SIMD{

/**
 * type convesion templates.
 * TypeCvt<T, Count, Precision, NumType>::ret is defined as the corresponding 
 * vector type according to other three template parameters.
 * 
 * For float point vector to either float point or integer vector
 * @T:          vector type to be converted.
 * @Count:      change of packed scalar type count. n for lifting to 2^n fold.
 * @Precision:  change of bytes representing the scalar type. n for lifting to 
 *              2^n fold.
 * @NumType:    Target scalar type. 0 for integer type, 1 for float-point type.
 * 
 * For integer vector to either float point or integer vector
 * `T` and `NumType` is of same meaning, but count and precision are the target
 * count of packed scalars (n for 2^n) and precision (n for 2^n bytes).
 */
template<typename T, int Count, int Precision, int NumType> class TypeCvt { };

#ifdef __SSE__
/**
 * float-128/128 v.s. float-128/64
 */
template<> class TypeCvt<__m128, -1, 1, 1>{ public: typedef __m128d ret; };
template<> class TypeCvt<__m128d, 1, -1, 1>{ public: typedef __m128 ret; };
template<> class TypeCvt<__m128, -1, 0, 1>{ public: typedef __m64 ret; };

/**
 * float-128 v.s. int-128
 */
template<> class TypeCvt<__m128, 0, 0, 0>{ public: typedef __m128i ret; };      
template<> class TypeCvt<__m128d, 0, 0, 0>{ public: typedef __m128i ret; };

/**
 * int/int-128 v.s. int/float-128
 */
template<> class TypeCvt<__m128i, 0, 4, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m128i, 1, 3, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m128i, 2, 2, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m128i, 3, 1, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m128i, 4, 0, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m128i, 1, 3, 1>{ public: typedef __m128d ret; };
template<> class TypeCvt<__m128i, 2, 2, 1>{ public: typedef __m128 ret; };
#endif

#ifdef __AVX__
/**
 * float-128/256/256 v.s. float-256/128/256
 */
template<> class TypeCvt<__m128, 1, 0, 1>{ public: typedef __m256 ret; };
template<> class TypeCvt<__m128, 0, 1, 1>{ public: typedef __m256d ret; };

template<> class TypeCvt<__m128d, 1, 0, 1>{ public: typedef __m256d ret; };
template<> class TypeCvt<__m256, -1, 0, 1>{ public: typedef __m128 ret; };
template<> class TypeCvt<__m256d, 0, -1, 1>{ public: typedef __m128 ret; };
template<> class TypeCvt<__m256d, -1, 0, 1>{ public: typedef __m128d ret; };

template<> class TypeCvt<__m256, -1, 1, 1>{ public: typedef __m256d ret; };
template<> class TypeCvt<__m256d, 1, -1, 1>{ public: typedef __m256 ret; };

/**
 * float-256/128/256 v.s. int-256/256/128
 */
template<> class TypeCvt<__m256, 0, 0, 0>{ public: typedef __m256i ret; };
template<> class TypeCvt<__m256d, 0, 0, 0>{ public: typedef __m256i ret; };

template<> class TypeCvt<__m128, 1, 0, 0>{ public: typedef __m256i ret; };
template<> class TypeCvt<__m128, 0, 1, 0>{ public: typedef __m256i ret; };
template<> class TypeCvt<__m128d, 1, 0, 0>{ public: typedef __m256i ret; };
template<> class TypeCvt<__m128d, 0, 1, 0>{ public: typedef __m256i ret; };

template<> class TypeCvt<__m256, -1, 0, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m256, 0, -1, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m256d, -1, 0, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m256d, 0, -1, 0>{ public: typedef __m128i ret; };

/**
 * int/int-256 v.s. int/float-256
 */
template<> class TypeCvt<__m256i, 2, 3, 1>{ public: typedef __m256d ret; };
template<> class TypeCvt<__m256i, 3, 2, 1>{ public: typedef __m256 ret; };
template<> class TypeCvt<__m256i, 0, 5, 0>{ public: typedef __m256i ret; };
template<> class TypeCvt<__m256i, 1, 4, 0>{ public: typedef __m256i ret; };
template<> class TypeCvt<__m256i, 2, 3, 0>{ public: typedef __m256i ret; };
template<> class TypeCvt<__m256i, 3, 2, 0>{ public: typedef __m256i ret; };
template<> class TypeCvt<__m256i, 4, 1, 0>{ public: typedef __m256i ret; };
template<> class TypeCvt<__m256i, 5, 0, 0>{ public: typedef __m256i ret; };
#endif
    
} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_TYPE_TRAIT_H_