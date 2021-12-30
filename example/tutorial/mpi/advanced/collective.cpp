#include <hippmpi.h>
#include <hippnumerical.h>

using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;
namespace ALG = HIPP::ALGORITHM;


void barrier() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();

    for(int i=0; i<n_procs; ++i){
        if( rank == i )
            pout << "Process ", rank, endl;
        comm.barrier();
    }

    {
        mpi::SeqBlock _{comm};
        pout << "Process ", rank, endl;
    }
}

void bcast() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();

    int root = 0, buf1[5];
    comm.bcast(buf1, root);

    float *buf2 = new float [10];
    comm.bcast(buf2, 10, mpi::FLOAT, root);
    delete [] buf2;
}

void gather_scatter() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();

    int root = 0;
    {
        vector<int> send_buf(8), recv_buf(8*n_procs);
        comm.gather(send_buf, recv_buf, root);

        if( rank == root ) {
            comm.gather(send_buf, recv_buf, root);
        } else {
            comm.gather(send_buf, {}, root);
        }

        if( rank == root ) {
            comm.gather(mpi::IN_PLACE, recv_buf.data(), 
                8, mpi::INT, root);
        } else {
            comm.gather(send_buf, {}, root);
        }
    }

    {
        vector<int> send_buf(8*n_procs), recv_buf(8);
        comm.scatter(send_buf, recv_buf, root);
    }

    {
        int send_buf;
        vector<int> recv_buf(2*n_procs), counts(n_procs, 1), 
            displs = ALG::LinSpaced(0, n_procs, 2).get();
        comm.gatherv(send_buf, recv_buf, counts, displs, root);
    }

    {
        vector<int> send_buf(2*n_procs), counts(n_procs, 1),
            displs = ALG::LinSpaced(0, n_procs, 2).get();
        int recv_buf;
        comm.scatterv(send_buf, counts, displs, recv_buf, root);
    }
}

void reduce() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();

    int root = 0, send_buf, recv_buf;
    comm.reduce(send_buf, recv_buf, mpi::SUM, root);

    comm.allreduce(send_buf, recv_buf, mpi::SUM);
}

int main(int argc, char const *argv[]) {
    mpi::Env env;
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();

    barrier();
    comm.barrier();
    
    bcast();
    comm.barrier();

    gather_scatter();
    comm.barrier();

    reduce();
    comm.barrier();

    return 0;
}
