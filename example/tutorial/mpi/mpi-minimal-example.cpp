// mpi-minimal-example.cpp
#include <hippmpi.h>                                    // [1]

using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;                              // [2]

int main(int argc, char *argv[]) {
    mpi::Env env(argc, argv);                           // [3]

    auto comm = env.world();                            // [4]
    int rank = comm.rank(), n_procs = comm.size();      // [5]
    
    pout << "Process ", rank, " among ", n_procs, endl;

    return 0;
}
