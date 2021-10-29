#include <hippnumerical.h>
#include <gtest/gtest.h>

namespace HIPP::NUMERICAL {
namespace {

class DArrayIntTest: public ::testing::Test {
protected:
    typedef DArray<int, 3> a3_t;
    typedef DArray<int, 2> a2_t;
    typedef DArray<int, 1> a1_t;
    typedef const DArray<int, 3> c_a3_t;
    typedef typename a3_t::shape_t shape3_t;
    typedef typename a2_t::shape_t shape2_t;
    typedef typename a1_t::shape_t shape1_t;

    DArrayIntTest(){}
    ~DArrayIntTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(DArrayIntTest, DynamicArrayProtocol){
    typedef DynamicArrayTraits<a3_t> tr_t;
    
    bool same_value_t = std::is_same_v<tr_t::value_t, int>;
    EXPECT_TRUE(same_value_t);
    EXPECT_EQ(tr_t::rank, a3_t::RANK);

    a3_t arr1 {{2,3,4}};
    tr_t tr1 {arr1};
    auto ext = tr1.extents(), sd = tr1.strides();
    size_t sz = tr1.size();
    int *buff = tr1.buff();

    ASSERT_EQ(buff, arr1.data());
    ASSERT_EQ(sz, arr1.size());
    
    ASSERT_EQ(ext.size(), a3_t::RANK);
    EXPECT_EQ(ext[0], 2);
    EXPECT_EQ(ext[1], 3);
    EXPECT_EQ(ext[2], 4);
    
    ASSERT_EQ(sd.size(), a3_t::RANK);
    EXPECT_EQ(sd[0], 12);
    EXPECT_EQ(sd[1], 4);
    EXPECT_EQ(sd[2], 1);

    DynamicArrayTraits tr2 {arr1};
    ext = tr2.extents(); 
    sd = tr2.strides();
    sz = tr2.size();
    buff = tr2.buff();

    ASSERT_EQ(buff, arr1.data());
    ASSERT_EQ(sz, arr1.size());
    
    ASSERT_EQ(ext.size(), a3_t::RANK);
    EXPECT_EQ(ext[0], 2);
    EXPECT_EQ(ext[1], 3);
    EXPECT_EQ(ext[2], 4);
    
    ASSERT_EQ(sd.size(), a3_t::RANK);
    EXPECT_EQ(sd[0], 12);
    EXPECT_EQ(sd[1], 4);
    EXPECT_EQ(sd[2], 1);
}

TEST_F(DArrayIntTest, DynamicArrayProtocolConst){
    typedef DynamicArrayTraits<c_a3_t> tr_t;
    
    bool same_value_t = std::is_same_v<tr_t::value_t, const int>;
    EXPECT_TRUE(same_value_t);
    EXPECT_EQ(tr_t::rank, c_a3_t::RANK);

    c_a3_t arr1 {{2,3,4}};
    tr_t tr1 {arr1};
    auto ext = tr1.extents(), sd = tr1.strides();
    size_t sz = tr1.size();
    const int *buff = tr1.buff();

    ASSERT_EQ(buff, arr1.data());
    ASSERT_EQ(sz, arr1.size());
    
    ASSERT_EQ(ext.size(), c_a3_t::RANK);
    EXPECT_EQ(ext[0], 2);
    EXPECT_EQ(ext[1], 3);
    EXPECT_EQ(ext[2], 4);
    
    ASSERT_EQ(sd.size(), c_a3_t::RANK);
    EXPECT_EQ(sd[0], 12);
    EXPECT_EQ(sd[1], 4);
    EXPECT_EQ(sd[2], 1);

    DynamicArrayTraits tr2 {arr1};
    ext = tr2.extents(); 
    sd = tr2.strides();
    sz = tr2.size();
    buff = tr2.buff();

    ASSERT_EQ(buff, arr1.data());
    ASSERT_EQ(sz, arr1.size());
    
    ASSERT_EQ(ext.size(), c_a3_t::RANK);
    EXPECT_EQ(ext[0], 2);
    EXPECT_EQ(ext[1], 3);
    EXPECT_EQ(ext[2], 4);
    
    ASSERT_EQ(sd.size(), c_a3_t::RANK);
    EXPECT_EQ(sd[0], 12);
    EXPECT_EQ(sd[1], 4);
    EXPECT_EQ(sd[2], 1);
}

TEST_F(DArrayIntTest, ContiguousBuffer) {
    typedef ContiguousBufferTraits<a3_t> tr_t;

    a3_t arr1 {{2,3,4}};
    tr_t tr1 {arr1};
    auto [p1, n1] = tr1;
    EXPECT_EQ(p1, arr1.data());
    EXPECT_EQ(n1, arr1.size());

    bool same_value_t = std::is_same_v<tr_t::value_t, int>;
    EXPECT_TRUE(same_value_t);

    ContiguousBufferTraits tr2 {arr1};
    auto [p2, n2] = tr2;
    EXPECT_EQ(p2, arr1.data());
    EXPECT_EQ(n2, arr1.size());

    bool same_type = std::is_same_v<decltype(tr1), decltype(tr2)>;
    EXPECT_TRUE(same_type);

    ContiguousBuffer<int> tr3 {arr1};
    auto [p3, n3] = tr3;
    EXPECT_EQ(p3, arr1.data());
    EXPECT_EQ(n3, arr1.size());

    ContiguousBuffer tr4 {arr1};
    auto [p4, n4] = tr4;
    EXPECT_EQ(p4, arr1.data());
    EXPECT_EQ(n4, arr1.size());

    same_type = std::is_same_v<decltype(tr3), decltype(tr4)>;
    EXPECT_TRUE(same_type);
}

TEST_F(DArrayIntTest, ContiguousBufferConst) {
    typedef ContiguousBufferTraits<c_a3_t> tr_t;

    c_a3_t arr1 {{2,3,4}};
    tr_t tr1 {arr1};
    auto [p1, n1] = tr1;
    EXPECT_EQ(p1, arr1.data());
    EXPECT_EQ(n1, arr1.size());

    bool same_value_t = std::is_same_v<tr_t::value_t, const int>;
    EXPECT_TRUE(same_value_t);

    ContiguousBufferTraits tr2 {arr1};
    auto [p2, n2] = tr2;
    EXPECT_EQ(p2, arr1.data());
    EXPECT_EQ(n2, arr1.size());

    bool same_type = std::is_same_v<decltype(tr1), decltype(tr2)>;
    EXPECT_TRUE(same_type);

    ContiguousBuffer<const int> tr3 {arr1};
    auto [p3, n3] = tr3;
    EXPECT_EQ(p3, arr1.data());
    EXPECT_EQ(n3, arr1.size());

    ContiguousBuffer tr4 {arr1};
    auto [p4, n4] = tr4;
    EXPECT_EQ(p4, arr1.data());
    EXPECT_EQ(n4, arr1.size());

    same_type = std::is_same_v<decltype(tr3), decltype(tr4)>;
    EXPECT_TRUE(same_type);
}

TEST_F(DArrayIntTest, ShapeInitialize){
    a3_t a1 {{2, 2, 3}};

    EXPECT_EQ(a1.size(), 12);

    bool sz_eq = (a1.shape() == shape3_t{2,2,3}).all();
    EXPECT_TRUE(sz_eq) << "a1 shape " << a1.shape() << " but expect {2,2,3}";

    a3_t a2 {{2,3,4}, 1};
    ASSERT_EQ(a2.size(), 24);
    for(size_t i=0; i<a2.size(); ++i){
        EXPECT_EQ(a2[i], 1);
    }
    
    bool val_eq = (a2 == 1).all();
    EXPECT_TRUE(val_eq) << "a2 = " << a2 << " but expect all " << 1;
}

TEST_F(DArrayIntTest, RangeInitialize) {
    vector<int> buff {1,2,3,4};
    
    a3_t a1{{2,2,3}, buff.begin(), buff.end()};
    ASSERT_EQ(a1.size(), 12);
    for(int i=0; i<4; ++i){
        EXPECT_EQ(a1[i], buff[i]);
    }

    a3_t a2{{2,2,3}, {1,2,3,4}};
    ASSERT_EQ(a2.size(), 12);
    for(int i=0; i<4; ++i){
        EXPECT_EQ(a2[i], buff[i]);
    }
}

TEST_F(DArrayIntTest, Copy) {
    a3_t a1 {{2,2,3}, 1};
    a3_t a2 (a1);

    EXPECT_EQ(a2.size(), 12);
    bool sz_eq = (a2.shape() == shape3_t{2,2,3}).all();
    EXPECT_TRUE(sz_eq);
    for(size_t i=0; i<a2.size(); ++i){
        EXPECT_EQ(a2[i], 1);
    }

    a3_t a3;
    a3 = a2;
    EXPECT_EQ(a3.size(), 12);
    sz_eq = (a3.shape() == shape3_t{2,2,3}).all();
    EXPECT_TRUE(sz_eq);
    for(size_t i=0; i<a3.size(); ++i){
        EXPECT_EQ(a3[i], 1);
    }

    // Assign with a constant value.
    for( int val : {-1,0,1,2,3,4,5} ) {
        a3 = val;
        EXPECT_EQ(a3.size(), 12);
        sz_eq = (a3.shape() == shape3_t{2,2,3}).all();
        EXPECT_TRUE(sz_eq);
        for(size_t i=0; i<a3.size(); ++i){
            EXPECT_EQ(a3[i], val);
        }
    }
}

TEST_F(DArrayIntTest, Move) {
    a3_t a1 {{2,2,3}, 1};
    a3_t a2 ( std::move(a1) );

    EXPECT_EQ(a1.size(), 0);
    EXPECT_EQ(a1.data(), nullptr);
    EXPECT_TRUE(a1.empty());

    EXPECT_EQ(a2.size(), 12);
    bool sz_eq = (a2.shape() == shape3_t{2,2,3}).all();
    EXPECT_TRUE(sz_eq);
    for(size_t i=0; i<a2.size(); ++i){
        EXPECT_EQ(a2[i], 1);
    }

    a1 = std::move(a2);
    EXPECT_EQ(a2.size(), 0);
    EXPECT_EQ(a2.data(), nullptr);
    EXPECT_TRUE(a2.empty());

    EXPECT_EQ(a1.size(), 12);
    sz_eq = (a1.shape() == shape3_t{2,2,3}).all();
    EXPECT_TRUE(sz_eq);
    for(size_t i=0; i<a1.size(); ++i){
        EXPECT_EQ(a1[i], 1);
    }
}

TEST_F(DArrayIntTest, Swap) {
    a3_t a1 {{2,2,3}, 1}, a2, a3(a1), a4(a2);

    swap(a3, a4);
    ASSERT_EQ( a3.size(), a2.size() );
    ASSERT_EQ( a4.size(), a1.size() );
    EXPECT_TRUE( (a3==a2).all() );
    EXPECT_TRUE( (a4==a1).all() );

    swap(a1, a2);
    ASSERT_EQ( a3.size(), a1.size() );
    ASSERT_EQ( a4.size(), a2.size() );
    EXPECT_TRUE( (a3==a1).all() );
    EXPECT_TRUE( (a4==a2).all() );
}

TEST_F(DArrayIntTest, ReleaseAndReset) {
    a3_t a1 {{2,2,3}, {1,2,3,4,5,6,7,8,9,10,11,12}};
    size_t n_elem = a1.size();
    int *p1 = new int [n_elem], *p2 = a1.release();
    std::fill_n(p1, n_elem, 1);

    ASSERT_TRUE(a1.empty());
    ASSERT_EQ(n_elem, 12);
    for(size_t i=0; i<n_elem; ++i){
        EXPECT_EQ(p1[i], 1);
        EXPECT_EQ(p2[i], i+1);
    }

    a1.reset(p1, {2,2,3});
    ASSERT_EQ(a1.size(), n_elem);
    for(size_t i=0; i<n_elem; ++i){
        EXPECT_EQ(a1[i], 1);
    }

    delete [] p2;
}

TEST_F(DArrayIntTest, STLLikeAPI) {
    a3_t a1 {{2,2,3}, {1,2,3,4,5,6,7,8,9,10,11,12}}, a2;

    const auto *pa1 = a1.data();
    ASSERT_EQ(a1.size(), 12);
    for(size_t i=0; i<a1.size(); ++i){
        EXPECT_EQ(a1[i], pa1[i]);
        EXPECT_EQ(a1.at(i), pa1[i]);
    }

    EXPECT_FALSE(a1.empty());
    EXPECT_TRUE(a2.empty());
    ASSERT_EQ(a2.size(), 0);

    bool has_raise=false;
    try {
        a1.at(12);
    } catch (const ErrLogic &e) {
        has_raise = true;
    }
    EXPECT_TRUE(has_raise);

    auto b = a1.begin(), e = a1.end();
    EXPECT_EQ(b+a1.size(), e);
    for(size_t i=0; i<a1.size(); ++i){
        auto x = *b++, y = a1[i];
        EXPECT_EQ(x, y);
    }
    for(auto p=b; p!=e; ++p){
        auto x = *p, y = a1[p-b];
        EXPECT_EQ(x, y);
    }

    auto cb = a1.cbegin(), ce = a1.cend();
    EXPECT_EQ(cb+a1.size(), ce);
    for(size_t i=0; i<a1.size(); ++i){
        *cb++ == a1[i];
    }
    for(auto p=cb; p!=ce; ++p){
        *p == a1[p-cb];
    }

    EXPECT_EQ(a1(0,0,0), 1);
    EXPECT_EQ(a1(0,0,1), 2);
    EXPECT_EQ(a1(0,0,2), 3);
    EXPECT_EQ(a1(1,1,0), 10);
    EXPECT_EQ(a1(1,1,1), 11);
    EXPECT_EQ(a1(1,1,2), 12);
}

TEST_F(DArrayIntTest, ReshapeAndConversion) {
    a3_t a1 {{2,2,3}, {1,2,3,4,5,6,7,8,9,10,11,12}},
        a2 = a1.reshaped<3>({2,3,2});
    a2_t a3 = a1.reshaped<2>({4,3});
    a1_t a4 = a1.reshaped<1>({a1.size()});

    EXPECT_EQ(a1.size(), a2.size());
    EXPECT_EQ(a1.size(), a3.size());
    EXPECT_EQ(a1.size(), a4.size());
    bool sz_eq2 = (a2.shape() == shape3_t{2,3,2}).all(), 
        sz_eq3 = (a3.shape() == shape2_t{4,3}).all(),
        sz_eq4 = (a4.shape() == shape1_t{12}).all();
    EXPECT_TRUE(sz_eq2) << "a2.shape " << a2.shape();
    EXPECT_TRUE(sz_eq3) << "a3.shape " << a3.shape();
    EXPECT_TRUE(sz_eq4) << "a4.shape " << a4.shape();

    auto v4 = a4.to_vector();
    ASSERT_EQ(v4.size(), a1.size());
    for(size_t i=0; i<a1.size(); ++i){
        EXPECT_EQ(v4[i], a1[i]);
    }
}

TEST_F(DArrayIntTest, ReshapeSize) {
    a3_t a1({2,2,3}, 1);
    a1.resize(12); 
    
    a1.resize(24);
    ASSERT_EQ(a1.size(), 24);
    bool sz_eq = ( a1.shape() == shape3_t{4,2,3} ).all();
    ASSERT_TRUE( sz_eq );
    for(int i=0; i<12; ++i){
        EXPECT_EQ(a1.at(i), 1);
        EXPECT_EQ(a1.at(i+12), 0);
    }

    a1.resize(48, 2);
    ASSERT_EQ(a1.size(), 48);
    sz_eq = ( a1.shape() == shape3_t{8,2,3} ).all();
    ASSERT_TRUE( sz_eq );

    for(int i=0; i<12; ++i){
        EXPECT_EQ(a1.at(i), 1);
        EXPECT_EQ(a1.at(i+12), 0);
        EXPECT_EQ(a1.at(i+24), 2);
        EXPECT_EQ(a1.at(i+36), 2);
    }

    a1.resize({8,2,3});
    a1.resize({2,2,3});
    a1.resize({4,2,3}, 2);
    ASSERT_EQ(a1.size(), 24);
    sz_eq = ( a1.shape() == shape3_t{4,2,3} ).all();
    ASSERT_TRUE( sz_eq );
    for(int i=0; i<12; ++i){
        EXPECT_EQ(a1.at(i), 1);
        EXPECT_EQ(a1.at(i+12), 2);
    }
}

TEST_F(DArrayIntTest, Arithmetic) {
    a3_t a1 {{2,2,3}, {-3,-2,-1,0,1,2,3,4,5,6,7,8}};
    a1 += 3;
    for(size_t i=0; i<a1.size(); ++i){
        EXPECT_EQ(a1[i], i);
    }
    a1 *= 2;
    for(size_t i=0; i<a1.size(); ++i){
        EXPECT_EQ(a1[i], 2*i);
    }
    a1 /= 2;
    for(size_t i=0; i<a1.size(); ++i){
        EXPECT_EQ(a1[i], i);
    }
    a1 -= 2;
    for(size_t i=0; i<a1.size(); ++i){
        EXPECT_EQ(a1[i], int(i)-2);
    }

    auto a2 = a1 + 2, a3 = a1 - 2, a4 = a1 * 2, a5 = a1 / 2, 
        a6 = 2 + a1, a7 = 2 - a1, a8 = 2*a1, a9 = 2/(a1+10),
        a10 = +a1, a11 = -a1, a12 = ~a1;
    ASSERT_EQ(a2.size(), a1.size());
    ASSERT_EQ(a3.size(), a1.size());
    ASSERT_EQ(a4.size(), a1.size());
    ASSERT_EQ(a5.size(), a1.size());
    ASSERT_EQ(a6.size(), a1.size());
    ASSERT_EQ(a7.size(), a1.size());
    ASSERT_EQ(a8.size(), a1.size());
    ASSERT_EQ(a9.size(), a1.size());
    ASSERT_EQ(a10.size(), a1.size());
    ASSERT_EQ(a11.size(), a1.size());
    ASSERT_EQ(a12.size(), a1.size());
    for(int i=0, n=a1.size(); i<n; ++i){
        EXPECT_EQ(a2[i], i);
        EXPECT_EQ(a3[i], a1[i]-2);
        EXPECT_EQ(a4[i], a1[i]*2);
        EXPECT_EQ(a5[i], a1[i]/2);
        EXPECT_EQ(a6[i], i);
        EXPECT_EQ(a7[i], 2-a1[i]);
        EXPECT_EQ(a8[i], 2*a1[i]);
        EXPECT_EQ(a9[i], 2/(a1[i]+10));
        EXPECT_EQ(a10[i], a1[i]);
        EXPECT_EQ(a11[i], -a1[i]);
        EXPECT_EQ(a12[i], ~a1[i]);
    }

    bool sz_eq = (a2.shape() == shape3_t{2,2,3}).all();
    ASSERT_TRUE(sz_eq) << "a2.shape " << a2.shape();
    EXPECT_EQ(a2(0,0,0), 0);
    EXPECT_EQ(a2(1,1,2), 11);
}

TEST_F(DArrayIntTest, Reduction) {
    a3_t a1 {{2,2,3}, {-3,-2,-1,0,1,2,3,4,5,6,7,8}};

    EXPECT_EQ(a1.sum(), (-3+8)*6);
    EXPECT_EQ(a1.prod(), 0);
    double mean = a1.mean<double>(),
        diff = std::abs(mean - a1.sum<double>()/a1.size());
    EXPECT_LT(diff, 1.0e-4);

    EXPECT_EQ(a1.min(), -3);
    EXPECT_EQ(a1.max(), 8);
    EXPECT_EQ(a1.minmax().first, -3);
    EXPECT_EQ(a1.minmax().second, 8);
    EXPECT_EQ(a1.min_index(), 0);
    EXPECT_EQ(a1.max_index(), 11);
    EXPECT_EQ(a1.minmax_index().first, 0);
    EXPECT_EQ(a1.minmax_index().second, 11);

    EXPECT_FALSE(a1.all());
    EXPECT_TRUE((a1+4).all());
    EXPECT_TRUE(a1.any());
    EXPECT_FALSE((a1*0).any());
}

TEST_F(DArrayIntTest, MapAndVisit) {
    a3_t a1 {{2,2,3}, {-3,-2,-1,0,1,2,3,4,5,6,7,8}}, a2 = a1;
    a2.map([](int x)->int {return x+3; });
    auto a3 = a1.mapped([](int x)->int{return x+3;});

    EXPECT_TRUE( (a2 == a3).all() );
    ASSERT_EQ(a2.size(), a1.size());
    ASSERT_EQ(a3.size(), a1.size());
    for(size_t i=0; i<a2.size(); ++i){
        EXPECT_EQ(a2[i], a1[i]+3);
    }

    a2.visit([](size_t i, int &x){ x -= 3; });
    EXPECT_TRUE( (a2 == a1).all() );
}

TEST_F(DArrayIntTest, MathematicalFunction) {
    a3_t a1 {{2,2,3}, {-3,-2,-1,0,1,2,3,4,5,6,7,8}},
        a2 = a1.floor(), a3 = a1.ceil(), a4 = a1.trunc();

    EXPECT_TRUE( (a1==a2).all() );
    EXPECT_TRUE( (a1==a3).all() );
    EXPECT_TRUE( (a1==a4).all() );
}

} // namespace
} // namespace HIPP::NUMERICAL


