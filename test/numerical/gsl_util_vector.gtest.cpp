#include <hippnumerical.h>
#include <gtest/gtest.h>

namespace HIPP::NUMERICAL {
namespace {

/* GSLVector */
class GSLUtilGSLVectorTest: public ::testing::Test {
protected:
    typedef GSLVector<double> vec_t;

    GSLUtilGSLVectorTest()
    : v11(n1), v12(n1, vec_t::INIT_ZERO), v2(n2), v3(n3), vlin(nlin)
    {
        v11.set_all(1.0);
        v2.set_all(2.0);
        v3.set_all(3.0);
        for(size_t i=0; i<nlin; ++i){
            vlin.set(i, vec_t::value_t(i));
        }
    }
    size_t n1 = 10, n2 = 8, n3 = 12, nlin = 5;
    vec_t v11, v12, v2, v3, vlin;
};

TEST_F(GSLUtilGSLVectorTest, Initialization) {
    EXPECT_EQ(v11.size(), n1);
    EXPECT_EQ(v12.size(), n1);
    EXPECT_EQ(v2.size(), n2);
    EXPECT_EQ(v3.size(), n3);
    EXPECT_EQ(vlin.size(), nlin);

    EXPECT_TRUE(v12.isnull());
    EXPECT_TRUE(v11.ispos());
    EXPECT_TRUE(v2.ispos());
    EXPECT_TRUE(v3.ispos());
    EXPECT_TRUE(vlin.isnonneg());

    v11 = {1., 2., 3., 4., 5.};
    vec_t v = vector<double>{1., 2., 3., 4., 5.};
    EXPECT_EQ(v11.size(), v.size());
    EXPECT_EQ(v11, v);
}

TEST_F(GSLUtilGSLVectorTest, BorrowRef) {
    vec_t v(v11.raw());
    ASSERT_EQ(v.raw(), v11.raw());
    EXPECT_EQ(v.size(), v11.size());
    EXPECT_EQ(v.stride(), v11.stride());
    EXPECT_EQ(v.data(), v11.data());
    EXPECT_TRUE(v == v11);

    size_t n = v.size();
    for(size_t i=0; i<n; ++i){
        EXPECT_EQ( v.get(i), v11.get(i) );
        EXPECT_EQ( v[i], v11[i] );
        EXPECT_EQ( v[i], 1.0 );
    }

    v[0] = 11.0;
    EXPECT_EQ(v11.get(0), 11.0);
    EXPECT_EQ(v.get(0), 11.0);
}

TEST_F(GSLUtilGSLVectorTest, CopyOperation) {
    vec_t v(v11);
    EXPECT_EQ(v.size(), v11.size());
    EXPECT_EQ(v.stride(), v11.stride());
    EXPECT_TRUE(v == v11);

    v = v2;
    EXPECT_EQ(v.size(), v2.size());
    EXPECT_EQ(v.stride(), v2.stride());
    EXPECT_TRUE(v == v2);

    size_t n = v.size();
    for(size_t i=0; i<n; ++i){
        EXPECT_EQ(v.get(i), v2.get(i));
        EXPECT_EQ(v[i], v2[i]);
        EXPECT_EQ(v[i], 2.0);
    }
    v[0] = 11.0;
    EXPECT_EQ(v2.get(0), 2.0);
    EXPECT_EQ(v.get(0), 11.0);
}

TEST_F(GSLUtilGSLVectorTest, Search) {
    typedef vec_t::value_t value_t;
    EXPECT_EQ(vlin.min(), value_t(0));
    EXPECT_EQ(vlin.min_index(), size_t(0));
    EXPECT_EQ(vlin.max(), value_t(nlin-1));
    EXPECT_EQ(vlin.max_index(), size_t(nlin-1));
    
    auto [minval, maxval] = vlin.minmax();
    auto [minid, maxid] = vlin.minmax_index();
    EXPECT_EQ(minval, value_t(0));
    EXPECT_EQ(maxval, value_t(nlin-1));
    EXPECT_EQ(minid, size_t(0));
    EXPECT_EQ(maxid, size_t(nlin-1));
}



} // namespace   
} // namespace HIPP::NUMERICAL

