#include <hippnumerical.h>

using namespace HIPP;
using namespace HIPP::NUMERICAL;

int main(int argc, char const *argv[]) {
    
    SArray<int, 2, 2, 3> a = {
        1,2,3,
        4,5,6,
        7,8,9,
        10,11,12};

    auto &[r0,r1] = a;      // r0, r1 refers to a SArray<int,2,3> subarray (i.e., matrix).

    auto is_same = r0 == SArray<int,2,3>{1,2,3,4,5,6};
    assert( is_same.all() );

    auto &[r00, r01] = r0;  // r00, r01 refers to a SArray<int,3> subarray (i.e., vector).

    assert( r00[0] == 1 );
    assert( r00[1] == 2 );
    assert( r00[2] == 3 );


    typedef RawArrayTraits<SArray<int,3,4> > traits_t;
    assert( traits_t::rank == 2 );
    assert( traits_t::size == 12 );

    return 0;
}

