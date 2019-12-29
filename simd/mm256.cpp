#include "header/LHCsimd.h"
#include <mystd.h>
using namespace LHC;
using namespace SIMD;
using namespace MYSTD;

typedef Packed<double, 4> pd_t;

int main(int argc, char const *argv[]){
    
    pd_t::vec_t v1 = pd_t::set( 4., 3., 2., 1. );
    pd_t::scal_t *x1 = (pd_t::scal_t *)&v1;

    MYFILE::print_range(x1, x1+4) << endl;

    return 0;
}
