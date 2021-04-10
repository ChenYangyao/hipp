#include "testdata.h"
namespace HIPP::MPI {

void test_pack(Comm &comm) {
    int rank = comm.rank(), n_procs = comm.size();
    assert(n_procs >= 2);
    if( rank == 0 ){
        TestData td; td.init();

        Pack pk(comm);
        pk.push(td.x).push(td.y).push(td.nz).push(td.z);
        comm.send(1, 0, pk.as_sendbuf());

        pk.set_size(10).set_position(0);
        pk.push(td.nz).push(td.z).push(td.y).push(td.x);
        comm.send(1, 1, pk.as_sendbuf());
    }else if (rank == 1){
        Pack pk(comm.probe(0, 0).count("packed"), comm);
        comm.recv(0, 0, pk.as_recvbuf());

        TestData td_targ, td; td_targ.init();
        pk.pop(td.x).pop(td.y).pop(td.nz);
        td.z.resize(td.nz);
        pk.pop(td.z);

        td_targ.compare(td);
        pout << "t_pack ... test_pack\n  -- Recv 1 has been finished.", endl;

        pk.set_position(0).set_size(comm.probe(0, 1).count("packed"));
        comm.recv(0, 1, pk.as_recvbuf());

        pk.pop(td.nz); 
        td.z.resize(td.nz);
        pk.pop(td.z).pop(td.y).pop(td.x);

        td_targ.compare(td);
        pout << "  -- Recv 2 has been finished.", endl;
        pout << "  -- Successfully exit", endl;
    }
}

void test_external_pack(Comm &comm) {
    int rank = comm.rank(), n_procs = comm.size();
    assert(n_procs >= 2);
    if( rank == 0 ){
        TestData td; td.init();

        ExternalPack pk;
        pk.push(td.x).push(td.y).push(td.nz).push(td.z);
        comm.send(1, 0, pk.as_sendbuf());

        pk.set_size(10).set_position(0);
        pk.push(td.nz).push(td.z).push(td.y).push(td.x);
        comm.send(1, 1, pk.as_sendbuf());
    }else if (rank == 1){
        ExternalPack pk(comm.probe(0, 0).count("packed"));
        comm.recv(0, 0, pk.as_recvbuf());

        TestData td_targ, td; td_targ.init();
        pk.pop(td.x).pop(td.y).pop(td.nz);
        td.z.resize(td.nz);
        pk.pop(td.z);

        td_targ.compare(td);
        pout << "t_pack ... test_external_pack\n", 
            "  -- Recv 1 has been finished.", endl;

        pk.set_position(0).set_size(comm.probe(0, 1).count("packed"));
        comm.recv(0, 1, pk.as_recvbuf());

        pk.pop(td.nz); 
        td.z.resize(td.nz);
        pk.pop(td.z).pop(td.y).pop(td.x);

        td_targ.compare(td);
        pout << "  -- Recv 2 has been finished.", endl;
        pout << "  -- Successfully exit", endl;
    }
}

} // namespace mpi



int main(int argc, char const *argv[])
{
    HIPP::MPI::Env env;
    auto comm = env.world();
    HIPP::MPI::test_pack(comm);
    HIPP::MPI::test_external_pack(comm);

    return 0;
}
