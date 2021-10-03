#include <hippnumerical.h>
#include <gtest/gtest.h>

namespace HIPP::NUMERICAL {
namespace {

class GaussianRandomNumberTest : public ::testing::Test {
protected:
    typedef decltype(randn) rng_t;
    typedef rng_t::result_t result_t;

    GaussianRandomNumberTest() : min(randn.min()), max(randn.max()) {}

    void assert_bound(result_t value) {
        ASSERT_GE(value, min);
        ASSERT_LE(value, max);
    }

    void assert_bound(result_t value, result_t _min, result_t _max) {
        ASSERT_GE(value, _min);
        ASSERT_LE(value, _max);
    }

    result_t min, max;
};

TEST_F(GaussianRandomNumberTest, OneNumber) {
    result_t x1 = randn(), x2 = randn(), x3 = randn();
    assert_bound(x1);
    assert_bound(x2);
    assert_bound(x3);
}

TEST_F(GaussianRandomNumberTest, OneNumberNonDefaultParam) {
    randn.reset_param(3., 10.);
    auto [_min, _max] = randn.range();
    assert_bound(randn(), _min, _max);
    assert_bound(randn(), _min, _max);
    assert_bound(randn(), _min, _max);
}

TEST_F(GaussianRandomNumberTest, ResetState) {
    randn.seed(0).reset_state();
    auto r0 = randn(), r1 = randn();
    randn.seed(0).reset_state();
    auto r2 = randn(), r3 = randn();
    EXPECT_EQ(r0, r2);
    EXPECT_EQ(r1, r3);
}

class PoissonRandomNumberTest: public ::testing::Test {
protected:
    typedef PoissonRandomNumber<> rng_t;
    typedef rng_t::result_t result_t;

    rng_t rng {};
    rng_t rng_1 {1.0};

    PoissonRandomNumberTest(){}
    ~PoissonRandomNumberTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(PoissonRandomNumberTest, GetRandomNumbers) {
    auto x1 = rng();

    EXPECT_LE(x1, rng.max());
    EXPECT_GE(x1, rng.min());

    rng_t rng_2;
    for(int i=0; i<100; ++i){
        EXPECT_LE(rng(), rng.max());
        EXPECT_GE(rng(), rng.min());
        EXPECT_LE(rng_1(), rng.max());
        EXPECT_GE(rng_1(), rng.min());

        rng_2.reset_param(i+1.0);
        EXPECT_LE(rng_2(), rng.max());
        EXPECT_GE(rng_2(), rng.min());
    }

    auto vx = rng(100);

    ASSERT_EQ(vx.size(), 100);
    for(size_t i=0; i<vx.size(); ++i){
        EXPECT_LE(vx[i], rng.max());
        EXPECT_GE(vx[i], rng.min());
    }
}

TEST_F(PoissonRandomNumberTest, GetSetParameters) {
    result_t min = rng.min(), max = rng.max();
    auto rg = rng.range();
    EXPECT_EQ(rg.first, min);
    EXPECT_EQ(rg.second, max);

    auto param = rng.param();
    auto mean1 = rng.mean(), mean2 = param.mean();
    EXPECT_EQ(mean1, mean2);
}

TEST_F(PoissonRandomNumberTest, ResetState) {
    rng.seed(0).reset_state();
    auto r0 = rng(), r1 = rng();
    rng.seed(0).reset_state();
    auto r2 = rng(), r3 = rng();
    EXPECT_EQ(r0, r2);
    EXPECT_EQ(r1, r3);
}

class ExponentialRandomNumberTest: public ::testing::Test {
protected:
    typedef ExponentialRandomNumber<> rng_t;
    typedef rng_t::result_t result_t;

    rng_t rng {};
    rng_t rng_1 {1.0};

