#include <hippmpi.h>

using namespace HIPP::MPI;
using namespace HIPP;

int main(int argc, char const *argv[]) {

    Env env;
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();

    assert( n_procs >= 2 );

    if( rank == 0 ) {
        int a[3] = {0, 1, 2};
        int dest = 1, tag = 0;

        comm.send(dest, tag, &a[0], 3, INT);      // Use the standard MPI style.
        
        comm.send(dest, tag, a);                  // Use the HIPP style.
        
        // The underlying implementation.
        auto [buff, size, dtype] = ConstDatapacket {a};      // Construct the Datapacket and extract buffer specificaton.
        
        assert(buff == (void *)&a[0]);
        assert(size == 3);
        assert(dtype.raw() == MPI_INT);
        
        comm.send(dest, tag, buff, size, dtype);        // Forward the buffer specification.


    }else if( rank == 1 ) {
        for(int i=0; i<3; ++i){
            int a[3] {};
            int src = 0, tag = 0;
            comm.recv(src, tag, a);
            pout << "receiving {", pout(a, a+3), "}\n";
        }
    }
    
    return 0;
}

