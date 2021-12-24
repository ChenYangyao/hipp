#include <hippmpi.h>
#include <hippnumerical.h>

using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;
namespace nu = HIPP::NUMERICAL;
using nu::SArray;
using nu::SVec;
using nu::DArray;


void basic_p2p() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();
    
    int tag = 0, buf[5];
    if( rank == 0 ) {
        comm.send(1, tag, buf);
    }else if ( rank == 1 ) {
        comm.recv(0, tag, buf);
    }
}

void comm_buffer() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();
    int tag = 0;

    double scalar;                                  // numerical scalar
    
    int raw_arr[3][4];                              // 3x4 array
    array<float, 3> arr;                            // 3-element array
    array<array<int, 4>, 3> arr_of_arr;             // 3x4 array
    SArray<double, 3, 4> s_arr;                     // 3x4 array
    SVec<int, 4> s_vec;                             // 4-element array

    vector<double> vec(10);                         // 10 scalar values
    vector<array<int, 3> > vec_of_arr(10);          // 10 arrays, each has length 3
    DArray<int, 3>  matrix({2,3,4});                // 3x3x3 matrix
    
    string s(5, 'c');

    float *buf1 = new float [12],
          *buf2 = (float *)malloc(sizeof(float)*12);

    if( rank == 0 ) {
        comm.send(1, tag, scalar);

        comm.send(1, tag, raw_arr);
        comm.send(1, tag, arr);
        comm.send(1, tag, arr_of_arr);
        comm.send(1, tag, s_arr);
        comm.send(1, tag, s_vec);
        
        comm.send(1, tag, vec);
        comm.send(1, tag, vec_of_arr);
        comm.send(1, tag, matrix);
        
        comm.send(1, tag, s);

        comm.send(1, tag, buf1, 12, mpi::FLOAT);
        comm.send(1, tag, buf2, 12, mpi::FLOAT);
    } else if ( rank == 1 ) {
        comm.recv(0, tag, scalar);

        comm.recv(0, tag, raw_arr);
        comm.recv(0, tag, arr);
        comm.recv(0, tag, arr_of_arr);
        comm.recv(0, tag, s_arr);
        comm.recv(0, tag, s_vec);

        comm.recv(0, tag, vec);
        comm.recv(0, tag, vec_of_arr);
        comm.recv(0, tag, matrix);
        
        comm.recv(0, tag, s);

        comm.recv(0, tag, buf1, 12, mpi::FLOAT);
        comm.recv(0, tag, buf2, 12, mpi::FLOAT);
    }

    delete [] buf1;
    free(buf2);

}

void comm_mode() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();
    int tag = 0;

    int buf1[4];
    vector<double> buf2(8);

    if( rank == 0 ) {
        /* Standard mode. */
        std::fill_n(buf1, 4, 1);
        std::fill_n(buf2.data(), 8, 1);
        comm.send(1, tag, buf1);
        comm.send(1, tag, buf2);

        /* Buffered mode. */
        size_t buf_size = sizeof(buf1) + sizeof(double)*buf2.size() 
            + 2 * mpi::BSEND_OVERHEAD;
        char *buff = new char[buf_size];
        mpi::buffer_attach(buff, buf_size);
        
        std::fill_n(buf1, 4, 2);
        std::fill_n(buf2.data(), 8, 2);
        comm.bsend(1, tag, buf1);
        comm.bsend(1, tag, buf2);

        /* Synchronous mode. */
        std::fill_n(buf1, 4, 3);
        std::fill_n(buf2.data(), 8, 3);
        comm.ssend(1, tag, buf1);
        comm.ssend(1, tag, buf2);
        
        /* Ready mode. */
        std::fill_n(buf1, 4, 4);
        std::fill_n(buf2.data(), 8, 4);
        comm.recv(1, tag);                          // wait for notification
        comm.rsend(1, tag, buf1);
        comm.rsend(1, tag, buf2);
    } else if( rank == 1 ) {
        comm.recv(0, tag, buf1);
        comm.recv(0, tag, buf2);
        pout << "buf1 ={", pout(buf1, buf1+4), "}, buf2={", buf2, "}", endl;

        comm.recv(0, tag, buf1);
        comm.recv(0, tag, buf2);
        pout << "buf1 ={", pout(buf1, buf1+4), "}, buf2={", buf2, "}", endl;

        comm.recv(0, tag, buf1);
        comm.recv(0, tag, buf2);
        pout << "buf1 ={", pout(buf1, buf1+4), "}, buf2={", buf2, "}", endl;

        auto reqs = comm.irecv(0, tag, buf1);
        reqs += comm.irecv(0, tag, buf2);           // issue the receive calls
        comm.send(0, tag);                          // send an empty message for notification
        reqs.waitall();
        pout << "buf1 ={", pout(buf1, buf1+4), "}, buf2={", buf2, "}", endl;
    }

}

void nonblocking_p2p() {
    auto comm = mpi::Env::world();
    int rank = comm.rank(), n_procs = comm.size();
    int tag = 0;

    int buf1[4];
    vector<double> buf2(8);
    float buf3;
    
    if( rank == 0 ) {
        mpi::Requests req = comm.isend(1, tag, buf1);
        req.wait();
    }else if( rank == 1 ){
        mpi::Requests req = comm.irecv(0, tag, buf1);
        req.wait();
    }

    if( rank == 0 ) {
        auto reqs = comm.isend(1, tag, buf1);
        reqs += comm.isend(1, tag, buf2);
        reqs += comm.isend(1, tag, buf3);

        pout << reqs.size(), " sends have been issued", endl;

        reqs.waitall();
    }else if( rank == 1 ){
        comm.recv(0, tag, buf1);
        comm.recv(0, tag, buf2);
        comm.recv(0, tag, buf3);
    }
}

int main(int argc, char *argv[]) {
    mpi::Env env(argc, argv);
    auto comm = env.world();

    basic_p2p();
    comm_buffer();
    comm_mode();

    comm.barrier();

    nonblocking_p2p();

    return 0;
}
