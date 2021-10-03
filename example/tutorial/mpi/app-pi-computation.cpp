#include <hippmpi.h>
using namespace HIPP;
using namespace HIPP::MPI;
using namespace std;

int main(int argc, char const *argv[]) {
    Env env;
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();

    /* Find summation at each process. */
    int N = 10000;
    double h = 1.0 / N, sum = 0.;
    for(int i=rank; i<N; i+=n_procs){
        double x = h * (i+0.5);
        sum += 1. / ( 1. + x*x );
    }
    double my_pi = 4.0 * h * sum;
    
    double pi;
    comm.reduce(my_pi, pi, SUM, 0);
    if( rank == 0 )
        pout << "Find PI = ", pi, endl;
    
    return 0;
}
