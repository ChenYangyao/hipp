#include <hippmpi.h>
#include <hippnumerical.h>
using namespace HIPP;
using namespace HIPP::MPI;

using HIPP::NUMERICAL::DArray;
typedef DArray<double, 2> arr_t;        // matrix
typedef DArray<double, 1> vec_t;        // vector

int main(int argc, char const *argv[]) {    
    Env env;
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size(), 
        n_workers = n_procs-1, root = n_procs-1;

    int n_rows = 8, n_cols = 16;
    if( rank != root ) {
        vec_t a({n_cols}), b({n_cols});
        comm.bcast(b, root);
        while(1) {
            int tag = comm.recv(root, ANY_TAG, a).tag();
            if( tag == n_rows ) break;
            double res = (a*b).sum();
            comm.send(root, tag, res);
        }
    }else {
        arr_t A({n_rows, n_cols}, 1.0);
        vec_t b({n_cols}, 1.0), c({n_rows});
        comm.bcast(b, root);

        int row = 0;
        while(row < n_workers && row < n_rows) {
            comm.send(row, row, &A(row, 0), n_cols);    // send initial rows to workers
            ++ row;
        }

        double res;
        for(int i=0; i<n_rows; ++i){
            Status st = comm.recv(ANY_SOURCE, ANY_TAG, res);
            int src = st.source(), tag = st.tag();
            c[tag] = res;
            if( row != n_rows ){                        // send another row of A
                comm.send(src, row, &A(row,0), n_cols); 
                ++row;
            }else {
                comm.send(src, row);                    // an empty message
            }
        }

        pout << "c = A * b = ", c, endl;
    }

    return 0;
}
