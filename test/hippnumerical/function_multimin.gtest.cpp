#include <hippnumerical.h>
#include <gtest/gtest.h>

namespace HIPP::NUMERICAL {
namespace {

/* FMinimizer */
class FMinimizerNDTest: public ::testing::Test {
protected:
    typedef FMinimizerND target_t;
    typedef target_t::vec_t vec_t;

    FMinimizerNDTest()
    :x_ans(2), fmin_default(2), 
    fmin_nmsimplex(2, target_t::NMSIMPLEX), 
    fmin_nmsimplex2rand(2, target_t::NMSIMPLEX2_RAND)
    {}
    ~FMinimizerNDTest() override {}

    void set_fn(double x0, double y0, double z0, double ax, double ay){
        fn = [x0, y0, z0, ax, ay](const vec_t &vx)->double {
            double x = vx[0], y = vx[1];
            return ax * (x-x0)*(x-x0) + ay * (y-y0)*(y-y0) + z0;
        };
        x_ans = {x0, y0};
    }
    void chk_result(size_t n_iters, const vec_t &ans){
        EXPECT_LT(n_iters, target_t::DEFAULT_MAX_N_ITERS);
        EXPECT_LT(std::fabs(ans[0]-x_ans[0]), 1.0e-2);
        EXPECT_LT(std::fabs(ans[1]-x_ans[1]), 1.0e-2);
    }
    
    std::function<double(const vec_t &)> fn;
    vec_t x_ans;
    target_t fmin_default, fmin_nmsimplex, fmin_nmsimplex2rand;
};

TEST_F(FMinimizerNDTest, DefaultType){
    auto &fmin = fmin_default;
    
    set_fn(5., 10., 2., 3., 4.);
    size_t n_iters = fmin.set_fn(fn, {0., 5.}, 1.).iterate_until();
    chk_result(n_iters, fmin.x_minimum());
}

TEST_F(FMinimizerNDTest, NMSimplexType){
    auto &fmin = fmin_nmsimplex;

    set_fn(5., 10., 2., 3., 4.);
    size_t n_iters = fmin.set_fn(fn, {0., 5.}, 1.).iterate_until();
    chk_result(n_iters, fmin.x_minimum());
}

TEST_F(FMinimizerNDTest, NMSimplex2RandType){
    auto &fmin = fmin_nmsimplex2rand;
    set_fn(5., 10., 2., 3., 4.);

    size_t n_iters = fmin.set_fn(fn, {0., 5.}, 1.).iterate_until();
    chk_result(n_iters, fmin.x_minimum());
}

TEST_F(FMinimizerNDTest, ChangeTargetFunction){
    auto &fmin = fmin_default;
    
    set_fn(5., 10., 2., 3., 4.);
    size_t n_iters = fmin.set_fn(fn, {1., 2.}, 1.).iterate_until();
    chk_result(n_iters, fmin.x_minimum());

    set_fn(50., -30., 5., 10., 20.);
    n_iters = fmin.set_fn(fn, {1., 2.}, 1.).iterate_until();
    chk_result(n_iters, fmin.x_minimum());
}

TEST_F(FMinimizerNDTest, ChangeType){
    auto &fmin = fmin_default;

    set_fn(5., 10., 2., 3., 4.);
    for(auto t: {target_t::NMSIMPLEX, 
        target_t::NMSIMPLEX2, target_t::NMSIMPLEX2_RAND}){
        size_t n_iters = fmin.set_type(t)
            .set_fn(fn, {1., 2.}, 1.).iterate_until();
        chk_result(n_iters, fmin.x_minimum());
    }
}

} // namespace
} // namespace HIPP::NUMERICAL