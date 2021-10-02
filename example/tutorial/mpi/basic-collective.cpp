/* basic-collective.cpp */
#include <hippmpi.h>
using namespace HIPP;
using namespace HIPP::MPI;
using namespace std;

void overview_collective(Comm comm) {
    const int rank = comm.rank(), n_procs = comm.size();

    comm.barrier();

    int buff[4] {}, root=0;
                             
    comm.bcast(buff, root);                      // bcast the whole raw-array
    comm.bcast(buff[0], root);                   // bcast a scalar
    comm.bcast({buff, 2}, root);                 // bcast the first two values

    comm.barrier();
    
    {
        vector<int> sendbuf(2), recvbuf(2*n_procs);
        comm.gather(sendbuf, recvbuf, root);
    }

    comm.barrier();
    {
        vector<int> sendbuf(rank+1, rank+1); 
        if( rank == root ) {
            vector<int> recvbuf(10), displs = {0,1,3,6}, counts = {1,2,3,4};
            comm.gatherv(sendbuf, recvbuf, counts, displs, root);
            pout << "recvbuf={", recvbuf, "}", endl;
        }else{
            comm.gatherv(sendbuf, {}, {}, {}, root);
        }
    }
    comm.barrier();
    {
        vector<int> sendbuf(2*n_procs), recvbuf(2);
        comm.scatter(sendbuf, recvbuf, root);
    }
    comm.barrier();
    {
        vector<int> recvbuf(rank+1); 
        if( rank == root ) {
            vector<int> sendbuf = {1,2,2,3,3,3,4,4,4,4}, 
                displs = {0,1,3,6}, counts = {1,2,3,4};
            comm.scatterv(sendbuf, counts, displs, recvbuf, root);
        }else{
            comm.scatterv({}, {}, {}, recvbuf, root);
        }
        SeqBlock seq {comm};
        pout << "recvbuf={", recvbuf, "}", endl;
    }
    comm.barrier();

    vector<int> operands(5), results(5);
    comm.reduce(operands, results, SUM, root);         
    comm.reduce(operands, results, "+", root);      // use name of the operator

    comm.barrier();
}

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;                                 
    auto comm = env.world();                            
    int rank = comm.rank(), n_procs = comm.size();

    overview_collective(comm);

    return 0;
}
