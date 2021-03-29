#include <hippmpi.h>
#include <hippalgorithm.h>
using namespace std;

void master_do(int n_rows, int n_cols, 
    const double *A, const double *b, double *c,
    HIPP::MPI::Comm &comm)
{
    const int root = 0, n_procs = comm.size(); 

    comm.bcast({b, n_cols, "double"}, root);

    /* Assign task to each slave. */
    int n_sent = 0;
    for(int i=0; i<std::min(n_rows, n_procs-1); ++i){
        comm.send(i+1, n_sent, A+n_sent*n_cols, n_cols, "double");
        ++n_sent;
    }

    /* Wait reply from any slave, and assign a new task to it. */
    for(int i=0; i<n_rows; ++i){
        double ans;
        auto status = comm.recv(HIPP::MPI::ANY_SOURCE, HIPP::MPI::ANY_TAG, 
            &ans, 1, "double");
        c[status.tag()] = ans;

        const double *buff = A + n_sent*n_cols; 
        int count = n_cols;
        if( n_sent == n_rows ){    // All tasks were finished. Send stop signal.
            buff = NULL; count = 0; 
        }
        comm.send(status.source(), n_sent, buff, count, "double");
        if( n_sent < n_rows ) ++n_sent;
    }
}

void slave_do(int n_rows, int n_cols, HIPP::MPI::Comm &comm){
    const int root = 0;
    vector<double> row(n_cols), b(n_cols);
    comm.bcast(b, root);

    if( comm.rank() > n_rows ) return;

    /* Wait for assigned task, finish it, and send back the answer. */
    while(true){
        int tag = comm.recv(root, HIPP::MPI::ANY_TAG, row).tag();
        if( tag == n_rows ) break;  // Stop working on a stop signal.
        
        double ans = 0.;
        for(int i=0; i<n_cols; ++i) ans += row[i]*b[i];
        comm.send(root, tag, &ans, 1, "double");
    }
}

int main(int argc, char const *argv[]) {    
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank();

    const int n_rows = 5, n_cols = 8, n_elems = n_rows*n_cols;
    if( rank == 0 ){
        vector<double> A(n_elems), b(n_cols, 1.0), c(n_rows);
        HIPP::ALGORITHM::LinSpaced(0., n_elems) >> A.data();
        master_do(n_rows, n_cols, A.data(), b.data(), c.data(), comm);

        HIPP::pout << "A =[\n", 
            HIPP::PrtArray(A.begin(), A.end()).ncol(n_cols).width(3), "]\n",
            "b = [", b, "]\n",
            "c = [", c, "]\n"; 
    }else{
        slave_do(n_rows, n_cols, comm);
    }

    return 0;
}
