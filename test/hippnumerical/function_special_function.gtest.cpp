#include <hippnumerical.h>
#include <gtest/gtest.h>
#include <limits>
namespace HIPP::NUMERICAL {
namespace {

class SpecialFunctionTest : public ::testing::Test {
protected:
    typedef SpecialFunction sp_t;
    typedef sp_t::value_t value_t;

    SpecialFunctionTest()
    : max_val( std::numeric_limits<value_t>::max() )
    {
        ErrGSL::set_error_handler();
    }
    ~SpecialFunctionTest() override {}

    void is_finite(value_t x) const {
        ASSERT_LT(x, max_val);
    }
    void is_positive(value_t x) const {
        ASSERT_GT(x, 0.);
    }

    value_t max_val;
};

TEST_F(SpecialFunctionTest, AiryFunction) {
    typedef sp_t::Airy fs;
    for(auto x: {1., 2., 3., 4., 5.}){
        is_finite(fs::Ai(x));
        is_finite(fs::Bi(x));
        is_finite(fs::Ai_scaled(x));
        is_finite(fs::Bi_scaled(x));
        is_finite(fs::Ai_deriv(x));
        is_finite(fs::Bi_deriv(x));
        is_finite(fs::Ai_deriv_scaled(x));
        is_finite(fs::Bi_deriv_scaled(x));
    }
    for(auto s: {1, 2, 3, 4, 5}){
        is_finite(fs::zero_Ai(s));
        is_finite(fs::zero_Bi(s));
        is_finite(fs::zero_Ai_deriv(s));
        is_finite(fs::zero_Bi_deriv(s));
    }
    
}

} // namespace
} // namespace HIPP::NUMERICAL