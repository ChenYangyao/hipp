/* basic-collective.cpp */
#include <hippmpi.h>
using namespace std;

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;                                 
    auto comm = env.world();                            
    int rank = comm.rank(), n_procs = comm.size();

    /* Process 0 prepares task edges for each process and scatters it to them. */
    vector<int> local_edges(2);                 // Each process will get its task edges.
    if( rank == 0 ){
        int total_num = 10000, num_per_proc = total_num/n_procs;
        vector<pair<int, int>> edges; 
        for (int i = 0; i < n_procs; i++) 
            edges.emplace_back(i*num_per_proc, (i+1)*num_per_proc);
        edges.back().second = total_num;
        comm.scatter(&edges[0], local_edges, 0); // [1] Scatter "edges" into "local_edges"
    }else{
        comm.scatter(NULL, local_edges, 0);                   
    }

    /* Each process finishes its computation. */
    int local_sum = 0;
    for(int i=local_edges[0]; i<local_edges[1]; ++i)
        local_sum += i;

    /* Make reduction of local summations to a total value. */
    if( rank == 0 ){
        int sum = 0;
        comm.reduce({&local_sum, 1, "int"}, 
            &sum, "+", 0);                      // [2] Reduce from "local_sum" into "sum" 

        HIPP::pout << "Result of sum is ", sum, 
            " (found by ", n_procs, " processes)", endl; 
    }else{
        comm.reduce({&local_sum, 1, "int"}, NULL, "+", 0);
    }

    return 0;
}
