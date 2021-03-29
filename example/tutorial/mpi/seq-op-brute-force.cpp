#include <hippmpi.h>
using namespace std;


int main(int argc, char const *argv[]) {
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();

    for(int i=0; i<n_procs; ++i){
        if( rank == i ){
            // ... code to execute on one process at a time,
            // e.g.,
            HIPP::pout << "This is process ", rank, endl;
        }
        comm.barrier();
    }

    return 0;
}

