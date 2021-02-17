#include <hippmpi.h>
#include <Eigen/Eigen>

using namespace std;
using Matrix = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using Comm = HIPP::MPI::Comm;
using Datatype = HIPP::MPI::Datatype;

class Jacobi2D {
public:
    Jacobi2D(int global_sz[2], double h, const Matrix &f, 
        const Comm &comm, double eps=1.0e-3, int max_n_iters=10000);
    void run();
    Matrix result() { return _u_new.block(1, 1, _sz[0], _sz[1]); }
private:
    /* the PDE matrices and their meta-info */
    int _global_sz[2], _sz[2];
    double _h;
    Matrix _u, _u_new, _f;

    /* communication pattern - the topology */
    Comm _comm;
    int _x_prev, _x_next, _y_prev, _y_next;
    Datatype _row_type, _col_type;
    
    /* stop criteria */
    double _eps;
    int _max_n_iters;

    void update(const Matrix &u_src, Matrix &u_dest);
    void exchange(Matrix &u);
    bool is_convergent();
};

Jacobi2D::Jacobi2D(int global_sz[2], double h, const Matrix &f, 
    const Comm &comm, double eps, int max_n_iters)
: _h(h), _f(f), _comm(nullptr), _row_type(nullptr), _col_type(nullptr),
_eps(eps), _max_n_iters(max_n_iters)
{
    /* Create cartesian topology, get the ranks of neighbors. */
    vector<int> dims;
    Comm::dims_create(comm.size(), 2, dims);
    _comm = comm.cart_create(dims, {0, 0});
    _comm.cart_shift(0, 1, _x_prev, _x_next);
    _comm.cart_shift(1, 1, _y_prev, _y_next);

    /* Pin down the local size. Initialize matrices. */
    vector<int> coords = _comm.cart_coords(_comm.rank());
    for(int i=0; i<2; ++i){
        _global_sz[i] = global_sz[i];
        auto [b, e] = HIPP::MPI::WorkDecomp1D::uniform_block(
            _global_sz[i], dims[i], coords[i]);
        _sz[i] = e-b;
    }
    _u = Matrix::Zero(_sz[0]+2, _sz[1]+2);
    _u_new = _u;
    _row_type = HIPP::MPI::DOUBLE.vector(1, _sz[1], _sz[1]+2);
    _col_type = HIPP::MPI::DOUBLE.vector(_sz[0], 1, _sz[1]+2);
}

void Jacobi2D::run() {
    for(int iter=0; iter<_max_n_iters; iter+=2){
        exchange(_u);
        update(_u, _u_new);
        exchange(_u_new);
        update(_u_new, _u);
        if( is_convergent() ) break;
    }
}

void Jacobi2D::update(const Matrix &u_src, Matrix &u_dest) {
    auto [m, n] = _sz;
    u_dest.block(1,1,m,n) = 0.25 * ( u_src.block(0,1,m,n) 
        + u_src.block(2,1,m,n) + u_src.block(1,0,m,n) 
        + u_src.block(1,2,m,n) - (_h*_h)*_f );
}

void Jacobi2D::exchange(Matrix &u) {
    auto [m, n] = _sz;

    /* Move data along x-axis */
    _comm.send(_x_prev, 0, &u(1,1), 1, _row_type);
    _comm.recv(_x_next, 0, &u(m+1,1), 1, _row_type);
    _comm.send(_x_next, 0, &u(m,1), 1, _row_type);
    _comm.recv(_x_prev, 0, &u(0,1), 1, _row_type);

    /* ... and along y-axis */
    _comm.send(_y_prev, 0, &u(1,1), 1, _col_type);
    _comm.recv(_y_next, 0, &u(1,n+1), 1, _col_type);
    _comm.send(_y_next, 0, &u(1,n), 1, _col_type);
    _comm.recv(_y_prev, 0, &u(1,0), 1, _col_type);
}

bool Jacobi2D::is_convergent() {
    auto [m, n] = _sz;
    double sum_sqr, total_sum_sqr, err;

    /* Find the RMS difference between two recent steps. */
    sum_sqr = (_u.block(1,1,m,n) - _u_new.block(1,1,m,n)).squaredNorm();
    _comm.allreduce({&sum_sqr, 1, "double"}, &total_sum_sqr, "+");
    err = std::sqrt(total_sum_sqr / (_global_sz[0]*_global_sz[1]));

    return err < _eps;
}

int main(int argc, char const *argv[]) {
    HIPP::MPI::Env env;
    auto comm = env.world();
    int rank = comm.rank();

    /* Put two source points in the field. Assume 4x4=16 processes are used. */
    int global_size[2] = {20, 20}, sz[2] = {5, 5};
    double h = 1.;
    Matrix f = Matrix::Zero(sz[0], sz[1]);
    if( rank == 1 || rank == 10 )
        f(4, 4) = -10.;

    Jacobi2D pde_solver(global_size, h, f, comm);
    pde_solver.run();
    Matrix result = pde_solver.result();

    /* Output the data. One block per line. */
    HIPP::MPI::SeqBlock seq(comm);
    HIPP::pout << HIPP::pout(result.data(), result.data()+25), endl;    

    return 0;
}


