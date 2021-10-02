/* app-distributed-sum.cpp */
#include <hippmpi.h>
using namespace HIPP;
using namespace HIPP::MPI;
using namespace std;


int main(int argc, char const *argv[])
{
    Env env;                                 
    auto comm = env.world();                            
    int rank = comm.rank(), n_procs = comm.size();

    /* Root computes index ranges and scatter them. */
    typedef std::array<int, 2> range_t;
    range_t range;
    vector<range_t> ranges;
    if( rank == 0 ) {
        int N = 10000, N_each = N / n_procs;
        for(int i=0; i<n_procs; ++i) 
            ranges.push_back({i*N_each, (i+1)*N_each});
    }
    comm.scatter(ranges.data(), range, 0);

    /* Processes compute local sums and reduce into the root. */
    auto [b,e] = range;
    int local_sum = 0, sum;
    for(int i=b; i<e; ++i) local_sum += i;
    comm.reduce(local_sum, sum, SUM, 0);

    if( rank == 0 )
        pout << "Sum = ", sum, endl;

    return 0;
}