    ExponentialRandomNumberTest(){}
    ~ExponentialRandomNumberTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ExponentialRandomNumberTest, GetRandomNumbers) {
    auto x1 = rng();

    EXPECT_LE(x1, rng.max());
    EXPECT_GE(x1, rng.min());

    rng_t rng_2;
    for(int i=0; i<100; ++i){
        EXPECT_LE(rng(), rng.max());
        EXPECT_GE(rng(), rng.min());
        EXPECT_LE(rng_1(), rng.max());
        EXPECT_GE(rng_1(), rng.min());

        rng_2.reset_param(i+1.0);
        EXPECT_LE(rng_2(), rng.max());
        EXPECT_GE(rng_2(), rng.min());
    }

    auto vx = rng(100);

    ASSERT_EQ(vx.size(), 100);
    for(size_t i=0; i<vx.size(); ++i){
        EXPECT_LE(vx[i], rng.max());
        EXPECT_GE(vx[i], rng.min());
    }
}

TEST_F(ExponentialRandomNumberTest, GetSetParameters) {
    result_t min = rng.min(), max = rng.max();
    auto rg = rng.range();
    EXPECT_EQ(rg.first, min);
    EXPECT_EQ(rg.second, max);

    auto param = rng.param();
    auto lambda1 = rng.lambda(), lambda2 = param.lambda();
    EXPECT_EQ(lambda1, lambda2);
}

TEST_F(ExponentialRandomNumberTest, ResetState) {
    rng.seed(0).reset_state();
    auto r0 = rng(), r1 = rng();
    rng.seed(0).reset_state();
    auto r2 = rng(), r3 = rng();
    EXPECT_EQ(r0, r2);
    EXPECT_EQ(r1, r3);
}

class GammaRandomNumberTest: public ::testing::Test {
protected:
    typedef GammaRandomNumber<> rng_t;
    typedef rng_t::result_t result_t;

    rng_t rng {};
    rng_t rng_1 {2.0, 1.0};

    GammaRandomNumberTest(){}
    ~GammaRandomNumberTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(GammaRandomNumberTest, GetRandomNumbers) {
    auto x1 = rng();

    EXPECT_LE(x1, rng.max());
    EXPECT_GE(x1, rng.min());

    rng_t rng_2;
    for(int i=0; i<100; ++i){
        EXPECT_LE(rng(), rng.max());
        EXPECT_GE(rng(), rng.min());
        EXPECT_LE(rng_1(), rng.max());
        EXPECT_GE(rng_1(), rng.min());

        rng_2.reset_param(i+1.0, i+1.0);
        EXPECT_LE(rng_2(), rng.max());
        EXPECT_GE(rng_2(), rng.min());
    }

    auto vx = rng(100);

    ASSERT_EQ(vx.size(), 100);
    for(size_t i=0; i<vx.size(); ++i){
        EXPECT_LE(vx[i], rng.max());
        EXPECT_GE(vx[i], rng.min());
    }
}

TEST_F(GammaRandomNumberTest, GetSetParameters) {
    result_t min = rng.min(), max = rng.max();
    auto rg = rng.range();
    EXPECT_EQ(rg.first, min);
    EXPECT_EQ(rg.second, max);

    auto param = rng.param();
    auto alpha1 = rng.alpha(), alpha2 = param.alpha();
    EXPECT_EQ(alpha1, alpha2);
    auto beta1 = rng.beta(), beta2 = param.beta();
    EXPECT_EQ(beta1, beta2);
}

TEST_F(GammaRandomNumberTest, ResetState) {
    rng.seed(0).reset_state();
    auto r0 = rng(), r1 = rng();
    rng.seed(0).reset_state();
    auto r2 = rng(), r3 = rng();
    EXPECT_EQ(r0, r2);
    EXPECT_EQ(r1, r3);
}

class WeibullRandomNumberTest: public ::testing::Test {
protected:
    typedef WeibullRandomNumber<> rng_t;
    typedef rng_t::result_t result_t;

    rng_t rng {};
    rng_t rng_1 {2.0, 1.0};

