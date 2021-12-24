#include <hippmpi.h>

using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;

int main(int argc, char *argv[]) {
    mpi::Env env(argc, argv);
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();

    vector<double> buff(10);

    if( rank == 0 ) 
        comm.send(1, 0, buff);
    else if( rank == 1 ) 
        comm.recv(0, 0, buff);

    comm.bcast(buff, 0);
    
    return 0;
}
