/* basic-collective.cpp */
#include <hippmpi.h>
using namespace std;

int main(int argc, char const *argv[]){
    HIPP::MPI::Env env;                                 
    auto comm = env.world();                            
    int rank = comm.rank(), n_procs = comm.size();

    /* Process 0 prepares task edges and scatters them to local edges of each 
       process. */
    int local_edges[2];
    if( rank == 0 ){
        int total_num = 10000, num_per_proc = total_num/n_procs;
        vector<int> edges;
        for (int i = 0; i < n_procs; i++){
            edges.push_back(i*num_per_proc);
            edges.push_back((i+1)*num_per_proc);
        } 
        edges.back() = total_num;
        comm.scatter(edges.data(), local_edges, 0);
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
        comm.reduce(local_sum, &sum, "+", 0);
        HIPP::pout << "Result of sum is ", sum, 
            " (found by ", n_procs, " processes)", endl; 
    }else{
        comm.reduce(local_sum, NULL, "+", 0);
    }

    return 0;
}
