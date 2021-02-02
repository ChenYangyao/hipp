#include <hippmpi.h>
#include <hippnumerical.h>
using namespace std;

const double REAL_PI = 3.141592653589793238462643;

void master_do(long long chunk_size, HIPP::MPI::Comm &world_comm){

    HIPP::NUMERICAL::UniformRealRandomNumber rng(-1., 1.);
    vector<double> rands(chunk_size);
    /* Wait for request. If request != 0, send back random numbers. */
    while(true){
        int request;
        auto status = world_comm.recv(HIPP::MPI::ANY_SOURCE, 0, 
            &request, 1, "int");
        if( !request ) break;
        rng(rands.begin(), rands.end());
        world_comm.send(status.source(), 0, rands);
    }
}

void slave_do(long long chunk_size, long long max_n_points, double eps, 
    HIPP::MPI::Comm &world_comm, HIPP::MPI::Comm &slaves_comm)
{
    long long n_in = 0, n_out = 0;
    vector<double> rands(chunk_size);
    int request = 1;
    while(request){
        /* Request random numbers. */
        world_comm.send(0, 0, &request, 1, "int");
        world_comm.recv(0, 0, rands);
        
        /* Computing PI using Monte-Carlo method. Reduce the result into one 
           process. */
        for(long long i=0; i<chunk_size; i+=2){
            double x = rands[i], y = rands[i+1];
            if( x*x+y*y < 1 ) ++ n_in;
            else ++ n_out;
        }
        long long n_inout[2] = {n_in, n_out}, total_inout[2];
        slaves_comm.allreduce({n_inout, 2, "long long"}, total_inout, "+");
        double pi = (4.0*total_inout[0]) / (total_inout[0]+total_inout[1]);

        /* See if convergent. If it is, send a stop signal. */
        bool done = ( std::fabs(pi-REAL_PI) < eps ) 
            || (total_inout[0]+total_inout[1] > max_n_points);
        request = done ? 0 : 1;
        if( slaves_comm.rank() == 0 ){
            HIPP::pout << "pi=", pi, endl;
            if( done )
                world_comm.send(0, 0, &request, 1, "int");
        }
    }
}

int main(int argc, char const *argv[]) {
    HIPP::MPI::Env env;
    auto world_comm = env.world();
    int rank = world_comm.rank(), master_rank = 0;

    auto slaves_group = world_comm.group().excl({master_rank});
    auto slaves_comm = world_comm.create(slaves_group);

    long long chunk_size = 50000, max_n_points = 100000000; 
    double eps = 1.0e-3;
    if( rank == master_rank ){
        master_do(chunk_size, world_comm);
    }else {
        slave_do(chunk_size, max_n_points, eps, world_comm, slaves_comm);
    }
    
    return 0;
}
