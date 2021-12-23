#include <hippmpi.h>
#include <hippnumerical.h>

using namespace HIPP;
using namespace std;
namespace mpi = HIPP::MPI;
namespace nu = HIPP::NUMERICAL;
using nu::SArray;
using nu::SVec;
using nu::DArray;

int main(int argc, char *argv[]) {
    mpi::Env env(argc, argv);
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();
    
    int tag = 0, buf[5];
    if( rank == 0 ) {
        comm.send(1, tag, buf);
    }else if ( rank == 1 ) {
        comm.recv(0, tag, buf);
    }

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

    return 0;
}
