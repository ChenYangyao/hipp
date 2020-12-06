/* p2p-comm.cpp */
#include <hippcntl.h>
#include <hippmpi.h>
using namespace std;

int main(int argc, char *argv[]) 
{
    HIPP::MPI::Env env(argc, argv);             // Init the MPI environment
    auto comm = env.world();

    if( comm.rank() == 0 ){
        int tag = 0, dest_rank = 1;
        vector<int> out_arr {1,2,3,4,5};
        comm.send(dest_rank, tag, out_arr);     // Process 0 sends to 1
    }else if( comm.rank() == 1 ){
        int tag = 0, src_rank = 0;
        vector<int> in_arr(5);
        comm.recv(src_rank, tag, in_arr);       // Process 1 sends to 0
        HIPP::prt_a(cout, in_arr) << endl;
    }

    return 0;
}
