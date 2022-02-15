#ifndef _HIPPMPI_MPI_TEST_DATA_BUFFER_H_
#define _HIPPMPI_MPI_TEST_DATA_BUFFER_H_

#include "mpi_test_incl.h"

namespace HIPP::MPI {

template<typename ScalarT>
class MPITestDataBuffer {
public:
    using scalar_t = ScalarT;
    
    MPITestDataBuffer(MPITestFixture &test_f, bool init = true) 
    : x_v8(8), x_v83(8), x_v834(8), _x_p8(8), x_p8( _x_p8.data() ), 
    _test_f(test_f)
    {
        if( init ) init_values();
        else zero_values();
    }
    ~MPITestDataBuffer() noexcept {}

    void init_values() {
        x_s = 0;
        for(int i=0; i<3; ++i) {
            x_rawarr3[i] = i+1;
            x_arr3[i] = i+11;
            for(int j=0; j<4; ++j){
                x_rawarr34[i][j] = i*j+21;
                x_arr34[i][j] = i*j+31;
            }
        } 
        for(int i=0; i<8; ++i){
            x_v8[i] = i+41;
            for(int j=0; j<3; ++j){
                x_v83[i][j] = i+j+51;
                for(int k=0; k<4; ++k){
                    x_v834[j][j][k] = i+j+k+61;
                }
            }
            _x_p8[i] = i+71;
        }
    }    
    void zero_values() {
        x_s = 0;
        std::fill_n(&x_rawarr3[0], 3, 0);
        std::fill_n(&x_rawarr34[0][0], 12, 0);
        std::fill_n(&x_arr3[0], 3, 0);
        std::fill_n(&x_arr34[0][0], 12, 0);
        std::fill_n(&x_v8[0], 8, 0);
        std::fill_n(&x_v83[0][0], 24, 0);
        std::fill_n(&x_v834[0][0][0], 96, 0);
        std::fill_n(&x_p8[0], 8, 0);
    }

    bool expect_eq(const MPITestDataBuffer &target) {
        auto &o =  target;
        bool suc = true;

        suc = suc && _test_f.expect_eq(x_s, o.x_s, "x_s");
        suc = suc && _test_f.expect_eq_range(x_rawarr3, o.x_rawarr3, 
            "x_rawarr3");
        for(int i=0; i<3; ++i){
            suc = suc && _test_f.expect_eq_range(x_rawarr34[i], 
                o.x_rawarr34[i], "x_rawarr34[", i, "]");
        }
        suc = suc && _test_f.expect_eq_range(x_arr3, o.x_arr3, "x_arr3");
        for(int i=0; i<3; ++i){
            suc = suc && _test_f.expect_eq_range(x_arr34[i], 
                o.x_arr34[i], "x_arr34[", i, "]");
        }


        suc = suc && _test_f.expect_eq_range(x_v8, o.x_v8, "x_v8");
        for(int i=0; i<8; ++i){
            suc = suc && _test_f.expect_eq_range(x_v83[i], 
                o.x_v83[i], "x_v83[", i, "]");
        }
        for(int i=0; i<8; ++i){
            for(int j=0; j<3; ++j){
                suc = suc && _test_f.expect_eq_range(x_v834[i][j], 
                    o.x_v834[i][j], "x_v834[", i, "][", j, "]");
            }
        }


        suc = suc && _test_f.expect_eq_buff(x_p8, o.x_p8, 8, "x_p8");

        return suc;
    }

    void assert_eq(const MPITestDataBuffer &target) {
        bool suc = expect_eq(target);
        _test_f.assert_true(suc);
    }

    scalar_t x_s;
    scalar_t x_rawarr3[3], x_rawarr34[3][4];
    std::array<scalar_t, 3> x_arr3;
    std::array<std::array<scalar_t, 4>, 3> x_arr34;
    
    vector<scalar_t> x_v8;
    vector<std::array<scalar_t, 3> > x_v83;
    vector<std::array<std::array<scalar_t, 4>, 3> > x_v834;

    vector<scalar_t> _x_p8;
    scalar_t *x_p8;

    MPITestFixture &_test_f;
};


} // namespace HIPP::MPI

#endif	//_HIPPMPI_MPI_TEST_DATA_BUFFER_H_