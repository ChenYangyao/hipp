#include <hippmpi.h>
using namespace std;

int main(int argc, char const *argv[]) {
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank();
    int tag = 0;

    if( rank == 0 ){
        /* Data to be packed and sent. */
        int x = 0; 
        double y[3] = {1.,2.,3.}; 
        vector<float> z = {4.0f, 5.0f, 6.0f};
        string s = "Hello World";
        int nz = z.size(), ns = s.size();

        /* Pack data into a single buffer, and send them once. */
        HIPP::MPI::Pack pack(comm);
        pack.push(x).push(y).push(nz).push(z).push(ns).push(s);
        comm.send(1, tag, pack.as_sendbuf());
    }else if(rank == 1){
        int x; double y[3]; vector<float> z;
        vector<char> s;
        int nz, ns;

        /** 
         * Probe the size of the message, preallocate the buffer, and receive 
         * message into the buffer.
         */
        HIPP::MPI::Pack pack(comm.probe(0, tag).count("packed"), comm);
        comm.recv(0, tag, pack.as_recvbuf());

        /* Pop out each data item. */
        pack.pop(x).pop(y);
        
        pack.pop(nz);
        z.resize(nz);
        pack.pop(z);
        
        pack.pop(ns);
        s.resize(ns+1);
        pack.pop(s); 
        s.back() = '\0';

        HIPP::pout << "x=", x, 
            ", y=(", HIPP::pout(y,y+3), ")", 
            ", z=(", z, ")", 
            ", s='", s.data(), "'\n";
    }
    
    return 0;
}
