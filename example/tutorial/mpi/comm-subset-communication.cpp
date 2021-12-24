#include <hippmpi.h>

using namespace HIPP;
using namespace HIPP::MPI;


int main(int argc, char *argv[]) {
    Env env (argc, argv);
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();

    int color = (rank == n_procs - 1) ? UNDEFINED : 0,
        key = 0;
    auto sub_comm = comm.split(color, key);

    if( rank != n_procs - 1 ) {
        int data[5] {0,1,2,3,4}, root = 0;
        sub_comm.bcast(data, root);
        pout << "At rank ", rank, ", data = {", 
            pout(data, data+5), "}", endl;
    }


    return 0;
}
