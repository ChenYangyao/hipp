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
 * f128 & f128f/64
 */
template<> class TypeCvt<__m128, -1, 1, 1>{ public: typedef __m128d ret; };
template<> class TypeCvt<__m128, -1, 0, 1>{ public: typedef __m64 ret; };
template<> class TypeCvt<__m128d, 1, -1, 1>{ public: typedef __m128 ret; };
template<> class TypeCvt<__m128d, 0, -1, 1>{ public: typedef __m64 ret; };

/**
 * f128 & i128/i64
 */
template<> class TypeCvt<__m128, 0, 0, 0>{ public: typedef __m128i ret; };  // 4x32
template<> class TypeCvt<__m128, -1, 0, 0>{ public: typedef __m64 ret; };   // 2x32
template<> class TypeCvt<__m128, 0, -1, 0>{ public: typedef __m64 ret; };   // 4x16
template<> class TypeCvt<__m128, 1, -2, 0>{ public: typedef __m64 ret; };   // 8x8
template<> class TypeCvt<__m128d, 0, 0, 0>{ public: typedef __m128i ret; }; 
template<> class TypeCvt<__m128d, 0, -1, 0>{ public: typedef __m64 ret; };  // 2x32
template<> class TypeCvt<__m128d, 1, -2, 0>{ public: typedef __m64 ret; };  // 4x16
template<> class TypeCvt<__m128d, 2, -3, 0>{ public: typedef __m64 ret; };  // 8x8 


/**
 * int/int-128 v.s. int/float-128
 */
template<> class TypeCvt<__m128i, 0, 4, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m128i, 1, 3, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m128i, 2, 2, 0>{ public: typedef __m128i ret; }; // 4x32
template<> class TypeCvt<__m128i, 3, 1, 0>{ public: typedef __m128i ret; };
template<> class TypeCvt<__m128i, 4, 0, 0>{ public: typedef __m128i ret; }; // 16x8
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
 * i256 to f256/i256
 */
template<> class TypeCvt<__m256i, 2, 3, 1>{ public: typedef __m256d ret; };     // 4 x 64
template<> class TypeCvt<__m256i, 3, 2, 1>{ public: typedef __m256 ret; };      // 8 x 32

template<> class TypeCvt<__m256i, 0, 5, 0>{ public: typedef __m256i ret; };     // 1 x 256
template<> class TypeCvt<__m256i, 1, 4, 0>{ public: typedef __m256i ret; };     // 2 x 128
template<> class TypeCvt<__m256i, 2, 3, 0>{ public: typedef __m256i ret; };     // 4 x 64
template<> class TypeCvt<__m256i, 3, 2, 0>{ public: typedef __m256i ret; };     // 8 x 32 (i.e., 8 signed/unsigned int)
template<> class TypeCvt<__m256i, 4, 1, 0>{ public: typedef __m256i ret; };     // 16 x 16
template<> class TypeCvt<__m256i, 5, 0, 0>{ public: typedef __m256i ret; };     // 32 x 8 (i.e., 32 singned/unsigned char)
#endif
    
} // namespace SIMD
} // namespace HIPP
#endif	//_HIPPSIMD_TYPE_TRAIT_H_