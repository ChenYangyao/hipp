#include <hippnumerical.h>
#include <gtest/gtest.h>

namespace HIPP::NUMERICAL {
namespace {

class SArrayIntTest: public ::testing::Test {
protected:
    typedef SArray<int, 2, 2, 3> arr_t;
    typedef SArray<double, 2, 2, 3> farr_t;
    typedef SArray<bool, 2, 2, 3> barr_t;

    void chk_eq(const arr_t &x, const arr_t &y) {
        for(size_t i=0; i<arr_t::SIZE; ++i) {
            ASSERT_EQ(x[i], y[i]);
            ASSERT_EQ(x[i]-y[i], 0);
        }
    }
    void chk_eq(const barr_t &x, const barr_t &y) {
        for(size_t i=0; i<barr_t::SIZE; ++i) {
            ASSERT_EQ(x[i], y[i]);
        }
    }
    void chk_eq(const farr_t &x, const farr_t &y, double eps=1.0e-4) {
        for(size_t i=0; i<farr_t::SIZE; ++i) {
            chk_eq(x[i], y[i], eps);
        }
    }
    void chk_eq(double x, double y, double eps=1.0e-4) {
        ASSERT_LT(std::abs(x-y), eps);
    }
    template<typename T1, typename T2>
    void chk_type_eq(const SArray<T1, 2, 2, 3> &x, const T2 &y) {
        bool match = std::is_same_v<T1, T2>;
        ASSERT_TRUE(match);
    }
    template<typename T1, typename T2>
    void chk_type_eq(const T1 &x, const T2 &y) {
        bool match = std::is_same_v<T1,T2>;
        ASSERT_TRUE(match);
    }
};

TEST_F(SArrayIntTest, Map) {
    arr_t a {
        -3,-2,-1,
        0,1,2, 
        3,4,5,
        6,7,8 };
    auto r1 = a.mapped([](int x){ return x+1; });
    chk_type_eq(r1, int {});
    chk_eq(r1, {-2,-1,
        0,1,2, 
        3,4,5,
        6,7,8,9});

    auto r2 = a.mapped([](int x)->double{ return x+1.0; });
    chk_type_eq(r2, double {});
    chk_eq(r2, {-2.,-1.,
        0.,1.,2., 
        3.,4.,5.,
        6.,7.,8.,9.});
}

TEST_F(SArrayIntTest, Visit) {
    arr_t a {
        -3,-2,-1,
        0,1,2, 
        3,4,5,
        6,7,8 };
    a.visit([](int i, int &j){j+=i;});
    chk_eq(a, {
        -3,-1,1,
        3,5,7, 
        9,11,13,
        15,17,19 });
}

TEST_F(SArrayIntTest, MathOp) {
    arr_t a {
        -3,-2,-1,
        0,1,2, 
        3,4,5,
        6,7,8 };
    chk_type_eq(arr_t::int_value_t {}, int {});

    auto fl = a.floor(), cl = a.ceil(), trc = a.trunc();
    chk_type_eq(fl, int {});
    chk_type_eq(cl, int {});
    chk_type_eq(trc, int {});

    chk_eq(fl, a);
    chk_eq(cl, a);
    chk_eq(trc, a);
}

TEST_F(SArrayIntTest, ArithmeticOp) {
    arr_t a {
        -3,-2,-1,
        0,1,2, 
        3,4,5,
        6,7,8 };
    auto aplus = a+2, aminus = a-2, aprod = a*2, adiv = a/2;
    chk_type_eq(aplus, int {});
    chk_type_eq(aminus, int {});
    chk_type_eq(aprod, int {});
    chk_type_eq(adiv, int {});
    chk_eq(aplus, {-1,
        0,1,2, 
        3,4,5,
        6,7,8,9,10});
    chk_eq(aminus, {-5, -4, -3, -2, -1,
        0,1,2, 
        3,4,5,
        6});
    chk_eq(aprod, {-6, -4, -2,
        0,2,4, 
        6,8,10,
        12,14,16});
    chk_eq(adiv, {-1, -1, 0, 
        0, 0, 1, 
        1, 2, 2, 
        3, 3, 4});
    a += 2;
    chk_eq(a, aplus);
    a -= 2;
    chk_eq(a, aplus-2);
    a *= 2;
    chk_eq(a, aprod);
    a /= 2;
    chk_eq(a, aprod/2);
}

} // namespace
} // namespace HIPP::NUMERICAL
