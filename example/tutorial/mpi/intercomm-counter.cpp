#include <hippmpi.h>

using namespace std;
using HIPP::MPI::Comm;

class Counter {
public:
    Counter(Comm &comm, int max_count, int comm_tag);
    bool get_next(int &count);
private:
    Comm _inter_comm;
    int _count, _max_count;
};

Counter::Counter(Comm &comm, int max_count, int comm_tag) 
: _inter_comm(nullptr), _count(0), _max_count(max_count)
{
    int rank = comm.rank(), n_procs = comm.size(),
        is_master = (rank == 0),
        remote_leader = is_master, local_leader = 0;

    /* Split the global comm into two, and create an inter-comm of them. */
    auto local_comm = comm.split(is_master);
    _inter_comm = local_comm.create_inter(
        local_leader, comm, remote_leader, comm_tag);
    
    /* The master waits for request and replies. */
    if( !is_master ) return;
    int n_stop = 0;
    while( n_stop < n_procs-1 ){
        auto status = 
            _inter_comm.recv(HIPP::MPI::ANY_SOURCE, 0, NULL, 0, "int");
        _inter_comm.send(status.source(), 0, &_count, 1, "int");
        if( _count < _max_count ) 
            ++ _count;
        else 
            ++ n_stop;
    }
}

bool Counter::get_next(int &count) {
    _inter_comm.send(0, 0, NULL, 0, "int");
    _inter_comm.recv(0, 0, &_count, 1, "int");
    if( _count < _max_count ){
        count = _count;
        return true;
    }else{
        return false;
    }
}

int main(int argc, char const *argv[])
{
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank();
    
    int max_counts = 16, comm_tag = 0;
    Counter counter(comm, max_counts, comm_tag);
    if( rank != 0 ){
        int count;
        while(counter.get_next(count))
            printf("Process %d gets %d.\n", rank, count);
    }
    
    return 0;
}
