/* more-collective-data-moving.cpp */
#include <hippmpi.h>
using namespace std;

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;                                 
    auto comm = env.world();                            
    int rank = comm.rank(), n_procs = comm.size();

    double x = rank;
    if( rank == 0 ){
        vector<double> all_x(n_procs);
        comm.gather(x, all_x.data(), 0);
        HIPP::pout << "Process 0 gathered (", all_x, ")", endl;
    }else{
        comm.gather(x, NULL, 0);
    }

}