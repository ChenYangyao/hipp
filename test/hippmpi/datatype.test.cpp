#include <mpi_test_incl.h>

namespace HIPP::MPI {

struct DatatypeTest : MPITestFixture {
    DatatypeTest(Comm _comm) 
    : MPITestFixture("DatatypeTest", _comm), arr_dt {INT} {
        
        assert_ge(n_procs, 2, "n_procs");

        HIPPMPI_TEST_F_ADD_CASE(resized);
        HIPPMPI_TEST_F_ADD_CASE(contiguous);
        HIPPMPI_TEST_F_ADD_CASE(vector_);
        HIPPMPI_TEST_F_ADD_CASE(hvector);
        HIPPMPI_TEST_F_ADD_CASE(indexed_block);
        HIPPMPI_TEST_F_ADD_CASE(hindexed_block);
        HIPPMPI_TEST_F_ADD_CASE(indexed);
        HIPPMPI_TEST_F_ADD_CASE(hindexed);
        HIPPMPI_TEST_F_ADD_CASE(struct_);
        HIPPMPI_TEST_F_ADD_CASE(c_struct);
    }

    void resized() {
        auto dt = arr_dt;
        auto c_dt = dt.resized(0, sizeof(int)*8);
        _init_data();
        if( rank == 0 ) {
            for(int i=0; i<8; ++i){
                pout << " - send column ", i, endl;
                comm.send(1, i, &arr[0][i], 4, c_dt);
            }
        }else if( rank == 1 ) {
            for(int i=0; i<8; ++i){
                auto st = comm.probe(0, i);
                assert_eq(st.count(dt), 4);
                comm.recv(0, i, &arr[0][i], 4, c_dt);
            }
            _chk_arr_eq_to_dest();
            
        }
    }

    void contiguous() {
        auto dt = arr_dt;
        auto c_dt = dt.contiguous(8);
        _init_data();
        if( rank == 0 ) {
            for(int i=0; i<4; ++i){
                comm.send(1, i, &arr[i][0], 1, c_dt);
                pout << " - sends 1 c_dt at row ", i, endl;
            }
        }else if( rank == 1 ) {
            for(int i=0; i<4; ++i){
                auto st = comm.probe(0, i);
                assert_eq(st.count(c_dt), 1);
                assert_eq(st.count(dt), 8);
                comm.recv(0, i, arr[i]);
            }
            _chk_arr_eq_to_dest();
        }
    }

    void vector_() {
        auto dt = arr_dt;
        auto c_dt = dt.vector(4,2,8);

        _init_data();
        if( rank == 0 ) {
            for(int i=0; i<4; ++i){
                pout << " - send column ", 2*i, " to ", 2*i+1, endl;
                comm.send(1, i, &arr[0][2*i], 1, c_dt);
            }
        }else if( rank == 1 ) {
            for(int i=0; i<4; ++i){
                auto st = comm.probe(0, i);
                assert_eq(st.count(dt), 8);    
                comm.recv(0, i, &arr[0][2*i], 1, c_dt);
            }
            _chk_arr_eq_to_dest();
        }
    }

    void hvector() {
        auto dt = arr_dt;
        auto c_dt = dt.hvector(4,2,sizeof(int)*8);

        _init_data();
        if( rank == 0 ) {
            for(int i=0; i<4; ++i){
                pout << " - send column ", 2*i, " to ", 2*i+1, endl;
                comm.send(1, i, &arr[0][2*i], 1, c_dt);
            }
        }else if( rank == 1 ) {
            for(int i=0; i<4; ++i){
                auto st = comm.probe(0, i);
                assert_eq(st.count(dt), 8);
                comm.recv(0, i, &arr[0][2*i], 1, c_dt);
            }
            int *p = arr[0], *p_dest = arr_dest[0];
            for(int i=0; i<32; ++i){
                expect_eq(p[i], p_dest[i], 
                "indexed (", i/8, ", ", i%8, ")");
            }
        }
    }

    void indexed_block() {
        auto dt = arr_dt;

        int displs[4] = {0,8,16,24};
        auto c_dt = dt.indexed_block(4,2,displs);

        _init_data();
        if( rank == 0 ) {
            for(int i=0; i<4; ++i){
                pout << " - send column ", 2*i, " to ", 2*i+1, endl;
                comm.send(1, i, &arr[0][2*i], 1, c_dt);
            }
        }else if( rank == 1 ) {
            for(int i=0; i<4; ++i){
                auto st = comm.probe(0, i);
                assert_eq(st.count(dt), 8);
                comm.recv(0, i, &arr[0][2*i], 1, c_dt);
            }
            int *p = arr[0], *p_dest = arr_dest[0];
            for(int i=0; i<32; ++i){
                expect_eq(p[i], p_dest[i], 
                "indexed (", i/8, ", ", i%8, ")");
            }
        }
    }

