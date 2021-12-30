#include <testdata.h>
#include <testmacro.h>
#include <mpi_test_incl.h>
#include <mpi_test_data_buffer.h>

namespace HIPP::MPI {

template<typename ScalarT>
struct PointToPointTest : MPITestFixture {
    using data_buffer_t = MPITestDataBuffer<ScalarT>;
    using scalar_t = typename data_buffer_t::scalar_t;

    PointToPointTest(Comm _comm, string scalar_name)
    : MPITestFixture("PointToPointTest_" + scalar_name, _comm) {
        HIPPMPI_TEST_F_ADD_CASE(blocking_standard);
        HIPPMPI_TEST_F_ADD_CASE(blocking_synchronous);
        HIPPMPI_TEST_F_ADD_CASE(blocking_buffered);
        HIPPMPI_TEST_F_ADD_CASE(blocking_ready);
        HIPPMPI_TEST_F_ADD_CASE(nonblocking_standard);
        HIPPMPI_TEST_F_ADD_CASE(nonblocking_synchronous);
        HIPPMPI_TEST_F_ADD_CASE(nonblocking_buffered);
        HIPPMPI_TEST_F_ADD_CASE(nonblocking_ready);
        HIPPMPI_TEST_F_ADD_CASE(persistent_standard);
        HIPPMPI_TEST_F_ADD_CASE(persistent_synchronous);
        HIPPMPI_TEST_F_ADD_CASE(persistent_buffered);
        HIPPMPI_TEST_F_ADD_CASE(persistent_ready);
    }
    void blocking_standard(){
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                comm.send(i, 0, b.x_s);
                comm.send(i, 0, b.x_rawarr3);
                comm.send(i, 0, b.x_rawarr34);
                comm.send(i, 0, b.x_arr3);
                comm.send(i, 0, b.x_arr34);
                comm.send(i, 0, b.x_v8);
                comm.send(i, 0, b.x_v83);
                comm.send(i, 0, b.x_v834);
                comm.send(i, 0, b.x_p8, 8);
                comm.send(i, 0, b.x_p8, 8, Datatype::from_type<scalar_t>());
            }
        } else {
                comm.recv(0, 0, b_in.x_s);
                comm.recv(0, 0, b_in.x_rawarr3);
                comm.recv(0, 0, b_in.x_rawarr34);
                comm.recv(0, 0, b_in.x_arr3);
                comm.recv(0, 0, b_in.x_arr34);
                comm.recv(0, 0, b_in.x_v8);
                comm.recv(0, 0, b_in.x_v83);
                comm.recv(0, 0, b_in.x_v834);
                comm.recv(0, 0, b_in.x_p8, 8);
                comm.recv(0, 0, b_in.x_p8, 8, Datatype::from_type<scalar_t>());
                b.expect_eq(b_in);
        }
    }
    void blocking_synchronous(){
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                comm.ssend(i, 0, b.x_s);
                comm.ssend(i, 0, b.x_rawarr3);
                comm.ssend(i, 0, b.x_rawarr34);
                comm.ssend(i, 0, b.x_arr3);
                comm.ssend(i, 0, b.x_arr34);
                comm.ssend(i, 0, b.x_v8);
                comm.ssend(i, 0, b.x_v83);
                comm.ssend(i, 0, b.x_v834);
                comm.ssend(i, 0, b.x_p8, 8);
                comm.ssend(i, 0, b.x_p8, 8, Datatype::from_type<scalar_t>());
            }
        } else {
                comm.recv(0, 0, b_in.x_s);
                comm.recv(0, 0, b_in.x_rawarr3);
                comm.recv(0, 0, b_in.x_rawarr34);
                comm.recv(0, 0, b_in.x_arr3);
                comm.recv(0, 0, b_in.x_arr34);
                comm.recv(0, 0, b_in.x_v8);
                comm.recv(0, 0, b_in.x_v83);
                comm.recv(0, 0, b_in.x_v834);
                comm.recv(0, 0, b_in.x_p8, 8);
                comm.recv(0, 0, b_in.x_p8, 8, Datatype::from_type<scalar_t>());
                b.expect_eq(b_in);
        }
    }
    void blocking_buffered(){
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                comm.bsend(i, 0, b.x_s);
                comm.bsend(i, 0, b.x_rawarr3);
                comm.bsend(i, 0, b.x_rawarr34);
                comm.bsend(i, 0, b.x_arr3);
                comm.bsend(i, 0, b.x_arr34);
                comm.bsend(i, 0, b.x_v8);
                comm.bsend(i, 0, b.x_v83);
                comm.bsend(i, 0, b.x_v834);
                comm.bsend(i, 0, b.x_p8, 8);
                comm.bsend(i, 0, b.x_p8, 8, Datatype::from_type<scalar_t>());
            }
        } else {
                comm.recv(0, 0, b_in.x_s);
                comm.recv(0, 0, b_in.x_rawarr3);
                comm.recv(0, 0, b_in.x_rawarr34);
                comm.recv(0, 0, b_in.x_arr3);
                comm.recv(0, 0, b_in.x_arr34);
                comm.recv(0, 0, b_in.x_v8);
                comm.recv(0, 0, b_in.x_v83);
                comm.recv(0, 0, b_in.x_v834);
                comm.recv(0, 0, b_in.x_p8, 8);
                comm.recv(0, 0, b_in.x_p8, 8, Datatype::from_type<scalar_t>());
                b.expect_eq(b_in);
        }
    }
    void blocking_ready(){
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                comm.recv(i, 0);
                comm.rsend(i, 0, b.x_s);
                comm.rsend(i, 0, b.x_rawarr3);
                comm.rsend(i, 0, b.x_rawarr34);
                comm.rsend(i, 0, b.x_arr3);
                comm.rsend(i, 0, b.x_arr34);
                comm.rsend(i, 0, b.x_v8);
                comm.rsend(i, 0, b.x_v83);
                comm.rsend(i, 0, b.x_v834);
                comm.rsend(i, 0, b.x_p8, 8);
                comm.rsend(i, 0, b.x_p8, 8, Datatype::from_type<scalar_t>());
            }
        } else {
            Requests req = 
                   comm.irecv(0, 0, b_in.x_s);
            req += comm.irecv(0, 0, b_in.x_rawarr3);
            req += comm.irecv(0, 0, b_in.x_rawarr34);
            req += comm.irecv(0, 0, b_in.x_arr3);
            req += comm.irecv(0, 0, b_in.x_arr34);
            req += comm.irecv(0, 0, b_in.x_v8);
            req += comm.irecv(0, 0, b_in.x_v83);
            req += comm.irecv(0, 0, b_in.x_v834);
            req += comm.irecv(0, 0, b_in.x_p8, 8);
            req += comm.irecv(0, 0, b_in.x_p8, 8, 
                Datatype::from_type<scalar_t>());
            
            comm.send(0, 0);
            req.waitall();
            b.expect_eq(b_in);
        }
    }
    
    void nonblocking_standard() {
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                Requests req = 
                       comm.isend(i, 0, b.x_s);
                req += comm.isend(i, 0, b.x_rawarr3);
                req += comm.isend(i, 0, b.x_rawarr34);
                req += comm.isend(i, 0, b.x_arr3);
                req += comm.isend(i, 0, b.x_arr34);
                req += comm.isend(i, 0, b.x_v8);
                req += comm.isend(i, 0, b.x_v83);
                req += comm.isend(i, 0, b.x_v834);
                req += comm.isend(i, 0, b.x_p8, 8);
                req += comm.isend(i, 0, b.x_p8, 8, 
                    Datatype::from_type<scalar_t>());
                req.waitall();
            }
        } else {
            Requests req = 
                   comm.irecv(0, 0, b_in.x_s);
            req += comm.irecv(0, 0, b_in.x_rawarr3);
            req += comm.irecv(0, 0, b_in.x_rawarr34);
            req += comm.irecv(0, 0, b_in.x_arr3);
            req += comm.irecv(0, 0, b_in.x_arr34);
            req += comm.irecv(0, 0, b_in.x_v8);
            req += comm.irecv(0, 0, b_in.x_v83);
            req += comm.irecv(0, 0, b_in.x_v834);
            req += comm.irecv(0, 0, b_in.x_p8, 8);
            req += comm.irecv(0, 0, b_in.x_p8, 8, 
                Datatype::from_type<scalar_t>());
            req.waitall();
            b.expect_eq(b_in);
        }
    }
    void nonblocking_synchronous() {
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                Requests req = 
                       comm.issend(i, 0, b.x_s);
                req += comm.issend(i, 0, b.x_rawarr3);
                req += comm.issend(i, 0, b.x_rawarr34);
                req += comm.issend(i, 0, b.x_arr3);
                req += comm.issend(i, 0, b.x_arr34);
                req += comm.issend(i, 0, b.x_v8);
                req += comm.issend(i, 0, b.x_v83);
                req += comm.issend(i, 0, b.x_v834);
                req += comm.issend(i, 0, b.x_p8, 8);
                req += comm.issend(i, 0, b.x_p8, 8, 
                    Datatype::from_type<scalar_t>());
                req.waitall();
            }
        } else {
            Requests req = 
                   comm.irecv(0, 0, b_in.x_s);
            req += comm.irecv(0, 0, b_in.x_rawarr3);
            req += comm.irecv(0, 0, b_in.x_rawarr34);
            req += comm.irecv(0, 0, b_in.x_arr3);
            req += comm.irecv(0, 0, b_in.x_arr34);
            req += comm.irecv(0, 0, b_in.x_v8);
            req += comm.irecv(0, 0, b_in.x_v83);
            req += comm.irecv(0, 0, b_in.x_v834);
            req += comm.irecv(0, 0, b_in.x_p8, 8);
            req += comm.irecv(0, 0, b_in.x_p8, 8, 
                Datatype::from_type<scalar_t>());
            req.waitall();
            b.expect_eq(b_in);
        }
    }
    void nonblocking_buffered() {
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                Requests req = 
                       comm.ibsend(i, 0, b.x_s);
                req += comm.ibsend(i, 0, b.x_rawarr3);
                req += comm.ibsend(i, 0, b.x_rawarr34);
                req += comm.ibsend(i, 0, b.x_arr3);
                req += comm.ibsend(i, 0, b.x_arr34);
                req += comm.ibsend(i, 0, b.x_v8);
                req += comm.ibsend(i, 0, b.x_v83);
                req += comm.ibsend(i, 0, b.x_v834);
                req += comm.ibsend(i, 0, b.x_p8, 8);
                req += comm.ibsend(i, 0, b.x_p8, 8, 
                    Datatype::from_type<scalar_t>());
                req.waitall();
            }
        } else {
            Requests req = 
                   comm.irecv(0, 0, b_in.x_s);
            req += comm.irecv(0, 0, b_in.x_rawarr3);
            req += comm.irecv(0, 0, b_in.x_rawarr34);
            req += comm.irecv(0, 0, b_in.x_arr3);
            req += comm.irecv(0, 0, b_in.x_arr34);
            req += comm.irecv(0, 0, b_in.x_v8);
            req += comm.irecv(0, 0, b_in.x_v83);
            req += comm.irecv(0, 0, b_in.x_v834);
            req += comm.irecv(0, 0, b_in.x_p8, 8);
            req += comm.irecv(0, 0, b_in.x_p8, 8, 
                Datatype::from_type<scalar_t>());
            req.waitall();
            b.expect_eq(b_in);
        }
    }
    void nonblocking_ready() {
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                comm.recv(i,0);
                Requests req = 
                       comm.irsend(i, 0, b.x_s);
                req += comm.irsend(i, 0, b.x_rawarr3);
                req += comm.irsend(i, 0, b.x_rawarr34);
                req += comm.irsend(i, 0, b.x_arr3);
                req += comm.irsend(i, 0, b.x_arr34);
                req += comm.irsend(i, 0, b.x_v8);
                req += comm.irsend(i, 0, b.x_v83);
                req += comm.irsend(i, 0, b.x_v834);
                req += comm.irsend(i, 0, b.x_p8, 8);
                req += comm.irsend(i, 0, b.x_p8, 8, 
                    Datatype::from_type<scalar_t>());
                req.waitall();
            }
        } else {
            Requests req = 
                   comm.irecv(0, 0, b_in.x_s);
            req += comm.irecv(0, 0, b_in.x_rawarr3);
            req += comm.irecv(0, 0, b_in.x_rawarr34);
            req += comm.irecv(0, 0, b_in.x_arr3);
            req += comm.irecv(0, 0, b_in.x_arr34);
            req += comm.irecv(0, 0, b_in.x_v8);
            req += comm.irecv(0, 0, b_in.x_v83);
            req += comm.irecv(0, 0, b_in.x_v834);
            req += comm.irecv(0, 0, b_in.x_p8, 8);
            req += comm.irecv(0, 0, b_in.x_p8, 8, 
                Datatype::from_type<scalar_t>());
            comm.send(0,0);
            req.waitall();
            b.expect_eq(b_in);
        }
    }

    void persistent_standard() {
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                Requests req = 
                       comm.send_init(i, 0, b.x_s);
                req += comm.send_init(i, 1, b.x_rawarr3);
                req += comm.send_init(i, 2, b.x_rawarr34);
                req += comm.send_init(i, 3, b.x_arr3);
                req += comm.send_init(i, 4, b.x_arr34);
                req += comm.send_init(i, 5, b.x_v8);
                req += comm.send_init(i, 6, b.x_v83);
                req += comm.send_init(i, 7, b.x_v834);
                req += comm.send_init(i, 8, b.x_p8, 8);
                req += comm.send_init(i, 9, b.x_p8, 8, 
                    Datatype::from_type<scalar_t>());
                for(int j=0; j<4; ++j){
                    req.startall();
                    req.waitall();
                }
            }
        } else {
            Requests req = 
                   comm.recv_init(0, 0, b_in.x_s);
            req += comm.recv_init(0, 1, b_in.x_rawarr3);
            req += comm.recv_init(0, 2, b_in.x_rawarr34);
            req += comm.recv_init(0, 3, b_in.x_arr3);
            req += comm.recv_init(0, 4, b_in.x_arr34);
            req += comm.recv_init(0, 5, b_in.x_v8);
            req += comm.recv_init(0, 6, b_in.x_v83);
            req += comm.recv_init(0, 7, b_in.x_v834);
            req += comm.recv_init(0, 8, b_in.x_p8, 8);
            req += comm.recv_init(0, 9, b_in.x_p8, 8, 
                Datatype::from_type<scalar_t>());
            for(int i=0; i<4; ++i){
                b_in.zero_values();
                req.startall();
                req.waitall();
                b.expect_eq(b_in);
            }
        }
    }
    void persistent_synchronous() {
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                Requests req = 
                       comm.ssend_init(i, 0, b.x_s);
                req += comm.ssend_init(i, 1, b.x_rawarr3);
                req += comm.ssend_init(i, 2, b.x_rawarr34);
                req += comm.ssend_init(i, 3, b.x_arr3);
                req += comm.ssend_init(i, 4, b.x_arr34);
                req += comm.ssend_init(i, 5, b.x_v8);
                req += comm.ssend_init(i, 6, b.x_v83);
                req += comm.ssend_init(i, 7, b.x_v834);
                req += comm.ssend_init(i, 8, b.x_p8, 8);
                req += comm.ssend_init(i, 9, b.x_p8, 8, 
                    Datatype::from_type<scalar_t>());
                for(int j=0; j<4; ++j){
                    req.startall();
                    req.waitall();
                }
            }
        } else {
            Requests req = 
                   comm.recv_init(0, 0, b_in.x_s);
            req += comm.recv_init(0, 1, b_in.x_rawarr3);
            req += comm.recv_init(0, 2, b_in.x_rawarr34);
            req += comm.recv_init(0, 3, b_in.x_arr3);
            req += comm.recv_init(0, 4, b_in.x_arr34);
            req += comm.recv_init(0, 5, b_in.x_v8);
            req += comm.recv_init(0, 6, b_in.x_v83);
            req += comm.recv_init(0, 7, b_in.x_v834);
            req += comm.recv_init(0, 8, b_in.x_p8, 8);
            req += comm.recv_init(0, 9, b_in.x_p8, 8, 
                Datatype::from_type<scalar_t>());
            for(int i=0; i<4; ++i){
                b_in.zero_values();
                req.startall();
                req.waitall();
                b.expect_eq(b_in);
            }
        }
    }
    void persistent_buffered() {
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                Requests req = 
                       comm.bsend_init(i, 0, b.x_s);
                req += comm.bsend_init(i, 1, b.x_rawarr3);
                req += comm.bsend_init(i, 2, b.x_rawarr34);
                req += comm.bsend_init(i, 3, b.x_arr3);
                req += comm.bsend_init(i, 4, b.x_arr34);
                req += comm.bsend_init(i, 5, b.x_v8);
                req += comm.bsend_init(i, 6, b.x_v83);
                req += comm.bsend_init(i, 7, b.x_v834);
                req += comm.bsend_init(i, 8, b.x_p8, 8);
                req += comm.bsend_init(i, 9, b.x_p8, 8, 
                    Datatype::from_type<scalar_t>());
                for(int j=0; j<4; ++j){
                    req.startall();
                    req.waitall();
                }
            }
        } else {
            Requests req = 
                   comm.recv_init(0, 0, b_in.x_s);
            req += comm.recv_init(0, 1, b_in.x_rawarr3);
            req += comm.recv_init(0, 2, b_in.x_rawarr34);
            req += comm.recv_init(0, 3, b_in.x_arr3);
            req += comm.recv_init(0, 4, b_in.x_arr34);
            req += comm.recv_init(0, 5, b_in.x_v8);
            req += comm.recv_init(0, 6, b_in.x_v83);
            req += comm.recv_init(0, 7, b_in.x_v834);
            req += comm.recv_init(0, 8, b_in.x_p8, 8);
            req += comm.recv_init(0, 9, b_in.x_p8, 8, 
                Datatype::from_type<scalar_t>());
            for(int i=0; i<4; ++i){
                b_in.zero_values();
                req.startall();
                req.waitall();
                b.expect_eq(b_in);
            }
        }
    }
    void persistent_ready() {
        data_buffer_t b(*this), b_in(*this, false);
        if( rank == 0 ) {
            for(int i=1; i<n_procs; ++i){
                Requests req = 
                       comm.ssend_init(i, 0, b.x_s);
                req += comm.ssend_init(i, 1, b.x_rawarr3);
                req += comm.ssend_init(i, 2, b.x_rawarr34);
                req += comm.ssend_init(i, 3, b.x_arr3);
                req += comm.ssend_init(i, 4, b.x_arr34);
                req += comm.ssend_init(i, 5, b.x_v8);
                req += comm.ssend_init(i, 6, b.x_v83);
                req += comm.ssend_init(i, 7, b.x_v834);
                req += comm.ssend_init(i, 8, b.x_p8, 8);
                req += comm.ssend_init(i, 9, b.x_p8, 8, 
                    Datatype::from_type<scalar_t>());
                for(int j=0; j<4; ++j){
                    comm.recv(i, 0);
                    req.startall();
                    req.waitall();
                }
            }
        } else {
            Requests req = 
                   comm.recv_init(0, 0, b_in.x_s);
            req += comm.recv_init(0, 1, b_in.x_rawarr3);
            req += comm.recv_init(0, 2, b_in.x_rawarr34);
            req += comm.recv_init(0, 3, b_in.x_arr3);
            req += comm.recv_init(0, 4, b_in.x_arr34);
            req += comm.recv_init(0, 5, b_in.x_v8);
            req += comm.recv_init(0, 6, b_in.x_v83);
            req += comm.recv_init(0, 7, b_in.x_v834);
            req += comm.recv_init(0, 8, b_in.x_p8, 8);
            req += comm.recv_init(0, 9, b_in.x_p8, 8, 
                Datatype::from_type<scalar_t>());
            for(int i=0; i<4; ++i){
                b_in.zero_values();
                req.startall();
                comm.send(0, 0);
                req.waitall();
                b.expect_eq(b_in);
            }
        }
    }
};

} // namespace HIPP::MPI

int main(int argc, char const *argv[]){
    using namespace HIPP::MPI;
    
    Env env;
    auto comm = env.world();

    const size_t buf_size = 20*1024*1024;
    std::vector<char> buf(buf_size);
    buffer_attach(buf.data(), buf_size);

    for(int i=0; i<100; ++i){
        PointToPointTest<float>(comm, "float")();
        PointToPointTest<double>(comm, "double")();
        PointToPointTest<short>(comm, "short")();
        PointToPointTest<int>(comm, "int")();
        PointToPointTest<long>(comm, "long")();
        PointToPointTest<long long>(comm, "long long")();
    }

    comm.barrier();
    buffer_detach();
    
    return 0;
}
