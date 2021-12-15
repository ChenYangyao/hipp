#include <hippmpi.h>
using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;

void send_and_recv() {
    auto comm = mpi::Env::world();
    int rank = comm.rank();

    double x_send, x_recv;
    int tag = 0;
    if( rank == 0 ) {
        comm.send(1, tag, x_send);
    } else if( rank == 1 ) {
        comm.recv(0, tag, x_recv);
    }

    int raw_arr[3][4];
    std::array<float, 3> arr;
    vector<double> vec(10);
    if( rank == 0 ) {
        comm.send(1, tag, raw_arr);
        comm.send(1, tag, arr);
        comm.send(1, tag, vec);

        comm.send(1, tag, &raw_arr[0][0], 12, mpi::INT);
    } else if( rank == 1 ) {
        comm.recv(0, tag, raw_arr);
        comm.recv(0, tag, arr);
        comm.recv(0, tag, vec);

        comm.recv(0, tag, &raw_arr[0][0], 12, mpi::INT);
    }
}

int main(int argc, char const *argv[]) {
    mpi::Env env;

    send_and_recv();

    return 0;
}
