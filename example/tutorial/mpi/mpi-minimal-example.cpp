// mpi-minimal-example.cpp
#include <hippmpi.h> 

using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;                              // [1]

int main(int argc, char *argv[]) {
    mpi::Env env(argc, argv);                           // [2]

    auto comm = env.world();                            // [3]
    int rank = comm.rank(), n_procs = comm.size();      // [4]
    
    pout << "Process ", rank, " among ", n_procs, endl;

    return 0;
}
