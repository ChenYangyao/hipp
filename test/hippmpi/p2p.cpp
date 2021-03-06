#include <testdata.h>
#include <testmacro.h>

namespace HIPP::MPI {
    
void test_comm_info(Comm &comm){
    int rank, n_procs;
    MPI_Comm_rank(comm.raw(), &rank);
    if( rank == 0 )
        HIPPMPI_TEST_ENTERF

    assert(comm.rank() == rank);
    if( rank == 0 )
        HIPPMPI_TEST_PROGRESS, "rank OK", endl;

    MPI_Comm_size(comm.raw(), &n_procs);
    assert(comm.size() == n_procs);
    if( rank == 0 )
        HIPPMPI_TEST_PROGRESS, "size OK", endl;
}

void test_send(Comm &comm){
    int rank = comm.rank(), n_procs = comm.size();
    if( rank == 0 )
        HIPPMPI_TEST_ENTERF

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
        HIPPMPI_TEST_PROGRESS, "send OK", endl;
}

} // namespace HIPP::MPI

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;
    auto comm = env.world();
    
    HIPP::MPI::test_comm_info(comm);
    HIPP::MPI::test_send(comm);
    
    return 0;
}
