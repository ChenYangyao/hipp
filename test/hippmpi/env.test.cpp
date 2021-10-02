#include <mpi_test_incl.h>

namespace HIPP::MPI {

struct EnvTest : MPITestFixture {
    EnvTest(Comm _comm, Env &_env) 
    : MPITestFixture("EnvTest", _comm), env(_env) {
        HIPPMPI_TEST_F_ADD_CASE(disp_info);
        HIPPMPI_TEST_F_ADD_CASE(version);
        HIPPMPI_TEST_F_ADD_CASE(runtime_env);
        HIPPMPI_TEST_F_ADD_CASE(world_comm);
    }
    void disp_info() {
        if( rank == 0 ) {
            pout << env;
            env.info(cout, 0);
            env.info(cout, 1);
        }
        comm.barrier();
        if( rank == 1 ) {
            pout << env;
            env.info(cout, 0);
            env.info(cout, 1);
        }
    }
    void version() {
        int v, subv;
        env.version(v, subv);
        assert_true(v>2, emFF);

        string libv = env.library_version();
        assert_true(libv.size() >= 0, "libversion=", libv);
    }
    void runtime_env(){
        int tag_ub = env.tag_ub(),
            host = env.host(),
            io = env.io(),
            wtime_is_global = env.wtime_is_global();
        expect_true(tag_ub > 0, "tag ub");
        expect_true(host >= 0 || host == MPI_PROC_NULL, "host");
        expect_true(io==MPI_ANY_SOURCE || io==MPI_PROC_NULL || io>=0, "io");
        expect_true(wtime_is_global>=0, "wtime_is_global");

        string pname = env.processor_name();
        assert_true(pname.size() >= 0, "processor_name=", pname);
    }
    void world_comm() {
        auto comm = Env::world();
        expect_eq_type<decltype(comm), Comm>("communicator types");
    }

private:
    Env &env;
};


} // namespace HIPP::MPI

int main(int argc, char *argv[])
{
    using namespace HIPP::MPI;

    Env env(argc, argv);
    auto comm = env.world();

    EnvTest(comm, env)();

    return 0;
}
