/**
 * Example: hello_world.cpp
 * This example demonstrates how to initialize MPI environment, get the 
 * information of each process in the communication world.
 */
#include <hippmpi.h>
using namespace std;


int main(int argc, char const *argv[]){
    /**
     * Once the Env variable is initialized, it init the MPI environment just
     * as calling MPI_Init(NULL, NULL). At the returning of main(), the Env
     * variable is deconstructed automatically so that MPI_Finalize() is called.
     *
     * Through the Env variable, you can get the communication world, which is
     * a high-level communicator class that acts as MPI_COMM_WORLD.
     */
    HIPP::MPI::Env env;
    auto comm = env.world();

    /**
     * The processor name can be retrived from Env vairable, and the the rank
     * and commnicator size can be retrived from the communicator variable.
     * 
     * The HIPP::prt() is a printing function accepts a stream object and 
     * arbitrary number of items to be printed into this stream.
     * 
     * In the output device it prints like:
     * 
     * Hello world from processor local-linux, rank 0 out of 4 processors
     * Hello world from processor local-linux, rank 1 out of 4 processors
     * Hello world from processor local-linux, rank 2 out of 4 processors
     * Hello world from processor local-linux, rank 3 out of 4 processors
     * 
     * In the different computer, the processor name may be different. It is OK
     * that sometimes the output from different processes get entangled.
     */
    HIPP::prt(cout, "Hello world from processor ", env.processor_name(),
        ", rank ", comm.rank(), " out of ", comm.size(), " processors") << endl;
    
    return 0;
}
