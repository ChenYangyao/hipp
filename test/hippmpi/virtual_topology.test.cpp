#include <mpi_test_incl.h>

namespace HIPP::MPI {

struct VirtualTopologyCartTest : MPITestFixture {
    VirtualTopologyCartTest(Comm _comm)
    : MPITestFixture("VirtualTopologyCartTest", _comm) {
        HIPPMPI_TEST_F_ADD_CASE(dims_create_1d);
        HIPPMPI_TEST_F_ADD_CASE(dims_create_nd);
        HIPPMPI_TEST_F_ADD_CASE(ring_1d_creation);
        HIPPMPI_TEST_F_ADD_CASE(ring_1d_communication);
        HIPPMPI_TEST_F_ADD_CASE(line_1d_creation);
        HIPPMPI_TEST_F_ADD_CASE(line_1d_communication);
        HIPPMPI_TEST_F_ADD_CASE(tori_2d);
    }

    void dims_create_1d() {
        int dims2[1] = {0}, dims_dst[1] = {n_procs};
        comm.dims_create(n_procs, 1, dims2);
        expect_eq_range(dims2, dims_dst, "dims_create()");

        vector<int> dims;
        comm.dims_create(n_procs, 1, dims);
        expect_eq_range(dims, dims_dst, "dims_create()");

        dims = {0};
        comm.dims_create(n_procs, dims);
        expect_eq_range(dims, dims_dst, "dims_create()");

        dims = comm.dims_create(n_procs, 1);
        expect_eq_range(dims, dims_dst, "dims_create()");
    }
    void dims_create_nd() {
        vector<int> dims, dims_dst;

        dims_dst = {3,2};
        int dims2[2] = {0, 0};
        comm.dims_create(6, 2, dims2);
        expect_eq_range(dims2, dims_dst, "dims_create()");

        dims = {};
        comm.dims_create(6, 2, dims);
        expect_eq_range(dims, dims_dst, "dims_create()");

        dims = {0,0};
        comm.dims_create(6, dims);
        expect_eq_range(dims, dims_dst, "dims_create()");

        dims = comm.dims_create(6, 2);
        expect_eq_range(dims, dims_dst, "dims_create()");

        dims = {}; dims_dst = {7,1};
        comm.dims_create(7, 2, dims);
        expect_eq_range(dims, dims_dst, "dims_create()");

        dims = {0,3,0}; dims_dst = {2,3,1};
        comm.dims_create(6, 3, dims);
        expect_eq_range(dims, dims_dst, "dims_create()");

        bool has_except = false;
        try {
            dims = {0,3,0};
            comm.dims_create(7, 3, dims);
        }catch( ErrMPI & e ) {
            has_except = true;
        }
        expect_true(has_except, "3d decomposition should raise");
    }

    void ring_1d_creation() {
        vector<int> dims {n_procs}, periods {1};
        auto ring_comm = comm.cart_create(dims, periods, 0);

        /* Check the shift. */
        for(int i=1; i<=n_procs; ++i){
            auto [src, dst] = ring_comm.cart_shift(0, i);
            int src_targ = (rank - i + n_procs)%n_procs, 
                dst_targ = (rank + i)%n_procs;
            expect_eq(src, src_targ, "disp ", i);
            expect_eq(dst, dst_targ, "disp ", i);
        }
        for(int i=1; i<=n_procs; ++i){
            auto [src, dst] = ring_comm.cart_shift(0, -i);
            int src_targ = (rank + i)%n_procs, 
                dst_targ = (rank - i + n_procs)%n_procs;
            expect_eq(src, src_targ, "disp ", i);
            expect_eq(dst, dst_targ, "disp ", i);
        }
    }
    void ring_1d_communication() {
        vector<int> dims {n_procs}, periods {1};
        auto ring_comm = comm.cart_create(dims, periods, 0);

        const int ndims = ring_comm.cartdim_get();
        assert_eq(ndims, 1, "cartdim_get()");
        
        vector<int> ibuf(ndims*2);
        ring_comm.neighbor_allgather(rank, ibuf);
        const int left = (rank - 1 + n_procs)%n_procs, 
            right = (rank+1)%n_procs;
        expect_eq(ibuf[0], left, "neighbor_allgather left");
        expect_eq(ibuf[1], right, "neighbor_allgather right");
    }
    
