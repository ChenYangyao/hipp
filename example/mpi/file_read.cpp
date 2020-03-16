/**
 * Example: file_read.cpp
 * This example demonstrates how to use the file interface of HIPP MPI to 
 * perform basic reading.
 */
#include <hippmpi.h>
using namespace std;

int main(int argc, char const *argv[]){
    // number of data items read by each process and file name to be read from
    const int num_per_proc = 5;
    const string filename = "./file_read.dat";

    HIPP::MPI::Env env;
    auto comm = env.world();
    

    /**
     * we first create a file use ordinary C++ binary IO interface.
     * Here we write a vector of length = num ber process x number of data items
     * of each process into the file. The file will be read by each process 
     * later.
     * The content of this vector is 0,0,0, ..., 1,1,1, ..., 2,2,2, ..., where
     * the data items that will be read by each process is just the rank of this
     * process.
     */
    if( comm.rank() == 0 ){
        ofstream file( filename.c_str() );
        vector<double> outbuf(comm.size()*num_per_proc);
        for(size_t i=0; i<outbuf.size(); ++i)
            outbuf[i] = i / num_per_proc;
        file.write( (char *)outbuf.data(), outbuf.size()*sizeof(double) );
    }
    comm.barrier();


    /**
     * now we open the file and read its content parallelly.
     * HIPP::MPI::File is the file interface of HIPP MPI. After initialized with
     * "r" (read) mode, the file is opened and prepared to be read.
     * File opening is a collective operation as in the MPI stardard. Each 
     * process in the communicator `comm` must call it.
     * 
     * After open the file, each process prepares a buffer `inbuf`, seek the
     * indivisual file pointer, and read the content belonging to this process.
     */
    HIPP::MPI::File file( comm, filename, "r" );
    vector<double> inbuf( num_per_proc );    
    file.seek( comm.rank()*num_per_proc*sizeof(double), "cur" );
    file.read( inbuf );

    /**
     * the content read by each process is printed.
     * If run with, for example, 4 processes, in the output device it prints:
     * Process 0 reads: 0,0,0,0,0
     * Process 1 reads: 1,1,1,1,1
     * Process 2 reads: 2,2,2,2,2
     * Process 3 reads: 3,3,3,3,3
     */
    sleep(comm.rank());
    HIPP::prt( cout, "Process ", comm.rank(), " reads: " );
    HIPP::prt_a( cout, inbuf ) << endl;

    return 0;
}
