#include <hippnumerical.h>
#include <gtest/gtest.h>

namespace HIPP::NUMERICAL {
namespace {

class SVecIntTest: public ::testing::Test {
protected:
    typedef SVec<int, 3> vec_t;
    typedef SVec<double, 3> fvec_t;
    typedef SVec<bool, 3> bvec_t;

    void chk_eq(const vec_t &x, const vec_t &y) {
        for(size_t i=0; i<vec_t::SIZE; ++i) {
            ASSERT_EQ(x[i], y[i]);
            ASSERT_EQ(x[i]-y[i], 0);
        }
        ASSERT_TRUE( (x == y).all() );
        ASSERT_TRUE( !(x != y).any() );
        ASSERT_TRUE( (x - y == 0).all() );
    }
    void chk_eq(const bvec_t &x, const bvec_t &y) {
        for(size_t i=0; i<vec_t::SIZE; ++i) {
            ASSERT_EQ(x[i], y[i]);
        }
        ASSERT_TRUE( (x == y).all() );
        ASSERT_TRUE( !(x != y).any() );
    }
    void chk_eq(const fvec_t &x, const fvec_t &y, double eps=1.0e-4) {
        for(size_t i=0; i<vec_t::SIZE; ++i) {
            chk_eq(x[i], y[i]);
        }
        ASSERT_TRUE(((x-y).abs() < eps).all());
    }
    void chk_eq(double x, double y, double eps=1.0e-4) {
        ASSERT_LT(std::abs(x-y), eps);
    }
    template<typename T1, typename T2>
    void chk_type_eq(const SVec<T1, 3> &x, const T2 &y) {
        bool match = std::is_same_v<T1, T2>;
        ASSERT_TRUE(match);
    }
    template<typename T1, typename T2>
    void chk_type_eq(const T1 &x, const T2 &y) {
        bool match = std::is_same_v<T1,T2>;
        ASSERT_TRUE(match);
    }
};

TEST_F(SVecIntTest, Reduction) {
    vec_t x {0,0,0}, x0 {1,0,0}, x1 {0,2,0}, x2 {0,0,3}, 
        x01 {0,5,10}, x012 {-5,5,10};

    EXPECT_EQ( x.all(), false );
    EXPECT_EQ( x0.all(), false );
    EXPECT_EQ( x1.all(), false );
    EXPECT_EQ( x2.all(), false );
    EXPECT_EQ( x01.all(), false );
    EXPECT_EQ( x012.all(), true );

    EXPECT_EQ( x.any(), false );
    EXPECT_EQ( x0.any(), true );
    EXPECT_EQ( x1.any(), true );
    EXPECT_EQ( x2.any(), true );
    EXPECT_EQ( x01.any(), true );
    EXPECT_EQ( x012.any(), true );

    EXPECT_EQ(x01.mean(), 5);
    auto mean_f = x01.mean<double>();
    chk_type_eq(mean_f, 5.0);
    chk_eq(mean_f, 5.0);
}

TEST_F(SVecIntTest, Map) {
    vec_t v1 {0, 5, 10};

    /** Test with a map */
    auto r1 = v1.mapped([](int x) { return x+1; });
    chk_type_eq(r1, int {});
    chk_eq(r1, {1, 6, 11});

    /** Map to a different type */
    auto r2 = v1.mapped( [](int x)->double { return x+1.0; } );
    chk_type_eq(r2, double {});
    chk_eq(r2, {1., 6., 11.});
}

TEST_F(SVecIntTest, Visit) {
    vec_t x {1,2,3}, x_dest = {1,3,5};
    x.visit([](int i, int &j){j+=i;});
    EXPECT_TRUE( (x==x_dest).all() );
}

TEST_F(SVecIntTest, MathOp) {
    vec_t x {-3, 1, 2};
    chk_type_eq(vec_t::int_value_t {}, int {});
    
    auto xfl = x.floor(), xcl = x.ceil(), xtrunc = x.trunc();
    chk_type_eq(xfl, int {});
    chk_type_eq(xcl, int {});
    chk_type_eq(xtrunc, int {});

    chk_eq(xfl, x);
    chk_eq(xcl, x);
    chk_eq(xtrunc, x);
}

class SVecDoubleTest: public SVecIntTest {
protected:
};

TEST_F(SVecDoubleTest, Cast) {
    fvec_t x {-3.5, 0.1, 1.5};
    vec_t xi (x);
    chk_eq(xi, {-3, 0, 1});

    fvec_t xf(xi);
    chk_eq(xf, {-3.0, 0., 1.});
}

TEST_F(SVecDoubleTest, Compare) {
    fvec_t x {-5., 5., 10.};
    auto cmp1 = x > -2.5, cmp2 = x <= 7.5;
    chk_eq(cmp1, {false, true, true});
    chk_eq(cmp2, {true, true, false});
}

TEST_F(SVecDoubleTest, Map) {
    fvec_t x{0., 5., 10.};
    
    /** Test with a map */
    auto r1 = x.mapped( [](double x)->double {return std::sin(x);} );
    chk_type_eq(r1, double {});
    chk_eq(r1, {std::sin(0.),std::sin(5.),std::sin(10.)});

    auto r2 = x.mapped( [](double x)->int {return x+1.1; } );
    chk_type_eq(r2, int {});
    chk_eq(r2, {1, 6, 11});
}

TEST_F(SVecDoubleTest, Reduction) {
    fvec_t x {0.,0.,0.}, x0 {1.,0.,0.}, x1 {0.,2.,0.}, x2 {0.,0.,3.}, 
        x01 {0.,5.,10.}, x012 {-5.,5.,10.};

    EXPECT_EQ( x.all(), false );
    EXPECT_EQ( x0.all(), false );
    EXPECT_EQ( x1.all(), false );
    EXPECT_EQ( x2.all(), false );
    EXPECT_EQ( x01.all(), false );
    EXPECT_EQ( x012.all(), true );

    EXPECT_EQ( x.any(), false );
    EXPECT_EQ( x0.any(), true );
    EXPECT_EQ( x1.any(), true );
    EXPECT_EQ( x2.any(), true );
    EXPECT_EQ( x01.any(), true );
    EXPECT_EQ( x012.any(), true );


    auto mean_f = x01.mean();
    chk_type_eq(mean_f, double {});
    chk_eq(mean_f, 5.0);
}

TEST_F(SVecDoubleTest, MathOp) {
    chk_type_eq(typename fvec_t::int_value_t {}, int {});

    fvec_t x {-3.5, 0.1, 1.5};
    
    auto xfl = x.floor(), xcl = x.ceil(), xtrunc = x.trunc();
    chk_type_eq(xfl, int {});
    chk_type_eq(xcl, int {});
    chk_type_eq(xtrunc, int {});
    chk_eq(xfl, {-4,0,1});
    chk_eq(xcl, {-3,1,2});
    chk_eq(xtrunc, {-3,0,1});
}

TEST_F(SVecDoubleTest, TupleAPI) {
    fvec_t v {-3., 0., 10.};
    auto [x,y,z] = v;
    auto &[xr,yr,zr] = v;
    auto &&[xrr,yrr,zrr] = fvec_t{-3.,0.,10.};
    chk_eq({x,y,z}, v);
    chk_eq({xr,yr,zr}, v);
    chk_eq({xrr,yrr,zrr}, v);
}

TEST_F(SVecDoubleTest, GridIndex) {
    fvec_t v {-3.1, 0.1, 10.1};
    double grid_sz = 0.5;
    auto ids = (v / grid_sz).floor();
    chk_type_eq(ids, int {});
    chk_eq(ids, {-7, 0, 20});
}
    
class SVecBoolTest: public SVecIntTest {
protected:
};

TEST_F(SVecBoolTest, Logic) {
    bvec_t cmp1 { false, true, true }, 
        cmp2 { true, true, false },
        cmpf(false), cmpt(true), 
        cmp1_neg {true,false,false};
    
    chk_eq(cmpf, {false,false,false});
    chk_eq(cmpt, {true,true,true});
    
    chk_eq(cmp1 & cmp2, {false,true,false});
    chk_eq(cmp1 & cmpf, cmpf);
    chk_eq(cmp1 & cmpt, cmp1);
    chk_eq(cmp1 & false, cmpf);
    chk_eq(cmp1 & true, cmp1);
    chk_eq(false & cmp1, cmpf);
    chk_eq(true & cmp1, cmp1);

    chk_eq(cmp1 | cmp2, cmpt);
    chk_eq(cmp1 | cmpf, cmp1);
    chk_eq(cmp1 | cmpt, cmpt);
    chk_eq(cmp1 | false, cmp1);
    chk_eq(cmp1 | true, cmpt);
    chk_eq(false | cmp1, cmp1);
    chk_eq(true | cmp1, cmpt);

    chk_eq(cmp1 ^ cmp2, {true,false,true});
    chk_eq(cmp1 ^ cmpf, cmp1);
    chk_eq(cmp1 ^ cmpt, cmp1_neg);
    chk_eq(cmp1 ^ false, cmp1);
    chk_eq(cmp1 ^ true, cmp1_neg);
    chk_eq(false ^ cmp1, cmp1);
    chk_eq(true ^ cmp1, cmp1_neg);
}


} // namespace
} // namespace HIPP::NUMERICAL
