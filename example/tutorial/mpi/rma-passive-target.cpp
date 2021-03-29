#include <hippmpi.h>
using namespace std;

int main(int argc, char const *argv[]) {
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank(), n_RMAs = 5;

    if( rank == 0 ){
        /* Process 0 creates window and puts new values into it. */
        int val, buff_size = sizeof(int), disp_unit = 1;
        auto win = comm.win_create(&val, buff_size, disp_unit);

        for(int i=0; i<n_RMAs; ++i){
            win.lock(win.LOCK_EXCLUSIVE, 0);
            val = i;
            win.unlock(0);
            comm.barrier();
            comm.barrier();
        }
    }else if(rank == 1){
        /* Process 1 creates window and gets the values from it. 
         * No memory is attached to this local window.
         */
        auto win = comm.win_create(NULL, 0, 1);
        int val, src_rank = 0, offset = 0;
        for(int i=0; i<n_RMAs; ++i){
            comm.barrier();
            win.lock(win.LOCK_EXCLUSIVE, src_rank);
            win.get(src_rank, val, offset);
            win.unlock(src_rank);
            comm.barrier();
            HIPP::pout << "Get ", val, endl;
        }
    }

    return 0;
}

