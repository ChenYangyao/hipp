#include <hippmpi.h>
#include <unistd.h>         // sleep() system call

int main(int argc, char const *argv[]){
    /**
     * Init the MPI environment, and get the communication world.
     */
    HIPP::MPI::Env env;
    auto world = env.world();
    
    /**
     * Split the world into two disjoint communicator.
     * The processes with odd ranks in the communicator world is grouped into
     * one communicator, those with even ranks into another.
     * This is equivalent to call MPI_Comm_split().
     */
    auto comm = world.split( world.rank() % 2 );

    /**
     * Print the infomation of the new communicator into std::cout.
     * sleep() avoids the printing entangled among processes. 
     * 
     * If invoked as `mpiexec -n 4`, the result is like:
     * HIPP::MPI::Comm instance [loc=0x7ffdacf11a10, size=16, align=8]
     * ----------
     * Size info (size=2, rank=0)
     * HIPP::MPI::Comm instance [loc=0x7fff6bd487d0, size=16, align=8]
     * ----------
     * Size info (size=2, rank=0)
     * HIPP::MPI::Comm instance [loc=0x7ffe8d568d90, size=16, align=8]
     * ----------
     * Size info (size=2, rank=1)
     * HIPP::MPI::Comm instance [loc=0x7fff32b9fc70, size=16, align=8]
     * ----------
     * Size info (size=2, rank=1)
     */
    sleep( world.rank()+1 );
    comm.info();

    return 0;
}
