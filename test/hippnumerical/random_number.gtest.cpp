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


} // namespace   
} // namespace HIPP::NUMERICAL

