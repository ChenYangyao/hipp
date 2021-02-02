#include <hippmpi.h>
using namespace std;

const double REAL_PI = 3.141592653589793238462643;

int main(int argc, char const *argv[]) {
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();

    /* Get user-input no. of intervals. */
    int n_intervals;
    if( rank == 0 ){
        cout << "Enter the no. of intervals: ";
        cin >> n_intervals;
    }
    comm.bcast({&n_intervals, 1, "int"}, 0);
    if( n_intervals <= 0 ) return 0;

    /* Find integration in each process and reduce it. */
    double h = 1. / n_intervals, sum = 0.;
    for(int i=rank; i<n_intervals; i+=n_procs){
        double x = h * (i+0.5);
        sum += 4. / ( 1. + x*x );
    }
    double my_pi = h * sum, pi;
    comm.reduce({&my_pi, 1, "double"}, &pi, "+", 0);

    if( rank == 0 ){
        cout << "Find pi=" << std::setprecision(16) << pi << 
            ", error=" << std::fabs(pi-REAL_PI) << endl;
    }
    
    return 0;
}
