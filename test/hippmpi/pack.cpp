#include <testdata.h>
#include <testmacro.h>

namespace HIPP::MPI {

void test_pack(Comm &comm) {
    int rank = comm.rank(), n_procs = comm.size();
    if( rank == 1 )
        HIPPMPI_TEST_ENTERF

    assert(n_procs >= 2);
    if( rank == 0 ){
        TestData td; td.init();

        Pack pk(comm);
        pk.push(td.x).push(td.y).push(td.nz).push(td.z)
            .push(td.sz_buff).push({td.ptr, td.sz_buff}).push(td.arr);
        comm.send(1, 0, pk.as_sendbuf());

        pk.set_size(10).set_position(0);
        pk.push(td.arr).push(td.sz_buff).push({td.ptr, td.sz_buff})
            .push(td.nz).push(td.z).push(td.y).push(td.x);
        comm.send(1, 1, pk.as_sendbuf());
    }else if (rank == 1){
        Pack pk(comm.probe(0, 0).count("packed"), comm);
        comm.recv(0, 0, pk.as_recvbuf());

        TestData td_targ, td; td_targ.init();
        pk.pop(td.x).pop(td.y).pop(td.nz);
        td.z.resize(td.nz);
        pk.pop(td.z);
        pk.pop(td.sz_buff);
        td._buff.resize(td.sz_buff);  td.ptr = td._buff.data();
        pk.pop({td.ptr, td.sz_buff}).pop(td.arr);

        td_targ.compare(td);
        HIPPMPI_TEST_PROGRESS, "receive 1 OK", endl;

        pk.set_position(0).set_size(comm.probe(0, 1).count("packed"));
        comm.recv(0, 1, pk.as_recvbuf());

        pk.pop(td.arr).pop(td.sz_buff);
        td._buff.resize(td.sz_buff); td.ptr = td._buff.data();
        pk.pop({td.ptr, td.sz_buff});
        pk.pop(td.nz); 
        td.z.resize(td.nz);
        pk.pop(td.z).pop(td.y).pop(td.x);

        td_targ.compare(td);
        HIPPMPI_TEST_PROGRESS, "receive 2 OK", endl;
    }
}

void test_external_pack(Comm &comm) {
    int rank = comm.rank(), n_procs = comm.size();
    if( rank == 1 )
        HIPPMPI_TEST_ENTERF

    assert(n_procs >= 2);
    if( rank == 0 ){
        TestData td; td.init();

        ExternalPack pk(1024);
        pk.push(td.x).push(td.y).push(td.nz).push(td.z)
            .push(td.sz_buff).push({td.ptr, td.sz_buff}).push(td.arr);
        comm.send(1, 0, pk.as_sendbuf());

        pk.set_size(10).set_position(0);
        pk.push(td.arr).push(td.sz_buff).push({td.ptr, td.sz_buff})
            .push(td.nz).push(td.z).push(td.y).push(td.x);
        comm.send(1, 1, pk.as_sendbuf());
    }else if (rank == 1){
        ExternalPack pk(comm.probe(0, 0).count("byte"));
        comm.recv(0, 0, pk.as_recvbuf());

        TestData td_targ, td; td_targ.init();
        pk.pop(td.x).pop(td.y).pop(td.nz);
        td.z.resize(td.nz);
        pk.pop(td.z);
        pk.pop(td.sz_buff);
        td._buff.resize(td.sz_buff);  td.ptr = td._buff.data();
        pk.pop({td.ptr, td.sz_buff}).pop(td.arr);
        td_targ.compare(td);
        HIPPMPI_TEST_PROGRESS,  "receive 1 OK", endl;

        pk.set_position(0).set_size(comm.probe(0, 1).count("byte"));
        comm.recv(0, 1, pk.as_recvbuf());

        pk.pop(td.arr).pop(td.sz_buff);
        td._buff.resize(td.sz_buff); td.ptr = td._buff.data();
        pk.pop({td.ptr, td.sz_buff});
        pk.pop(td.nz); 
        td.z.resize(td.nz);
        pk.pop(td.z).pop(td.y).pop(td.x);

        td_targ.compare(td);
        HIPPMPI_TEST_PROGRESS,  "receive 2 OK", endl;
    }
}

} // namespace mpi



int main(int argc, char const *argv[])
{
    HIPP::MPI::Env env;
    auto comm = env.world();

    HIPP::MPI::test_pack(comm);
    comm.barrier();

    HIPP::MPI::test_external_pack(comm);
    comm.barrier();

    return 0;
}
