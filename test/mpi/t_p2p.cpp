#include "testdata.h"

namespace HIPP::MPI {
    
void test_comm_info(Comm &comm){
    int rank, n_procs;
    MPI_Comm_rank(comm.raw(), &rank);
    assert(comm.rank() == rank);

    MPI_Comm_size(comm.raw(), &n_procs);
    assert(comm.size() == n_procs);

    if( rank == 0 ){
        pout << "t_p2p ... test_comm_info\n  -- finished successful", endl;
    }
}

void test_send(Comm &comm){
    int rank = comm.rank(), n_procs = comm.size();
    assert(n_procs >= 2);

    TestData td, td_targ; td_targ.init(); 
    if( rank == 0 ){
        td.init();
        comm.send(1,0,td.x);
        comm.send(1,1,td.y);
        comm.send(1,2,td.z);
    }else if( rank == 1 ){
        td.init_empty();
        comm.recv(0,0,td.x);
        comm.recv(0,1,td.y);
        comm.recv(0,2,td.z);
        td_targ.compare(td);
    }
    comm.barrier();
    if( rank == 0 )
        pout << "t_p2p ... test_send\n  -- finished succesful", endl;
}

} // namespace HIPP::MPI

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;
    auto comm = env.world();
    
    HIPP::MPI::test_comm_info(comm);
    HIPP::MPI::test_send(comm);
    
    return 0;
}
