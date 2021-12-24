#include <hippmpi.h>
using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;

void cartesian() {
    auto comm = mpi::Env::world();

    int dims[2] = {2,2}, periods[2] = {1,1};
    auto cart = comm.cart_create(dims, periods);

    int rank = cart.rank();
    auto coords = cart.cart_coords(rank);
    auto [top, bottom] = cart.cart_shift(0);
    auto [left, right] = cart.cart_shift(1);

    pout << "Process ", rank, " is at coordinates (", coords, 
        ") with neighbors {", left, ", ", right, 
        ", ", top, " and ", bottom, "}", endl;
}

void graph() {
    auto comm = mpi::Env::world();

    int nnodes = 4, index[4] = {2, 4, 6, 8}, 
        edges[8] = {3, 1, 0, 2, 1, 3, 2, 0};
    auto graph = comm.graph_create(nnodes, index, edges);

    int rank = graph.rank();
    auto ngbs = graph.graph_neighbors(rank);
    pout << "Process ", rank, " has neighbors {", ngbs, "}", endl;
}

void dist_graph() {
    auto comm = mpi::Env::world();
    
    int rank = comm.rank(), n_procs = comm.size(),
        left = (rank - 1 + n_procs) % n_procs, right = (rank + 1) % n_procs;

    int srcs[1] = {rank}, degrees[1] = {2}, dsts[2] = {left, right}, 
        reorder = 0;
    auto dgraph = comm.dist_graph_create(srcs, degrees, dsts, 
        mpi::Info::nullval(), reorder);

    auto [ngb_srcs, ngb_dsts] = dgraph.dist_graph_neighbors();
    pout << "Process ", rank, " has incoming neighbors {" , ngb_srcs, "} "
        "and outgoing neighbors {", ngb_dsts, "}", endl;

    int send_data = rank, recv_buff[2];
    dgraph.neighbor_allgather(send_data, recv_buff);
    pout << "Process ", rank, " gathered {", 
        pout(recv_buff, recv_buff+2), "}", endl;
}

int main(int argc, char const *argv[]) {
    mpi::Env env;
    auto comm = env.world();

    assert( comm.size() == 4 );

    //cartesian();
    //graph();
    dist_graph();

    return 0;
}
