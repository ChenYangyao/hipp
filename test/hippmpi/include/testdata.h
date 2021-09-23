#ifndef HIPPMPI_TEST_TESTDATA_H_
#define HIPPMPI_TEST_TESTDATA_H_
#include <hippmpi.h>

namespace HIPP::MPI {

class ValidateData {
public:
    template<typename T1, typename T2>
    static void chk_seq_eq(const T1 &x, const T2 &y) {
        auto bx = std::begin(x), ex = std::end(x);
        auto by = std::begin(y), ey = std::end(y);
        size_t szx = ex-bx, szy = ey-by;
        while( bx != ex ){
            if( by == ey )
                ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
                    "  ... sz x != sz y(", szx, " and ", szy, ")\n");
            if( *bx != *by )
                ErrRuntime::throw_(ErrRuntime::eRANGE, emFLPFB, 
                    "  ... x[", bx-std::begin(x), 
                    "] != y[", bx-std::begin(x), "] (", 
                    *bx, " and ", *by, ")\n");
            bx++;
            by++;
        }
        assert(by == ey);
    }

    template<typename T1, typename T2, typename Cnts>
    static void chk_seq_eq_chunks(const T1 &x, const T2 &y, const Cnts &cnts) {
        auto bx = std::begin(x), ex = std::end(x);
        auto by = std::begin(y), ey = std::end(y);
        auto bc = std::begin(cnts), ec = std::end(cnts);
        for(auto pc=bc; pc!=ec; ++pc){
            int cnt = *pc;
            for(int i=0; i<cnt; ++i){
                assert(bx != ex); assert(by != ey);
                if( *bx != *by )
                    ErrRuntime::throw_(ErrRuntime::eRANGE, emFLPFB, 
                        "  ... chunk ", pc-bc, ", x[", i, "] != y[", i, "] (", 
                        *bx, " and ", *by, ")\n");
                ++bx; ++by;
            }
        }
        assert(bx == ex);
        assert(by == ey);
    }

    template<typename T>
    static ostream & prt_bytes(ostream &os, const T *ptr, size_t n){
        size_t sz = sizeof(T) * n;
        unsigned char *addr = (unsigned char *)ptr;
        os << "{" << std::hex;
        if( sz > 0 ) os << (unsigned int )(addr[0]);
        for(size_t i=1; i<sz; ++i)
            os << " ," << (unsigned int )(addr[i]);
        os << std::dec << "}";
        return os;
    }
};

class TestDataBase {
public:
    TestDataBase(Comm &_comm) 
    : comm(_comm), rank(comm.rank()), n_procs(comm.size()) {}
    virtual ~TestDataBase() {}

    Comm &comm;
    int rank, n_procs;
};

class TestData {
public:
    void init() {
        x = 0;
        y[0] = 1.1; y[1] = 2.2; y[2] = 3.3;
        z = {-4.1, 5.2, 6.3};
        nz = z.size();

        _buff = {-10,-20,-30};
        ptr = _buff.data();
        sz_buff = _buff.size();

        arr = {99.1, 100.2, 101.3};
    }
    void init_empty(){
        x = -1;
        y[0] = -1.; y[1] = -1.; y[2] = -1.;
        z = {-1.,-1.,-1.};
        nz = z.size();

        _buff = {-1, -1, -1};
        ptr = _buff.data();
        sz_buff = _buff.size();

        arr = {-1., -1., -1.};
    }
    void compare(const TestData &o){
        assert(x == o.x);
        ValidateData::chk_seq_eq(y, o.y);
        ValidateData::chk_seq_eq(z, o.z);
        ValidateData::chk_seq_eq(_buff, o._buff);
        ValidateData::chk_seq_eq(arr, o.arr);
    }

    int x;
    double y[3];
    vector<double> z;
    int nz;

    vector<long> _buff;
    long *ptr;
    int sz_buff;

    std::array<double, 3> arr;
};


class TestGatherData : public TestDataBase {
public:
    TestGatherData(Comm &_comm) : TestDataBase(_comm) {
        is_root = rank == 0;
        assert(n_procs == 4);
        if( is_root ) init_master();
        else init_worker();
    }

    void init_master() {
        cnts = {1,2,3,4}, offs = {0,1,3,6};

        recvbuf.assign(cnts.back()+offs.back(), -1);
        res = {1,2,2,3,3,3,4,4,4,4};

        recvbuf_eqsz.assign(n_procs*3,-1);
        res_eqsz = {1,1,1,2,2,2,3,3,3,4,4,4};
        init_any();
    }
    void init_worker() {
        init_any();
    }
    void init_any() {
        sendbuf.assign(rank+1, rank+1);
        sendbuf_eqsz.assign(3, rank+1);
    }

    void chk_res() {
        ValidateData::chk_seq_eq_chunks(recvbuf, res, cnts);
        recvbuf.assign(recvbuf.size(), -1);
    }
    void chk_res_eqsz(bool chk_recv = true) {
        ValidateData::chk_seq_eq(recvbuf_eqsz, res_eqsz);
        recvbuf_eqsz.assign(recvbuf_eqsz.size(), -1.0f);
    }

    bool is_root;

    vector<int> cnts, offs;
    vector<int> sendbuf, recvbuf, res;

    vector<float> sendbuf_eqsz, recvbuf_eqsz, res_eqsz;
};

} // namespace HIPP::MPI
#endif	//HIPPMPI_TEST_TESTDATA_H_