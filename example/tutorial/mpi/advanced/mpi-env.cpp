#include <hippmpi.h>

using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;

int main(int argc, char *argv[]) {
    /*
    mpi::Env env;
    mpi::Env env(argc, argv);
    */
    
    int required = mpi::Env::THREAD_MULTIPLE, 
        provided;
    mpi::Env env(argc, argv, required, provided);
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();

    if( rank == 0 ) {
        pout << "Thread safety: ", 
                "required ", required, ", got ", provided, endl;

        pout << "version = ", HIPPMPI_STD_VERSION, ".", HIPPMPI_STD_SUBVERSION, endl;
        
        int ver, sub_ver;
        mpi::Env::version(ver, sub_ver);
        pout << "version = ", ver, ".", sub_ver, endl;
        
        pout << mpi::Env::library_version(), endl;

        pout << "Tag upper bound: ",     env.tag_ub(),           '\n',
                "Host rank: ",           env.host(),             '\n',
                "Rank of IO process: ",  env.io(),               '\n',
                "Wtime is global: ",     env.wtime_is_global(),  '\n',
                "Host machine: ",        env.processor_name(),   '\n';

        pout << env;
    }

    return 0;
}
