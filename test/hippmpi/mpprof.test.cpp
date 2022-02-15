#include <hippmpi.h>
#include <testmacro.h>
namespace HIPP::MPI {

typedef DefaultLogger::msg_t msg_t;


void test_log_open_and_close(Comm &comm) {
    int rank = comm.rank(), n_procs = comm.size();
    if( rank == 0 )
        HIPPMPI_TEST_ENTERF
    DefaultLogger lg(comm, "MPProfTestLogOpenAndClose.h5");
    if( rank == 0 )
        HIPPMPI_TEST_PROGRESS, "open file OK", endl;
}

void test_log_sequential(Comm &comm) {
    int rank = comm.rank(), n_procs = comm.size();
    if( rank == 0 )
        HIPPMPI_TEST_ENTERF
    DefaultLogger lg(comm, "MPProfTestLogSequential.h5");
    if( rank == 0 )
        HIPPMPI_TEST_PROGRESS, "open file OK", endl;
    lg.tick(str("P", rank, '/', n_procs, " begins"));
    vector<int> a(99); size_t sz = a.size()*sizeof(int);
    {
        auto _ = lg.duration(msg_t::sized(sz, " bytes initialized"));
        std::fill_n(a.data(), a.size(), 0);
    }
    if( rank == 0 )
        HIPPMPI_TEST_PROGRESS, "Sequential log OK", endl;
}

void test_log_p2p(Comm &comm) {
    int rank = comm.rank(), n_procs = comm.size();
    if( rank == 0 )
        HIPPMPI_TEST_ENTERF
    DefaultLogger lg(comm, "MPProfTestLogP2P.h5");
    if( rank == 0 )
        HIPPMPI_TEST_PROGRESS, "open file OK", endl;
    lg.tick(str("P", rank, '/', n_procs, " begins"));
    vector<int> a(99); size_t sz = a.size()*sizeof(int);
    {
        auto _ = lg.duration(msg_t::sized(sz, " bytes initialized"));
        std::fill_n(a.data(), a.size(), 0);
    }
    assert( n_procs >= 2 );
    if( rank == 0 ){
        for(int i=1; i<n_procs; ++i){
            auto _ = lg.send(i, 0, msg_t::sized(sz, str("sends to P", i)));
            comm.send(i, 0, a);
        }
    }else{
        auto _ = lg.recv(0,0, str("P", rank, "receives"));
        comm.recv(0,0, a);
    }
    if( rank == 0 )
        HIPPMPI_TEST_PROGRESS, "data transfer OK", endl;
}

void test_log_collective(Comm &comm) {
    int rank = comm.rank(), n_procs = comm.size();
    if( rank == 0 )
        HIPPMPI_TEST_ENTERF
    DefaultLogger lg(comm, "MPProfTestLogCollective.h5");
    if( rank == 0 )
        HIPPMPI_TEST_PROGRESS, "open file OK", endl;
    lg.tick(str("P", rank, '/', n_procs, " begins"));
    vector<int> a(99); size_t sz = a.size()*sizeof(int);
    {
        auto _ = lg.duration(msg_t::sized(sz, " bytes initialized"));
        std::fill_n(a.data(), a.size(), 0);
    }
    assert( n_procs >= 2 );
    {
        auto _= lg.barrier("sync");
        comm.barrier();
    }
    {
        auto _= lg.bcast(msg_t::sized(sz, " bytes bcasted"));
        comm.bcast(a, 0);
    }
}

} // namespace HIPP::MPI

int main(int argc, char *argv[]){
    HIPP::MPI::Env env(argc, argv);   
    auto comm = env.world();

    HIPP::MPI::test_log_open_and_close(comm);
    HIPP::MPI::test_log_sequential(comm);
    HIPP::MPI::test_log_p2p(comm);
    HIPP::MPI::test_log_collective(comm);

    return 0;
}
