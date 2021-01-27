#include <hippnumerical.h>
#include <gtest/gtest.h>

namespace HIPP::NUMERICAL {
namespace {

/* GSLMultiminFn */
class GSLUtilGSLMultiminFnTest: public ::testing::Test {
protected:
    typedef GSLMultiminFn target_t;
    typedef target_t::user_fn_t user_fn_t;
    typedef target_t::vec_t vec_t;
    typedef target_t::value_t value_t;

    GSLUtilGSLMultiminFnTest(){}
    ~GSLUtilGSLMultiminFnTest() override {}

    vec_t v1 = {1.,2.}, v2 = {2.,3.}, v3 = {-2., -3.}, 
        v4 = {1., 2., 3., 4., 5.};

    static value_t sin2(const vec_t &x){
        return std::sin(x[0]) * std::sin(2.0*x[1]);
    }
    target_t fsin2 {2, sin2};

    static value_t sum(const vec_t &x){
        size_t n = x.size();
        value_t res = 0.;
        for(size_t i=0; i<n; ++i){
            res += x[i];
        }
        return 0.;
    }
    user_fn_t lamsum = [](const vec_t &x)->value_t {
        return sum(x);
    };
    target_t fsum {5, lamsum};
};

TEST_F(GSLUtilGSLMultiminFnTest, ValueEvaluation){
    EXPECT_EQ(fsin2(v1), sin2(v1));
    EXPECT_EQ(fsin2(v2), sin2(v2));
    EXPECT_EQ(fsin2(v3), sin2(v3));

    EXPECT_EQ(fsum(v4), sum(v4));
}

TEST_F(GSLUtilGSLMultiminFnTest, CopyOperation){
    auto fcopy_sin2 = fsin2;
    EXPECT_EQ(fcopy_sin2(v1), sin2(v1));
    EXPECT_EQ(fcopy_sin2(v2), sin2(v2));
    EXPECT_EQ(fcopy_sin2(v3), sin2(v3));
    EXPECT_EQ(fcopy_sin2.dim(), fsin2.dim());

    auto fcopy_sum = fsum;
    EXPECT_EQ(fcopy_sum(v4), sum(v4));
    EXPECT_EQ(fcopy_sum.dim(), fsum.dim());
}

TEST_F(GSLUtilGSLMultiminFnTest, MoveOperation){
    auto fmv_sin2 = std::move(fsin2);
    EXPECT_EQ(fmv_sin2(v1), sin2(v1));
    EXPECT_EQ(fmv_sin2(v2), sin2(v2));
    EXPECT_EQ(fmv_sin2(v3), sin2(v3));
    EXPECT_EQ(fmv_sin2.dim(), fsin2.dim());

    auto fmv_sum = std::move(fsum);
    EXPECT_EQ(fmv_sum(v4), sum(v4));
    EXPECT_EQ(fmv_sum.dim(), fsum.dim());
}

TEST_F(GSLUtilGSLMultiminFnTest, PropertyQeury){
    target_t f1, f2;
    EXPECT_EQ(f1.empty(), true);
    EXPECT_EQ(f2.empty(), true);

    f1 = fsum;
    EXPECT_EQ(bool(f1), true);

    f2.set_fn(2, sin2);
    EXPECT_EQ(f2.empty(), false);
}

} // namespace
} // namespace HIPP::NUMERICAL