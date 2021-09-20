#include <hippmpi.h>
#include <testmacro.h>

namespace HIPP::MPI {
    
void test_gatherv(Comm &comm) {
    int rank = comm.rank(), n_procs = comm.size();

    assert(n_procs >= 4);
    if( rank >= 4 ) return;

    vector<int> sendbuf(rank+1, rank);
    if( rank == 0 ){
        vector<int> cnts = {1,2,3,4}, offs = {0,1,3,6};
        vector<int> recvbuf(cnts.back()+offs.back());
        comm.gatherv(sendbuf.data(), sendbuf.size(), INT,
            recvbuf.data(), cnts.data(), offs.data(), INT, rank);
        
        int b = 0;
        for(int i=0, n=cnts.size(); i<n; ++i){
            assert(b == offs[i]);
            for(int j=0; j<cnts[i]; ++j){
                assert(recvbuf[b++] == i);
            }
        }
    } else {
        comm.gatherv(sendbuf.data(), sendbuf.size(), INT, BOTTOM, 
            NULL, NULL, INT, 0);
    }
}
} // namespace HIPP::MPI

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;
    auto comm = env.world();
    
    HIPP::MPI::test_gatherv(comm);
    
    return 0;
}