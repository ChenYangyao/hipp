#include <hippmpi.h>
using namespace std;

int main(int argc, char const *argv[]) {
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();

    /**
     * Create the window object by attaching a memory buffer from each process
     * to it.
     */
    vector<int> buff(2);
    void *base = buff.data();
    int disp_unit = sizeof(int), buff_size = disp_unit*buff.size();
    auto win = comm.win_create(base, buff_size, disp_unit);

    /**
     * Use RMA operation "put" to transfer data to a remote buffer.
     */
    int prev = (rank != 0) ? (rank-1) : (n_procs-1),
        next = (rank != n_procs-1) ? (rank+1) : 0;
    win.fence();
    win.put(next, rank, 0);
    win.put(prev, rank, 1);
    win.fence();

    {
        HIPP::MPI::SeqBlock seq(comm);
        HIPP::pout << "rank=", rank, ", buff=(", buff, ")", endl;
    }
    
    return 0;
}
