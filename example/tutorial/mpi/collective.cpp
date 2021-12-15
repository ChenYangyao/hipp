#include <hippmpi.h>
using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;

void collective() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();

    comm.barrier();

    int root = 0;
    double buff[4];
    comm.bcast(buff, root);

    int data;
    if( rank == root ) {
        vector<int> buff(n_procs);
        comm.gather(data, buff, root);
    } else {
        comm.gather(data, {}, root);
    }

    if( rank == root ) {
        vector<int> buff(2*n_procs), counts(n_procs, 1), displs(n_procs);
        for(int i=0; i<n_procs; ++i) displs[i] = 2*i;
        comm.gatherv(data, buff, counts, displs, root);
    } else {
        comm.gatherv(data, {}, {}, {}, root);
    }

    int total;
    comm.reduce(data, total, mpi::SUM, root);
}

int main(int argc, char const *argv[]) {
    mpi::Env env;

    collective();

    return 0;
}