    void hindexed_block() {
        auto dt = arr_dt;
        constexpr int sz = sizeof(int);
        aint_t displs[4] = {0*sz,8*sz,16*sz,24*sz};
        auto c_dt = dt.hindexed_block(4,2,displs);

        _init_data();
        if( rank == 0 ) {
            for(int i=0; i<4; ++i){
                pout << " - send column ", 2*i, " to ", 2*i+1, endl;
                comm.send(1, i, &arr[0][2*i], 1, c_dt);
            }
        }else if( rank == 1 ) {
            for(int i=0; i<4; ++i){
                auto st = comm.probe(0, i);
                assert_eq(st.count(dt), 8);
                comm.recv(0, i, &arr[0][2*i], 1, c_dt);
            }
            int *p = arr[0], *p_dest = arr_dest[0];
            for(int i=0; i<32; ++i){
                expect_eq(p[i], p_dest[i], 
                "indexed (", i/8, ", ", i%8, ")");
            }
        }
    }

    void indexed() {
        auto dt = arr_dt;
        int blklens[4] = {2,2,2,2}, displs[4] = {0,8,16,24};
        auto c_dt = dt.indexed(4,blklens,displs);

        _init_data();
        if( rank == 0 ) {
            for(int i=0; i<4; ++i){
                pout << " - send column ", 2*i, " to ", 2*i+1, endl;
                comm.send(1, i, &arr[0][2*i], 1, c_dt);
            }
        }else if( rank == 1 ) {
            for(int i=0; i<4; ++i){
                auto st = comm.probe(0, i);
                assert_eq(st.count(dt), 8);
                comm.recv(0, i, &arr[0][2*i], 1, c_dt);
            }
            int *p = arr[0], *p_dest = arr_dest[0];
            for(int i=0; i<32; ++i){
                expect_eq(p[i], p_dest[i], 
                "indexed (", i/8, ", ", i%8, ")");
            }
        }
    }

    void hindexed() {
        auto dt = arr_dt;
        constexpr int sz = sizeof(int);
        int blklens[4] = {2,2,2,2}; 
        aint_t displs[4] = {0*sz,8*sz,16*sz,24*sz};
        auto c_dt = dt.hindexed(4,blklens,displs);

        _init_data();
        if( rank == 0 ) {
            for(int i=0; i<4; ++i){
                pout << " - send column ", 2*i, " to ", 2*i+1, endl;
                comm.send(1, i, &arr[0][2*i], 1, c_dt);
            }
        }else if( rank == 1 ) {
            for(int i=0; i<4; ++i){
                auto st = comm.probe(0, i);
                assert_eq(st.count(dt), 8);
                comm.recv(0, i, &arr[0][2*i], 1, c_dt);
            }
            int *p = arr[0], *p_dest = arr_dest[0];
            for(int i=0; i<32; ++i){
                expect_eq(p[i], p_dest[i], 
                "indexed (", i/8, ", ", i%8, ")");
            }
        }
    }

    void struct_() {
        auto dt = arr_dt;
        constexpr int sz = sizeof(int);
        int blklens[4] = {2,2,2,2}; 
        aint_t displs[4] = {0*sz,8*sz,16*sz,24*sz};
        Datatype dtypes[4] = {INT, INT, INT, INT};
        auto c_dt = dt.struct_(4,blklens,displs,dtypes);

        _init_data();
        if( rank == 0 ) {
            for(int i=0; i<4; ++i){
                pout << " - send column ", 2*i, " to ", 2*i+1, endl;
                comm.send(1, i, &arr[0][2*i], 1, c_dt);
            }
        }else if( rank == 1 ) {
            for(int i=0; i<4; ++i){
                auto st = comm.probe(0, i);
                assert_eq(st.count(dt), 8);
                comm.recv(0, i, &arr[0][2*i], 1, c_dt);
            }
            int *p = arr[0], *p_dest = arr_dest[0];
            for(int i=0; i<32; ++i){
                expect_eq(p[i], p_dest[i], 
                "indexed (", i/8, ", ", i%8, ")");
            }
        }
    }

