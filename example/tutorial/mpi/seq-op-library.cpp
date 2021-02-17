#include <hippmpi.h>
#include <optional>

using namespace std;
using HIPP::MPI::Comm;

struct SeqAttr {
    int _prev, _next;
    std::optional<Comm> _comm;
    
    SeqAttr(const Comm &comm) {
        int rank = comm.rank(), n_procs = comm.size();
        _prev = (rank==0) ? HIPP::MPI::PROC_NULL : (rank-1);
        _next = (rank==n_procs-1) ? HIPP::MPI::PROC_NULL : (rank+1);
        _comm.emplace( comm.dup() );
    }
    SeqAttr(const SeqAttr &o) : 
        _prev(o._prev), _next(o._next), _comm(std::nullopt) { }
};

class Seq {
public:
    Seq(Comm &comm); 
    ~Seq();    
private:   
    inline static int _keyval = HIPP::MPI::KEYVAL_INVALID;
    SeqAttr *_attr;
};

Seq::Seq(Comm &comm) {
    if( _keyval == HIPP::MPI::KEYVAL_INVALID )
        _keyval = Comm::create_keyval<SeqAttr>();
    if( !comm.get_attr(_keyval, _attr) ){
        _attr = new SeqAttr(comm);
        comm.set_attr(_keyval, _attr);
    }
    auto &seq_comm = _attr->_comm;
    if( !seq_comm)
        seq_comm.emplace(comm.dup());
    seq_comm->recv(_attr->_prev, 0, NULL, 0, "int");
}
Seq::~Seq() {
    auto &seq_comm = _attr->_comm;
    seq_comm->send(_attr->_next, 0, NULL, 0, "int");
    seq_comm->barrier();
};

int main(int argc, char const *argv[]) 
{
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank();

    {
        Seq seq(comm);
        // ... code to execute on one process at a time, 
        // e.g.,
        HIPP::pout << "This is process ", rank, endl;
    }

    return 0;
}

