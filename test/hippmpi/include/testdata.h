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

} // namespace HIPP::MPI
#endif	//HIPPMPI_TEST_TESTDATA_H_