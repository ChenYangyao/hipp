#include <hippmpi.h>
#include <hippalgorithm.h>
using namespace std;

int main(int argc, char const *argv[]) {
    HIPP::MPI::Env env;
    auto comm = env.world();
    const int nrow = 3, ncol = 4, n_total = nrow*ncol;
    double mat[nrow][ncol], mat_T[ncol][nrow];

    /* Initialize the matrix "mat" using linear-spaced values. */
    HIPP::ALGORITHM::LinSpaced(0., n_total) >> mat[0];
    HIPP::pout << "M =\n", 
        HIPP::PrtArray(mat[0], mat[0]+n_total).ncol(ncol).width(3), endl;
    
    /** 
     * Define a datatype for each column of "mat". 
     * Then, send columns of "mat" into rows of "mat_T".
     */
    auto col_type = 
        HIPP::MPI::DOUBLE.vector(nrow, 1, ncol).resized(0, sizeof(double));
    int rank = comm.rank(), tag = 0;
    comm.sendrecv({mat[0], ncol, col_type}, rank, tag, 
        {mat_T[0], n_total, "double"}, rank, tag);
    HIPP::pout << "Transposed M =\n",
        HIPP::PrtArray(mat[0], mat[0]+n_total).ncol(nrow).width(3), endl;

    return 0;
}
