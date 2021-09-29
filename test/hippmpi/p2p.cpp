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

    auto cart = comm.cart_create({2,2}, {0,1});
    auto null_comm = comm.nullval();
    // Test printing
    if( rank == 0 ) {
        pout << comm;
        comm.info(cout, 1);
        comm.info(cout, 0) << endl;
        
        pout << null_comm;
        null_comm.info(cout, 1);
        null_comm.info(cout, 0) << endl;
    
        pout << cart;
        cart.info(cout, 1);
        cart.info(cout, 0) << endl;
        
        HIPPMPI_TEST_PROGRESS, "info and operator<< OK\n", endl;
    }
}

void test_send(Comm &comm){
    int rank = comm.rank(), n_procs = comm.size();
    if( rank == 0 )
        HIPPMPI_TEST_ENTERF

    assert(n_procs >= 2);

    TestData td, td_targ; td_targ.init(); 
    HIPPMPI_TEST_PROGRESS, "data initialized", endl;
    comm.barrier();

    if( rank == 0 ){
        td.init();
        {
            ConstDatapacket dpx{td.x};
            auto &[p,n,dt] = dpx;
            assert(p == (void*)&td.x);
            assert(n == 1);
            assert(dt.raw() == MPI_INT);
            HIPPMPI_TEST_PROGRESS, "send x begins", endl;
            comm.send(1,0,td.x);
            HIPPMPI_TEST_PROGRESS, "send x", endl;
        }

        ConstDatapacket dpy{td.y};
        HIPPMPI_TEST_PROGRESS, "send y begins", endl;
        comm.send(1,1,td.y);
        HIPPMPI_TEST_PROGRESS, "send y", endl;
        
        ConstDatapacket dpz{td.z};
        HIPPMPI_TEST_PROGRESS, "send z begins", endl;
        comm.send(1,2,td.z);
        HIPPMPI_TEST_PROGRESS, "send z", endl;
        
        ConstDatapacket dpptr{td.ptr, td.sz_buff};
        HIPPMPI_TEST_PROGRESS, "send (ptr,sz_buff) begins", endl;
        comm.send(1,3,td.ptr, td.sz_buff);
        HIPPMPI_TEST_PROGRESS, "send (ptr,sz_buff)", endl;
        
        ConstDatapacket dparr{td.arr};
        HIPPMPI_TEST_PROGRESS, "send arr begins", endl;
        comm.send(1,4,td.arr);
        HIPPMPI_TEST_PROGRESS, "send arr", endl;
    }else if( rank == 1 ){
        td.init_empty();
        {
            comm.recv(0,0,td.x);
            ConstDatapacket dpx{td.x};
            auto &[p,n,dt] = dpx;
            assert(p == (void*)&td.x);
            assert(n == 1);
            assert(dt.raw() == MPI_INT);
            HIPPMPI_TEST_PROGRESS, "send x begins", endl;
        }

        HIPPMPI_TEST_PROGRESS, "recv x", endl;
        comm.recv(0,1,td.y);
        HIPPMPI_TEST_PROGRESS, "recv y", endl;
        comm.recv(0,2,td.z);
        HIPPMPI_TEST_PROGRESS, "recv z", endl;
        comm.recv(0,3,td.ptr, td.sz_buff);
        HIPPMPI_TEST_PROGRESS, "recv (ptr, sz_buff)", endl;
        comm.recv(0,4,td.arr);
        HIPPMPI_TEST_PROGRESS, "recv arr", endl;
        td_targ.compare(td);
    }
    comm.barrier();
    if( rank == 0 )
        HIPPMPI_TEST_PROGRESS, "send OK\n", endl;
}

} // namespace HIPP::MPI

using namespace HIPP::MPI;
using namespace HIPP;

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;
    auto comm = env.world();
    
    HIPP::MPI::test_comm_info(comm);
    comm.barrier();

    HIPP::MPI::test_send(comm);
    comm.barrier();
    
    return 0;
}
