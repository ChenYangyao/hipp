#include <hippnumerical.h>
#include <gtest/gtest.h>

namespace HIPP::NUMERICAL {
namespace {

/* FMinimizer */
class FMinimizerTest: public ::testing::Test {
protected:
    typedef FMinimizer target_t;

    FMinimizerTest()
    :fmin_golden(target_t::GOLDEN_SECTION), 
    fmin_quad_golden(target_t::QUAD_GOLDEN)
    {}
    ~FMinimizerTest() override {}

    std::function<double(double)> fsin = 
        [](double x)->double { return std::sin(x); };
    static double fquad(double x) {
        return (x-1.0)*(x-1.0) + 5.0;
    }
    target_t fmin_default, fmin_golden, fmin_quad_golden;

    void chk_result(double x_est, double x_true, size_t n_iters, 
        double eps=target_t::DEFAULT_EPSABS)
    {
        EXPECT_LT(n_iters, target_t::DEFAULT_MAX_N_ITERS);
        EXPECT_LT(std::fabs(x_est-x_true), 10.*eps);
    }
};

TEST_F(FMinimizerTest, DefaultType){
    auto &fmin = fmin_default;
    size_t n_iters = fmin.set_fn(fsin, 4.0, 0., 6.5).iterate_until();    
    chk_result(fmin.x_minimum(), 3.1415926*1.5, n_iters);
    
    n_iters = fmin.set_fn(fquad, 0., -5., 5.).iterate_until();
    chk_result(fmin.x_minimum(), 1.0, n_iters);
}

TEST_F(FMinimizerTest, GoldenSectionType){
    auto &fmin = fmin_golden;
    size_t n_iters = fmin.set_fn(fsin, 4.0, 0., 6.5).iterate_until();    
    chk_result(fmin.x_minimum(), 3.1415926*1.5, n_iters);
    
    n_iters = fmin.set_fn(fquad, 0., -5., 5.).iterate_until();
    chk_result(fmin.x_minimum(), 1.0, n_iters);
}

TEST_F(FMinimizerTest, QuadGoldenType){
    auto &fmin = fmin_quad_golden;
    size_t n_iters = fmin.set_fn(fsin, 4.0, 0., 6.5).iterate_until();    
    chk_result(fmin.x_minimum(), 3.1415926*1.5, n_iters);
    
    n_iters = fmin.set_fn(fquad, 0., -5., 5.).iterate_until();
    chk_result(fmin.x_minimum(), 1.0, n_iters);
}

TEST_F(FMinimizerTest, ChangeEpsAbs){
    auto &fmin = fmin_default;
    size_t last_n_iters = 0;
    for(auto epsabs: {1., 0.1, 0.01, 1.0e-3, 1.0e-4, 1.0e-5, 1.0e-6}){
        size_t n_iters = fmin.set_fn(fsin, 4.0, 0., 6.5)
            .iterate_until(target_t::DEFAULT_MAX_N_ITERS, epsabs, 0.);
        chk_result(fmin.x_minimum(), 3.141592654*1.5, n_iters, epsabs);
        EXPECT_GE(n_iters, last_n_iters);
        last_n_iters = n_iters;
    }
}

} // namespace
} // namespace HIPP::NUMERICAL