#include <mystd.h>
#include <x86intrin.h>
using namespace MYSTD;

__m128 addtwo( float c ){
    union u_sp4_t{ __m128 x4; float x[4]; };
    u_sp4_t u_sp4, x;

    u_sp4.x[1] += 1;

    //u_sp4.x4 = _mm_set_ps1(c);
    x.x4 = _mm_add_ps( u_sp4.x4, u_sp4.x4 );

    return x.x4;
}