    void line_1d_creation() {
        vector<int> dims {n_procs}, periods {0};
        auto line_comm = comm.cart_create(dims, periods, 0);

        /* Check the shift. */
        for(int i=1; i<=n_procs; ++i){
            auto [src, dst] = line_comm.cart_shift(0, i);
            int src_targ = rank - i, dst_targ = rank + i;
            if( src_targ < 0 ) src_targ = PROC_NULL;
            if( dst_targ >= n_procs ) dst_targ = PROC_NULL;
            expect_eq(src, src_targ, "disp ", i);
            expect_eq(dst, dst_targ, "disp ", i);
        }
        for(int i=1; i<=n_procs; ++i){
            auto [src, dst] = line_comm.cart_shift(0, -i);
            int src_targ = rank + i, dst_targ = rank - i;
            if( src_targ >= n_procs ) src_targ = PROC_NULL;
            if( dst_targ < 0 ) dst_targ = PROC_NULL;
            expect_eq(src, src_targ, "disp ", i);
            expect_eq(dst, dst_targ, "disp ", i);
        }
    }
    void line_1d_communication() {
        vector<int> dims {n_procs}, periods {0};
        auto line_comm = comm.cart_create(dims, periods, 0);

        const int ndims = line_comm.cartdim_get();
        assert_eq(ndims, 1, "cartdim_get()");
        
        vector<int> ibuf(ndims*2);
        line_comm.neighbor_allgather(rank, ibuf);
        int left = rank - 1, right = rank + 1;
        if( rank == 0 ) left = 0;
        if( rank == n_procs-1 ) right = 0;
        expect_eq(ibuf[0], left, "neighbor_allgather left");
        expect_eq(ibuf[1], right, "neighbor_allgather right");
    }
    
    void tori_2d() {
        constexpr int ndims = 2;
        vector<int> dims, periods {1,1};
        comm.dims_create(n_procs, ndims, dims);
        assert_eq(dims.size(), (size_t)ndims, "dims_create()");
        int d0 = dims[0], d1 = dims[1];
        vector<int> coords = { rank/d1, rank%d1 };

        auto tori_comm = comm.cart_create(dims, periods, 0);

        int dims2[ndims] = {d0, d1}, periods2[ndims] = {1,1};
        auto tori_comm2 = comm.cart_create(dims2, periods2, 0);
        
        assert_eq(tori_comm.cartdim_get(), ndims, "cartdims_get()");
        assert_eq(tori_comm2.cartdim_get(), ndims, "cartdims_get()");

        /* Check the topology meta. */
        vector<int> cur_dims, cur_periods, cur_coords;
        tori_comm.cart_get(cur_dims, cur_periods, cur_coords);
        expect_eq_range(cur_dims, dims, "cart_get() dims");
        expect_eq_range(cur_periods, periods, "cart_get() periods");
        expect_eq_range(cur_coords, coords, "cart_get() coords");

        int cur_dims2[ndims], cur_periods2[ndims], cur_coords2[ndims];
        tori_comm.cart_get(cur_dims2, cur_periods2, cur_coords2);
        expect_eq_range(cur_dims2, dims, 
            "cart_get(contiguous buffer) dims");
        expect_eq_range(cur_periods2, periods, 
            "cart_get(contiguous buffer) periods");
        expect_eq_range(cur_coords2, coords, 
            "cart_get(contiguous buffer) coords");

        expect_eq(tori_comm.cart_rank(cur_coords), rank, 
            "cart_rank()");
        expect_eq(tori_comm.cart_rank(cur_coords2), rank, 
            "cart_rank(contiguous buffer)");

        cur_coords = tori_comm.cart_coords(rank);
        tori_comm.cart_coords(rank, cur_coords2);
        expect_eq_range(cur_coords, coords, "cart_coords()");
        expect_eq_range(cur_coords2, coords, "cart_coords(contiguous buffer)");

        /* Check the shift. */
        for(int i=1; i<=d0; ++i) {
            auto [src, dst] = tori_comm.cart_shift(0, i);
            int c0 = (coords[0] - i + d0)%d0, c1 = coords[1];
            int src_targ = c0 * d1 + c1;
            c0 = (coords[0] + i)%d0;
            int dst_targ = c0 * d1 + c1;
            expect_eq(src, src_targ, "disp dim0 ", i);
            expect_eq(dst, dst_targ, "disp dim0 ", i);
        }
        for(int j=1; j<=d1; ++j) {
            auto [src, dst] = tori_comm.cart_shift(1, j);
            int c0 = coords[0], c1 = (coords[1] - j + d1)%d1;
            int src_targ = c0 * d1 + c1;
            c1 = (coords[1] + j)%d1;
            int dst_targ = c0 * d1 + c1;
            expect_eq(src, src_targ, "disp dim1 ", j);
            expect_eq(dst, dst_targ, "disp dim1 ", j);
        }

    }
};


struct VirtualTopologyGraphTest : MPITestFixture {
    VirtualTopologyGraphTest(Comm _comm)
    : MPITestFixture("VirtualTopologyGraphTest", _comm) {
        HIPPMPI_TEST_F_ADD_CASE(graph_create_ring);
        HIPPMPI_TEST_F_ADD_CASE(graph_create_ring_asymmetric);
    }