    void c_struct() {
        int blklens[] = {1,1,1,4};
        Datatype dtypes[] = {INT, LONG, FLOAT, DOUBLE};
        aint_t disps[] = { _get_offset(&st_data_t::i), 
            _get_offset(&st_data_t::l), _get_offset(&st_data_t::f), 
            _get_offset(&st_data_t::d) };
        aint_t tot_sz = sizeof(st_data_t);

        auto c_dt = Datatype::struct_(4, blklens, disps, 
            dtypes).resized(0, tot_sz);
        _init_data();
        if( rank == 0 ) {
            pout << 
                " - struct size = ", tot_sz, "\n",
                " - dtypes{", pout(dtypes,dtypes+4), "}\n",
                " - disps"; 
            for(int i=0; i<4; ++i) {
                pout << "   - "; dtypes[i].info(cout, 0) << '\n';
            }
            pout << " - blklens{", pout(blklens,blklens+4), "}\n",
                " - struct datatype = "; 
            c_dt.info(cout, 0) << endl;

            comm.send(1,0,st_data,8,c_dt);
            pout << " - send 8 st_data_t", endl;
        }else if( rank == 1 ) {
            comm.recv(0,0,st_data,8,c_dt);
            _chk_st_data_eq_to_dest();
        }
    }
private:
    void _init_data() {
        int sz = RawArrayTraits<decltype(arr)>::size;
        int *p = arr[0], *p_dest = arr_dest[0];
        if(is_root) {
            for(int i=0; i<sz; ++i)
                p[i] = i;
        }else {
            std::fill_n(arr[0], sz, -1);
        }
        for(int i=0; i<sz; ++i)
            p_dest[i] = i;

        if( is_root ) {
            for(int i=0; i<8; ++i){
                st_data[i].i = 8*i;
                st_data[i].l = 8*i+1;
                st_data[i].f = 8*i+2;
                for(int j=0; j<4; ++j)
                    st_data[i].d[j] = 8*i+3+0.1*j;
            }
        }else {
            for(int i=0; i<8; ++i){
                st_data[i].i = -1;
                st_data[i].l = -1;
                st_data[i].f = -1;
                std::fill_n(st_data[i].d, 4, -1);
            }
        }
        for(int i=0; i<8; ++i){
            st_data_dest[i].i = 8*i;
            st_data_dest[i].l = 8*i+1;
            st_data_dest[i].f = 8*i+2;
            for(int j=0; j<4; ++j)
                st_data_dest[i].d[j] = 8*i+3+0.1*j;
        }
    }
    template<typename ...Args>
    void _chk_arr_eq_to_dest(Args &&...args) {
        const char *pref = sizeof...(args)>0 ? ", " : "";
        for(int i=0; i<4; ++i){
            expect_eq_range(arr[i], arr_dest[i], 
                std::forward<Args>(args)..., pref, "arr row ", i);
        }
    }

    template<typename ...Args>
    void _chk_st_data_eq_to_dest(Args &&...args) {
        const char *pref = sizeof...(args)>0 ? ", " : "";
        for(int i=0; i<8; ++i){
            expect_eq(st_data[i].i, st_data_dest[i].i,
                std::forward<Args>(args)..., pref, "index ", i, ", elem i");
            expect_eq(st_data[i].l, st_data_dest[i].l,
                std::forward<Args>(args)..., pref, "index ", i, ", elem l");
            expect_eq(st_data[i].f, st_data_dest[i].f,
                std::forward<Args>(args)..., pref, "index ", i, ", elem f");
            expect_eq_range(st_data[i].d, st_data_dest[i].d,
                std::forward<Args>(args)..., pref, "index ", i, ", elem d");
        }
    }

    template<typename St, typename Elem>
    constexpr aint_t _get_offset(Elem St::* ptr) {
        return (char *)&( (St*)(NULL)->*ptr ) - (char *)(NULL);
    }

    int arr[4][8], arr_dest[4][8];
    Datatype arr_dt;

    struct st_data_t {
        int i;
        long l;
        float f;
        double d[4];
    };
    st_data_t st_data[8], st_data_dest[8];
};


} // namespace HIPP::MPI

int main(int argc, char *argv[])
{
    using namespace HIPP::MPI;

    Env env(argc, argv);
    auto comm = env.world();

    for(int i=0; i<5; ++i){
        DatatypeTest{comm}();        
    }

    return 0;
}
