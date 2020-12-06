/* basic-p2p.cpp */
#include <hippmpi.h>
using namespace std;

int main(int argc, char const *argv[])
{
    HIPP::MPI::Env env;                                 // [1] Initialize MPI environment.
    auto comm = env.world();                            // The world communicator.

    if( comm.rank() == 0 ){                             // [2] use rank()
        vector<double> send_buff(10, 0.);
        int dest = 1, tag = 0;
        comm.send(dest, tag, send_buff);                // [3] Send out the whole vector.
    }else if( comm.rank() == 1 ){
        vector<double> recv_buff(20, 0.);               // Prepare a larger buffer.
        int src = 0, tag = 0;
        auto status = comm.recv(src, tag, recv_buff);   // [4] Receive into the buffer.

        /* Find the no. of values actually received, and print it. */
        int count = status.count(HIPP::MPI::DOUBLE);    
        HIPP::pout << "Received ", count, " values: ";
        HIPP::prt_a(cout, recv_buff.data(), recv_buff.data()+count) << endl;
    }
    
    return 0;
}
