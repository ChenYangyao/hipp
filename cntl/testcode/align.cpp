#include <x86intrin.h>
#include <mystd.h>
using namespace MYSTD;

struct dv_t
{
    double d;
    __m256d v;
};


int main(int argc, char const *argv[])
{
    cout << sizeof( dv_t ) << " and " << alignof(dv_t) << endl;
    return 0;
}
