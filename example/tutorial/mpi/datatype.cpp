#include <hippmpi.h>
using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;

void predefined() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size(), tag = 0;

    double buff[4];
    {
        auto &dt = mpi::DOUBLE;
        if( rank == 0 ) {
            comm.send(1, tag, buff, 4, dt);
        } else {
            comm.recv(0, tag, buff, 4, dt);
        }
    }

    {
        auto &dt = mpi::Datatype::from_type<double>();
        if( rank == 0 ) {
            comm.send(1, tag, buff, 4, dt);
        } else {
            comm.recv(0, tag, buff, 4, dt);
        }
    }

    {
        auto &dt = mpi::Datatype::from_name("double");
        if( rank == 0 ) {
            comm.send(1, tag, buff, 4, dt);
        } else {
            comm.recv(0, tag, buff, 4, dt);
        }
    }
}

void customized(){
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size(), root = 0;

    double mat[4][4];
    
    auto dt_row = mpi::DOUBLE.contiguous(4);
    comm.bcast(&mat[0][0], 2, dt_row, root);        // Broadcast the first two rows.

    int count = 4, blklen = 1, stride = 4;
    auto dt_col = mpi::DOUBLE.vector(count, blklen, stride).resized(0, sizeof(int));
    comm.bcast(&mat[0][0], 2, dt_col, root);        // Broadcast the first two columns.

    int sizes[2] = {4,4}, subsizes[2] = {2,2}, starts[2] = {1, 1};
    auto dt_subarray = mpi::DOUBLE.subarray(sizes, subsizes, starts);
    comm.bcast(&mat[0][0], 1, dt_subarray, root);   // Broadcast a 2x2 sub-array.
}

int main(int argc, char const *argv[]) {
    mpi::Env env;

    predefined();
    customized();

    return 0;
}
