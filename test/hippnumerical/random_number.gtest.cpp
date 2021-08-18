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

} // namespace   
} // namespace HIPP::NUMERICAL

