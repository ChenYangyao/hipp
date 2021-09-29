#include <hippmpi.h>
#include <testmacro.h>
#include <testdata.h>

namespace HIPP::MPI {

struct TestGather : TestGatherData {
    using TestGatherData::TestGatherData;
    void operator()() {
        test_standard_call();
        test_datapacket();
    }
    HIPPMPI_TEST_F_BEGIN(test_standard_call)
        float *p_s = sendbuf_eqsz.data(), *p_r = recvbuf_eqsz.data();
        int n_s = sendbuf_eqsz.size();
        for(int i=0; i<5; ++i){
            comm.gather( p_s, n_s, FLOAT, p_r, n_s, FLOAT, 0 );
            if( is_root ) chk_res_eqsz();
            
        }
    HIPPMPI_TEST_F_END
    HIPPMPI_TEST_F_BEGIN(test_datapacket)
        auto &v_s = sendbuf_eqsz, &v_r = recvbuf_eqsz;
        float *p_s = v_s.data(), *p_r = v_r.data();
        int n_s = v_s.size();
        for(int i=0; i<3; ++i){
            comm.gather( p_s, p_r, n_s, FLOAT, 0 );
            if( is_root ) chk_res_eqsz();
            
            comm.gather( v_s, p_r, 0 );
            if( is_root ) chk_res_eqsz();
        }
    HIPPMPI_TEST_F_END
};

struct TestGatherv : TestGatherData {
    using TestGatherData::TestGatherData;

    void operator()() {
        test_standard_call();
        test_contiguous_buffer();
    }
    HIPPMPI_TEST_F_BEGIN(test_standard_call)
        auto *p_s = sendbuf.data(), *p_r = recvbuf.data();
        int n_s = sendbuf.size();
        for(int i=0; i<3; ++i){
            comm.gatherv(p_s, n_s, INT, p_r, cnts.data(), offs.data(), INT, 0);
            if( is_root ) chk_res();
        }
    HIPPMPI_TEST_F_END
    HIPPMPI_TEST_F_BEGIN(test_contiguous_buffer)
        for(int i=0; i<3; ++i){
            comm.gatherv(sendbuf, recvbuf.data(), cnts, offs, INT, 0);
            if( is_root ) chk_res();
        }
    HIPPMPI_TEST_F_END
};

} // namespace HIPP::MPI

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;
    auto comm = env.world();

    HIPP::MPI::TestGather t1(comm); t1();
    HIPP::MPI::TestGatherv t2(comm); t2();
    
    return 0;
}