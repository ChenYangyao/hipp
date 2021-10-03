#include <hippmpi.h>

using namespace HIPP;
using namespace HIPP::MPI;
using namespace std;

int main(int argc, char const *argv[]) {
    Env env;
    Comm comm = env.world();

    string name = env.processor_name();
    int rank = comm.rank(), n_procs = comm.size();

    pout << "Host name ", name, '\n',
        "Rank ", rank, ", no. of processes ", n_procs, endl;

    comm.barrier();
    if(rank == 0) pout << comm;

    return 0;
}