    WeibullRandomNumberTest(){}
    ~WeibullRandomNumberTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(WeibullRandomNumberTest, GetRandomNumbers) {
    auto x1 = rng();

    EXPECT_LE(x1, rng.max());
    EXPECT_GE(x1, rng.min());

    rng_t rng_2;
    for(int i=0; i<100; ++i){
        EXPECT_LE(rng(), rng.max());
        EXPECT_GE(rng(), rng.min());
        EXPECT_LE(rng_1(), rng.max());
        EXPECT_GE(rng_1(), rng.min());

        rng_2.reset_param(i+1.0, i+1.0);
        EXPECT_LE(rng_2(), rng.max());
        EXPECT_GE(rng_2(), rng.min());
    }

    auto vx = rng(100);

    ASSERT_EQ(vx.size(), 100);
    for(size_t i=0; i<vx.size(); ++i){
        EXPECT_LE(vx[i], rng.max());
        EXPECT_GE(vx[i], rng.min());
    }
}

TEST_F(WeibullRandomNumberTest, GetSetParameters) {
    result_t min = rng.min(), max = rng.max();
    auto rg = rng.range();
    EXPECT_EQ(rg.first, min);
    EXPECT_EQ(rg.second, max);

    auto param = rng.param();
    auto a1 = rng.a(), a2 = param.a();
    EXPECT_EQ(a1, a2);
    auto b1 = rng.b(), b2 = param.b();
    EXPECT_EQ(b1, b2);
}

TEST_F(WeibullRandomNumberTest, ResetState) {
    rng.seed(0).reset_state();
    auto r0 = rng(), r1 = rng();
    rng.seed(0).reset_state();
    auto r2 = rng(), r3 = rng();
    EXPECT_EQ(r0, r2);
    EXPECT_EQ(r1, r3);
}

class ExtremeValueRandomNumberTest: public ::testing::Test {
protected:
    typedef ExtremeValueRandomNumber<> rng_t;
    typedef rng_t::result_t result_t;

    rng_t rng {};
    rng_t rng_1 {2.0, 1.0};

    ExtremeValueRandomNumberTest(){}
    ~ExtremeValueRandomNumberTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(ExtremeValueRandomNumberTest, GetRandomNumbers) {
    auto x1 = rng();

    EXPECT_LE(x1, rng.max());
    EXPECT_GE(x1, rng.min());

    rng_t rng_2;
    for(int i=0; i<100; ++i){
        EXPECT_LE(rng(), rng.max());
        EXPECT_GE(rng(), rng.min());
        EXPECT_LE(rng_1(), rng.max());
        EXPECT_GE(rng_1(), rng.min());

        rng_2.reset_param(i+1.0, i+1.0);
        EXPECT_LE(rng_2(), rng.max());
        EXPECT_GE(rng_2(), rng.min());
    }

    auto vx = rng(100);

    ASSERT_EQ(vx.size(), 100);
    for(size_t i=0; i<vx.size(); ++i){
        EXPECT_LE(vx[i], rng.max());
        EXPECT_GE(vx[i], rng.min());
    }
}

TEST_F(ExtremeValueRandomNumberTest, GetSetParameters) {
    result_t min = rng.min(), max = rng.max();
    auto rg = rng.range();
    EXPECT_EQ(rg.first, min);
    EXPECT_EQ(rg.second, max);

    auto param = rng.param();
    auto a1 = rng.a(), a2 = param.a();
    EXPECT_EQ(a1, a2);
    auto b1 = rng.b(), b2 = param.b();
    EXPECT_EQ(b1, b2);
}

TEST_F(ExtremeValueRandomNumberTest, ResetState) {
    rng.seed(0).reset_state();
    auto r0 = rng(), r1 = rng();
    rng.seed(0).reset_state();
    auto r2 = rng(), r3 = rng();
    EXPECT_EQ(r0, r2);
    EXPECT_EQ(r1, r3);
}


} // namespace   
} // namespace HIPP::NUMERICAL