    void graph_create_ring() {
        vector<int> index, edges;
        int last_idx = 0;
        for(int i=0; i<n_procs; ++i){
            int left = (i-1+n_procs)%n_procs, right = (i+1)%n_procs;
            edges.push_back(left);
            edges.push_back(right);
            last_idx += 2;
            index.push_back(last_idx);
        }
        auto ring_comm = comm.graph_create(n_procs, index.data(), 
            edges.data(), 0);


        auto [nnodes, nedges] = ring_comm.graphdims_get();
        expect_true(nnodes, n_procs, "graphdims_get() nnodes");
        expect_true(nedges, 2*n_procs, "graphdims_get() nedges");

        vector<int> out_index(nnodes), out_edges(nedges);
        ring_comm.graph_get(nnodes, nedges, out_index.data(), out_edges.data());
        expect_eq_range(out_index, index, "graph_get() index");
        expect_eq_range(out_edges, edges, "graph_get() edges");

        ring_comm.graph_get(out_index, out_edges);
        expect_eq_range(out_index, index, "graph_get(contiguous buffer) index");
        expect_eq_range(out_edges, edges, "graph_get(contiguous buffer) edges");

        auto [out2_index, out2_edges] = ring_comm.graph_get();
        expect_eq_range(out2_index, index, "graph_get() -> pair index");
        expect_eq_range(out2_edges, edges, "graph_get() -> pair edges");
    }
    void graph_create_ring_asymmetric() {
        vector<int> index, edges;
        int last_idx = 0;
        for(int i=0; i<n_procs; ++i){
            int right = (i+1)%n_procs;
            edges.push_back(right);
            last_idx += 1;
            index.push_back(last_idx);
        }
        auto ring_comm = comm.graph_create(index, edges, 0);

        auto [nnodes, nedges] = ring_comm.graphdims_get();
        expect_true(nnodes, n_procs, "graphdims_get() nnodes");
        expect_true(nedges, n_procs, "graphdims_get() nedges");

        vector<int> out_index(nnodes), out_edges(nedges);
        ring_comm.graph_get(nnodes, nedges, out_index.data(), out_edges.data());
        expect_eq_range(out_index, index, "graph_get() index");
        expect_eq_range(out_edges, edges, "graph_get() edges");

        ring_comm.graph_get(out_index, out_edges);
        expect_eq_range(out_index, index, "graph_get(contiguous buffer) index");
        expect_eq_range(out_edges, edges, "graph_get(contiguous buffer) edges");

        auto [out2_index, out2_edges] = ring_comm.graph_get();
        expect_eq_range(out2_index, index, "graph_get() -> pair index");
        expect_eq_range(out2_edges, edges, "graph_get() -> pair edges");
    }
};

} // namespace HIPP::MPI

int main(int argc, char *argv[])
{
    using namespace HIPP::MPI;

    Env env(argc, argv);
    auto comm = env.world();

    for(int i=0; i<16; ++i){
        VirtualTopologyCartTest{comm}();
    }

    for(int i=0; i<16; ++i){
        VirtualTopologyGraphTest{comm}();
    }


    return 0;
}