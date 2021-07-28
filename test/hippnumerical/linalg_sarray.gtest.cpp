#include <hippnumerical.h>
#include <gtest/gtest.h>

namespace HIPP::NUMERICAL {
namespace {

class SArrayIntTest: public ::testing::Test {
protected:
    typedef SArray<int, 2, 2, 3> arr_t;
    typedef SArray<double, 2, 2, 3> farr_t;
    typedef SArray<bool, 2, 2, 3> barr_t;
    typedef SArray<int, 2, 3> sub_arr_t;
    typedef SVec3i sub_vec_t;

    void chk_eq(const arr_t &x, const arr_t &y) {
        for(size_t i=0; i<arr_t::SIZE; ++i) {
            EXPECT_EQ(x[i], y[i]);
            EXPECT_EQ(x[i]-y[i], 0);
        }
    }
    void chk_eq(const barr_t &x, const barr_t &y) {
        for(size_t i=0; i<barr_t::SIZE; ++i) {
            EXPECT_EQ(x[i], y[i]);
        }
    }
    void chk_eq(const farr_t &x, const farr_t &y, double eps=1.0e-4) {
        for(size_t i=0; i<farr_t::SIZE; ++i) {
            chk_eq(x[i], y[i], eps);
        }
    }
    void chk_eq(const sub_vec_t &v1, const sub_vec_t &v2) {
        for(size_t i=0; i<v1.size(); ++i){
            EXPECT_EQ(v1[i], v2[i]);
        }
        auto all_same = (v1 == v2).all();
        EXPECT_TRUE(all_same);
    }
    void chk_eq(const sub_arr_t &v1, const sub_arr_t &v2) {
        for(size_t i=0; i<v1.size(); ++i){
            EXPECT_EQ(v1[i], v2[i]);
        }
    }
    void chk_eq(double x, double y, double eps=1.0e-4) {
        ASSERT_LT(std::abs(x-y), eps);
    }
    template<typename T1, typename T2>
    void chk_type_eq(const SArray<T1, 2, 2, 3> &x, const T2 &y) {
        bool match = std::is_same_v<T1, T2>;
        EXPECT_TRUE(match);
    }
    template<typename T1, typename T2>
    void chk_type_eq(const T1 &x, const T2 &y) {
        bool match = std::is_same_v<T1,T2>;
        EXPECT_TRUE(match);
    }
    template<typename T1, typename T2>
    void chk_type_eq() {
        bool match = std::is_same_v<T1,T2>;
        EXPECT_TRUE(match);
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

TEST_F(SArrayIntTest, BoolView) {
    arr_t a {
        -3,-2,-1,
        0,1,2, 
        3,4,5,
        6,7,8 };
    a[a>=0] += 2;
    chk_eq(a, {-3, -2, -1,
        2,3,4,
        5,6,7,
        8,9,10});
}

TEST_F(SArrayIntTest, StrideView){
    arr_t a {
        -3,-2,-1,
        0,1,2, 
        3,4,5,
        6,7,8 };
    a.view(a.s_stride, a.s_all, a.s_one(1), a.s_one(1)) += 1;
    chk_eq(a, {
        -3,-2,-1,
        0,2,2, 
        3,4,5,
        6,8,8
    });
    a.view(a.s_stride, a.s_range(1,2), 
        a.s_one(1), a.s_range(1,3)) *= 2;
    chk_eq(a, {
        -3,-2,-1,
        0,2,2, 
        3,4,5,
        6,16,16
    });
}

TEST_F(SArrayIntTest, TupleAPI){
    // 2x2x3
    arr_t a {
        -3,-2,-1,
        0,1,2, 
        3,4,5,
        6,7,8 
    };
    const auto & cref_a = a;

    chk_type_eq<decltype(get<0>(a)), sub_arr_t &>();
    chk_type_eq<decltype(get<0>( cref_a )), const sub_arr_t &>();
    chk_type_eq<decltype(get<0>( std::move(a) )), sub_arr_t &&>();

    chk_type_eq<decltype(get<0>(get<0>(a))), sub_vec_t&>();
    chk_type_eq<decltype(get<0>(get<0>(cref_a))), const sub_vec_t&>();
    chk_type_eq<decltype(get<0>(std::move(get<0>(a)) )), sub_vec_t&&>();

    chk_eq(get<0>(get<0>(a)), sub_vec_t{-3,-2,-1});
    chk_eq(get<1>(get<0>(a)), sub_vec_t{0,1,2});
    chk_eq(get<0>(get<1>(a)), sub_vec_t{3,4,5});
    chk_eq(get<1>(get<1>(a)), sub_vec_t{6,7,8});

    chk_eq(get<0>(a), {-3,-2,-1,0,1,2});
    chk_eq(get<1>(a), {3,4,5,6,7,8});

    auto &[x0,x1] = a;
    auto &[y0,y1] = x0;
    auto &[z0,z1] = x1;

    y0[1] = 2;
    y1[0] = 100;
    EXPECT_EQ(z0.sum(), 12);
    EXPECT_EQ(z1.sum(), 21);
    chk_eq(a, {
        -3,2,-1,
        100,1,2, 
        3,4,5,
        6,7,8 
    });
}

TEST_F(SArrayIntTest, Reduction) {
    arr_t a {
        -3,-2,-1,
        0,1,2, 
        3,4,5,
        6,7,8 
    };
    EXPECT_EQ( (a >= 0).all(), false );
    EXPECT_EQ( (a >= -3).all(), true );
    EXPECT_EQ( (a < 5).all(), false );
    EXPECT_EQ( (a < 9).all(), true );


    EXPECT_EQ( (a[a>=7]).sum(), 15 );
    EXPECT_EQ( (a[a>=7]).prod(), 56 );
}

} // namespace
} // namespace HIPP::NUMERICAL
