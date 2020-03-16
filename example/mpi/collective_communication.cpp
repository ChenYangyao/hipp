/**
 * Example: collective_communication.cpp
 * This example demonstrates how to make collective communication using HIPP
 * MPI interface.
 */
#include <hippmpi.h>
using namespace std;

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;
    auto world = env.world();
    
    vector<double> rands;
    if( world.rank() == 0 ){
        for(int i=0; i<world.size(); ++i)
            rands.push_back(i);
        shuffle( rands.begin(), rands.end(), 
            default_random_engine() );
        HIPP::prt(cout, "Random numbers are: ");
        HIPP::prt_a(cout, rands) << endl;
    }
    
    double local_rand;
    world.scatter( rands.data(), 1, HIPP::MPI::DOUBLE,
        &local_rand, 1, HIPP::MPI::DOUBLE, 0);


    struct double_int_t{ double value; int rank; } 
        in = {local_rand, world.rank()}, out;
    world.reduce( &in, &out, 1, HIPP::MPI::DOUBLE_INT, 
        "maxloc", 0 );
    if( world.rank() == 0 )
        HIPP::prt(cout, "Max element is ", out.value, 
            " at process ", out.rank) << endl;

    
    return 0;
}

