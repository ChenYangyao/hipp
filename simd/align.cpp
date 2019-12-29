#include <mystd.h>
#include <my_random.h>
#include <x86intrin.h>
#include "header/mysimd.h"
using namespace MYSTD;

template<typename F = float, size_t N = 8>
class W{
public:
    typedef MYMATH::MMPacked<F, N> pack_t;
    typedef typename pack_t::vector_t vector_t;
    typedef typename pack_t::scalar_t scalar_t;
    W(){}
    W( const vector_t a): _a(a){ }
    W( const W &w ): _a(w.a){ }
    W( W &&w ): _a(w._a){ }
    W & operator=( const W &w ){ _a = w.a; return *this; }
    W & operator=( W &&w ){ _a = w.a; return *this; }
    W operator+( const W &w2 ){ return W( pack_t::add( _a, w2._a ) ); }
    W operator&&( const W &w2 ){ return W( pack_t::and_( _a, w2._a ) ); }
    W operator>=( const W &w2 ){ return W( pack_t::ge( _a, w2._a ) ); }

    void load( const scalar_t *p ){ _a = pack_t::load( p ); }
    void store( scalar_t *p ){ pack_t::store( p, _a ); }
    void zero(){ _a = pack_t::setzero(); }
private:
    vector_t _a;
};


int temp(float *p, int N)
{
    for(int i=0; i<N; i+=8){
        __m256 d = _mm256_load_ps( p+i );
        __m256 mask = _mm256_cmp_ps( d, _mm256_setzero_ps(), _CMP_GE_OQ );
        d = _mm256_and_ps( d, mask );
        _mm256_store_ps( p+i, d );
    }
    return 0;
}

int temp1(float *p, int N){
    typedef MYMATH::MMPacked<float, 8> mm;
    for(int i=0; i<N; i+=8){
        auto d = mm::load( p+i ), mask = mm::ge( d, mm::set() );
        mm::store( p+i, mm::and_( d, mask ) );
    }
    return 0;
}

int temp2(float *p, int N)
{
    typedef W<> Wt;
    Wt w1, w2;
    w2.zero();
    for(int i=0; i<N; i+=8){
        w1.load(p+i); 
        ( w1 >= w2 && w1 ).store(p+i);
    }
    return 0;
}

int main(int argc, char const *argv[]){
    int N = 1024 * 1024;
    float *vf = (float *)aligned_alloc( 32, N*sizeof(float) );
    
    MYMATH::RandomsFloat<float>(-1, 1).get(vf, N);
    MYFILE::print_range( vf, vf+12, cout, '\t' ) << endl;
    temp( vf, N );
    MYFILE::print_range( vf, vf+12, cout, '\t' ) << endl;

    MYMATH::RandomsFloat<float>(-1, 1).get(vf, N);
    MYFILE::print_range( vf, vf+12, cout, '\t' ) << endl;
    temp1( vf, N );
    MYFILE::print_range( vf, vf+12, cout, '\t' ) << endl;

    MYMATH::RandomsFloat<float>(-1, 1).get(vf, N);
    MYFILE::print_range( vf, vf+12, cout, '\t' ) << endl;
    temp2( vf, N );
    MYFILE::print_range( vf, vf+12, cout, '\t' ) << endl;

    free(vf);

    return 0;
}
