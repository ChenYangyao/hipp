#include <hippmpi.h>
using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;

void create_new_comm(){
    {
        auto 
            self_comm = mpi::Comm::selfval(),
            null_comm = mpi::Comm::nullval(),
            world_comm = mpi::Comm::world();
    }

    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();
    
    int color = rank % 2;
    auto mod2_comm = comm.split(color);
    pout << "Process ", rank, '/', n_procs, " in the new comm is ", 
        mod2_comm.rank(), '/', mod2_comm.size(), endl;

    {
        auto copied_comm = comm.dup();
    }
    {
        auto grp = comm.group(),
            sub_grp = (rank < 2) ? grp.incl({0,1}) : grp.emptyval();
        auto sub_comm = comm.create(sub_grp);
    }

    int remote_leader = (rank % 2)? 0 : 1;
    auto inter_comm = mod2_comm.create_inter(0, comm, remote_leader, 0);
    
    bool is_inter = inter_comm.is_inter();          // => true
    assert(is_inter);
    
    auto local_grp = inter_comm.group();
    int local_rank = inter_comm.rank(),             // => mod2comm.rank()
        local_n_procs = inter_comm.size();          // => mod2comm.size()
    assert( local_grp.size() == mod2_comm.size() );
    assert( local_rank == mod2_comm.rank() );
    assert( local_n_procs == mod2_comm.size() );

    auto remote_grp = inter_comm.remote_group();
    int remote_size = inter_comm.remote_size();     // comm.size() - mod2comm.size()
    assert( remote_size == comm.size() - mod2_comm.size() );
    
    auto merged_comm = inter_comm.merge_inter(rank % 2);
}

void meta_info_inquiry() {
    auto comm = mpi::Env::world();
    
    int rank = comm.rank(), n_procs = comm.size();
    bool is_null = comm.is_null();
    auto grp = comm.group();

    if( rank == 0 ) {
        pout << "rank ", rank, ", n_procs ", n_procs, 
            ", is_null ", is_null, endl;
        pout << grp.rank(), "/", grp.size(), endl;
    }    
}

struct Attr {
    int x;
    double y;

    Attr(int _x, int _y) : x(_x), y(_y) {}
    
    Attr(const Attr &that) : x(that.x), y(that.y) {
        pout << "Attr is copied", endl;
    }
    
    ~Attr() {
        pout << "Attr is destroyed", endl;
    }
};

void attr_cache() {
    static int keyval = mpi::Comm::create_keyval<Attr>();
    
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();


    comm.set_attr(keyval, new Attr(1, 2.0));
        
    Attr *attr;
    comm.get_attr(keyval, attr);
    pout << "x=", attr->x, ", y=", attr->y, endl;

    auto copied_comm = comm.dup();
    if( copied_comm.get_attr(keyval, attr) ) {
        pout << "x=", attr->x, ", y=", attr->y, endl;
    } else {
        pout << "attr is not set", endl;
    }

    comm.del_attr(keyval);
}

int main(int argc, char const *argv[]) {
    mpi::Env env;

    //create_new_comm();
    //meta_info_inquiry();
    attr_cache();

    return 0;
}
