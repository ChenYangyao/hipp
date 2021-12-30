#include <mpi_test_incl.h>

namespace HIPP::MPI {

struct CommTest : MPITestFixture {
    CommTest(Comm _comm)
    : MPITestFixture("CommTest", _comm) {
        HIPPMPI_TEST_F_ADD_CASE(comm_info);
        HIPPMPI_TEST_F_ADD_CASE(comm_printing);
    }
    void comm_info() {
        /**
        rank and size
        */
        int raw_rank, raw_n_procs;
        MPI_Comm_rank(comm.raw(), &raw_rank);
        MPI_Comm_size(comm.raw(), &raw_n_procs);

        assert_eq(rank, raw_rank);
        assert_eq(n_procs, raw_n_procs);

        /**
        special communicators
        */
        int dims[] = {2,2}, periods[] = {1,1};
        auto null_comm = Comm::nullval(), 
            self = Comm::selfval(),
            world = Comm::world(),
            cart = comm.cart_create(dims, periods);
   
        assert_false(comm.is_inter());
        assert_false(self.is_inter());
        assert_false(world.is_inter());
        assert_false(cart.is_inter());

        assert_false(comm.is_null());
        assert_false(self.is_null());
        assert_false(world.is_null());
        assert_false(cart.is_null());
     
        assert_true(null_comm.is_null());
        assert_eq(null_comm.raw(), MPI_COMM_NULL);
    }
    void comm_printing() {
        int dims[] = {2,2}, periods[] = {1,1};
        auto cart = comm.cart_create(dims, periods), 
            null = Comm::nullval(), 
            self = Comm::selfval(),
            world = Comm::world();
        if( rank == 0 ) {
            pout << cart;
            pout << null;
            pout << self;
            pout << world;

            cart.info(cout, 0);
            null.info(cout, 0);
            self.info(cout, 0);
            world.info(cout, 0);

            cart.info(cout, 1);
            null.info(cout, 1);
            self.info(cout, 1);
            world.info(cout, 1);
        }
    }
};


} // namespace HIPP::MPI

int main(int argc, char *argv[]){
    using namespace HIPP::MPI;
    
    Env env(argc, argv);
    auto comm = env.world();
    
    CommTest{comm}();
    
    return 0;
}
