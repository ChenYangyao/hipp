#include <hippmpi.h>

using namespace HIPP;
using namespace HIPP::MPI;
using namespace std;

int main(int argc, char const *argv[]) {
    Env env;
    Comm comm = env.world();

    string name = env.processor_name();
    int rank = comm.rank();
    int n_procs = comm.size();

    if( rank == 0 ) {
        pout << 
            "Host name ", name, '\n',
            "Rank ", rank, '\n',
            "No. of processes ", n_procs, '\n';
        pout << comm, endl;
        comm.info(cout);
    }   

    return 0;
}

