#include <hippcntl.h>
#include <gtest/gtest.h>
namespace HIPP {
namespace {

class ConceptRawArrayTest : public ::testing::Test {
protected:
    typedef RawArrayHelper helper_t;
    template<typename T>
    using traits_t = RawArrayTraits<T>;

    template<size_t N>
    using arr_st = std::array<size_t, N>;

    template<typename C1, typename C2>
    void chk_iterable_eq(const C1 &c1, const C2 &c2) {
        ASSERT_EQ(c1.size(), c2.size());
        bool same = std::is_same_v<typename C1::value_type, 
            typename C2::value_type>;
        EXPECT_TRUE(same);
        for(size_t i=0; i<c1.size(); ++i){
            EXPECT_EQ(c1[i], c2[i]);
        }
    }
    template<typename T1, typename T2>
    void chk_type_eq() {
        bool same = std::is_same_v<T1, T2>;
        EXPECT_TRUE(same);
    }
};

TEST_F(ConceptRawArrayTest, HelperSizeExtents) {
    using t1 = int[3];
    using t2 = double[3][4];
    using at1 = std::array<int, 3>;
    using at2 = std::array<std::array<double, 4>, 3>;
    class A {};
    
    auto ext1 = helper_t::extents<t1>();
    auto ext2 = helper_t::extents<t2>();
    chk_iterable_eq(ext1, arr_st<1>{3});
    chk_iterable_eq(ext2, arr_st<2>{3, 4});
    
    size_t sz1 = helper_t::size<t1>(),
        sz2 = helper_t::size<t2>();
    EXPECT_EQ(sz1, 3);
    EXPECT_EQ(sz2, 12);
}

TEST_F(ConceptRawArrayTest, HelperExtentsToRawArray) {
    struct A {};
    using t0 = helper_t::extents_to_array_t<double>;
    using t0A = helper_t::extents_to_array_t<A>;
    using t1 = helper_t::extents_to_array_t<double,3>;
    using t1A = helper_t::extents_to_array_t<A,3>;
    using t3 = helper_t::extents_to_array_t<double,3, 4, 5>;
    using t3A = helper_t::extents_to_array_t<A,3, 4, 5>;

    chk_type_eq<t0, double>();
    chk_type_eq<t0A, A>();
    chk_type_eq<t1, double[3]>();
    chk_type_eq<t1A, A[3]>();
    chk_type_eq<t3, double[3][4][5]>();
    chk_type_eq<t3A, A[3][4][5]>();
}


TEST_F(ConceptRawArrayTest, HelperStdArray) {
    using t1 = int[3];
    using t2 = double[3][4];
    using at1 = std::array<int, 3>;
    using at2 = std::array<std::array<double, 4>, 3>;
    class A {};

    bool y1 = helper_t::is_std_array_v<t1>,
        ya1 = helper_t::is_std_array_v<at1>, 
        ya2 = helper_t::is_std_array_v<at2>,
        yA = helper_t::is_std_array_v<A>,
        yInt = helper_t::is_std_array_v<int>;
    EXPECT_FALSE(y1);
    EXPECT_TRUE(ya1);
    EXPECT_TRUE(ya2);
    EXPECT_FALSE(yA);
    EXPECT_FALSE(yInt);

    chk_type_eq<helper_t::std_array_to_raw_t<at1>, t1>();
    chk_type_eq<helper_t::std_array_to_raw_t<at2>, t2>();
}

TEST_F(ConceptRawArrayTest, HelperStdArrayConst) {
    using t1 = const int[3];
    using t2 = const double[3][4];
    using at11 = std::array<const int, 3>;
    using at12 = const std::array<int, 3>;
    using at21 = std::array<std::array<const double, 4>, 3>;
    using at22 = std::array<const std::array<double, 4>, 3>;
    using at23 = const std::array<std::array<double, 4>, 3>;

    chk_type_eq<helper_t::std_array_to_raw_t<at11>, t1>();
    chk_type_eq<helper_t::std_array_to_raw_t<at12>, t1>();
    chk_type_eq<helper_t::std_array_to_raw_t<at21>, t2>();
    chk_type_eq<helper_t::std_array_to_raw_t<at22>, t2>();
    chk_type_eq<helper_t::std_array_to_raw_t<at23>, t2>(); 
}

TEST_F(ConceptRawArrayTest, Traits) {
    using t1 = int[3];
    using t2 = double[3][4];
    using at1 = std::array<int, 3>;
    using at2 = std::array<std::array<double, 4>, 3>;

    using t_t1 = traits_t<t1>;
    using t_t2 = traits_t<t2>;
    using t_at1 = traits_t<at1>;
    using t_at2 = traits_t<at2>;

    bool s1 = std::is_same_v<t_t1::array_t, t_at1::array_t>,
        s2 = std::is_same_v<t_t2::array_t, t_at2::array_t>,
        s3 = std::is_same_v<t_t1::value_t, t_at1::value_t>,
        s4 = std::is_same_v<t_t2::value_t, t_at2::value_t>,
        s5 = std::is_same_v<t_at1::array_t, int[3]>,
        s6 = std::is_same_v<t_at2::array_t, double [3][4]>;
    EXPECT_TRUE(s1);
    EXPECT_TRUE(s2);
    EXPECT_TRUE(s3);
    EXPECT_TRUE(s4);
    EXPECT_TRUE(s5);
    EXPECT_TRUE(s6);

    EXPECT_EQ(t_t1::size, 3);
    EXPECT_EQ(t_at1::size, 3);
    EXPECT_EQ(t_t2::size, 12);
    EXPECT_EQ(t_at2::size, 12);


    chk_iterable_eq(t_t1::extents, arr_st<1>{3});
    chk_iterable_eq(t_at1::extents, arr_st<1>{3});
    chk_iterable_eq(t_t2::extents, arr_st<2>{3, 4});
    chk_iterable_eq(t_at2::extents, arr_st<2>{3, 4});
}

TEST_F(ConceptRawArrayTest, TraitsConst) {
    using t1 = const int[3];
    using t2 = const double[3][4];
    using at11 = std::array<const int, 3>;
    using at12 = const std::array<int, 3>;
    using at21 = std::array<std::array<const double, 4>, 3>;
    using at22 = std::array<const std::array<double, 4>, 3>;
    using at23 = const std::array<std::array<double, 4>, 3>;

    chk_type_eq< typename traits_t<at11>::array_t, t1>();
    chk_type_eq< typename traits_t<at12>::array_t, t1>();
    chk_type_eq< typename traits_t<at21>::array_t, t2>();
    chk_type_eq< typename traits_t<at22>::array_t, t2>();
    chk_type_eq< typename traits_t<at23>::array_t, t2>();

    chk_type_eq< typename traits_t<at11>::value_t, const int>();
    chk_type_eq< typename traits_t<at12>::value_t, const int>();
    chk_type_eq< typename traits_t<at21>::value_t, const double>();
    chk_type_eq< typename traits_t<at22>::value_t, const double>();
    chk_type_eq< typename traits_t<at23>::value_t, const double>();
}


class ConceptContiguousBufferTest : public ::testing::Test {
protected:
    template<typename T>
    using traits_t = ContiguousBufferTraits<T>;

    template<typename T1, typename T2>
    void chk_type_eq() {
        bool same = std::is_same_v<T1, T2>;
        EXPECT_TRUE(same);
    }
    template<typename T1, typename T2>
    void chk_buff_eq(T1 *p1, size_t n1, T2 *p2, size_t n2) {
        EXPECT_EQ((void *)p1, (void *)p2);
        EXPECT_EQ(n1, n2);
    }

    template<typename T, typename T1>
    void chk_buff_eq(const traits_t<T> &t, T1 *p, size_t n) {
        auto [p1, n1] = t;
        chk_buff_eq(p1, n1, p, n);
    }

    template<typename T1, typename T2>
    void chk_range_eq(const T1 &t1, const T2 &t2) {
        auto b1 =  std::begin(t1), e1 = std::end(t1);
        auto b2 =  std::begin(t2), e2 = std::end(t2);
        while( b1 != e1 ) {
            ASSERT_NE(b2, e2);
            EXPECT_EQ(*b1++, *b2++);
        }
        EXPECT_EQ(b2, e2);
    }

    int b1[4] {};
    const int b1c[4] {};
    std::array<int, 3> b2;
    const std::array<int, 3> b2c1 {};
    std::array<const int, 3> b2c2 {};
    std::vector<double> b3 = vector<double>(5.0, 3);
    const std::vector<double> b3c1 = vector<double>(5.0, 3);

    class A {} nb1;
    int nb2 {};

    typedef traits_t<decltype(b1)> t_b1;
    typedef traits_t<decltype(b1c)> t_b1c;
    typedef traits_t<decltype(b2)> t_b2;
    typedef traits_t<decltype(b2c1)> t_b2c1;
    typedef traits_t<decltype(b2c2)> t_b2c2;
    typedef traits_t<decltype(b3)> t_b3;
    typedef traits_t<decltype(b3c1)> t_b3c1;

    typedef traits_t<decltype(nb1)> t_nb1;
    typedef traits_t<decltype(nb2)> t_nb2;
};

TEST_F(ConceptContiguousBufferTest, ProtocolCompliance){
    EXPECT_TRUE( t_b1::is_buffer );
    EXPECT_TRUE( traits_t<decltype(b1c)>::is_buffer );
    EXPECT_TRUE( t_b2::is_buffer );
    EXPECT_TRUE( t_b2c1::is_buffer );
    EXPECT_TRUE( t_b2c2::is_buffer );
    EXPECT_TRUE( t_b3::is_buffer );
    EXPECT_TRUE( t_b3c1::is_buffer );

    // non compliant
    EXPECT_FALSE( t_nb1::is_buffer );
    EXPECT_FALSE( t_nb2::is_buffer );

    // const
    EXPECT_FALSE( t_b1::is_const );
    EXPECT_FALSE( t_b2::is_const );
    EXPECT_FALSE( t_b3::is_const );

    EXPECT_TRUE( t_b1c::is_const );
    EXPECT_TRUE( t_b2c1::is_const );
    EXPECT_TRUE( t_b2c2::is_const );
    EXPECT_TRUE( t_b3c1::is_const );
    
    // value_t and cvalue_t
    chk_type_eq< typename t_b1::value_t, int >();
    chk_type_eq< typename t_b1::cvalue_t, const int >();
    chk_type_eq< typename t_b1c::value_t, const int >();
    chk_type_eq< typename t_b1c::cvalue_t, const int >();
    chk_type_eq< typename t_b2::value_t, int >();
    chk_type_eq< typename t_b2::cvalue_t, const int >();
    chk_type_eq< typename t_b2c1::value_t, const int >();
    chk_type_eq< typename t_b2c1::cvalue_t, const int >();
    chk_type_eq< typename t_b2c2::value_t, const int >();
    chk_type_eq< typename t_b2c2::cvalue_t, const int >();
    chk_type_eq< typename t_b3::value_t, double >();
    chk_type_eq< typename t_b3::cvalue_t, const double >();
    chk_type_eq< typename t_b3c1::value_t, const double >();
    chk_type_eq< typename t_b3c1::cvalue_t, const double >();

    // buffer_t
    chk_type_eq< typename t_b1::buffer_t, decltype(b1) >();
    chk_type_eq< typename t_b1c::buffer_t, decltype(b1c) >();
    chk_type_eq< typename t_b2::buffer_t, decltype(b2) >();
    chk_type_eq< typename t_b2c1::buffer_t, decltype(b2c1) >();
    chk_type_eq< typename t_b2c2::buffer_t, decltype(b2c2) >();
    chk_type_eq< typename t_b3::buffer_t, decltype(b3) >();
    chk_type_eq< typename t_b3c1::buffer_t, decltype(b3c1) >();
}

TEST_F(ConceptContiguousBufferTest, TraitsObjectConstruction){
    t_b1 o_b1 {b1};
    t_b1c o_b1c {b1c};
    t_b2 o_b2 {b2};
    t_b2c1 o_b2c1 {b2c1};
    t_b2c2 o_b2c2 {b2c2};
    t_b3 o_b3 {b3};
    t_b3c1 o_b3c1 {b3c1};

    // buffer range
    chk_buff_eq(o_b1, b1, 4);
    chk_buff_eq(o_b1c, b1c, 4);
    chk_buff_eq(o_b2, b2.data(), b2.size());
    chk_buff_eq(o_b2c1, b2c1.data(), b2c1.size());
    chk_buff_eq(o_b2c2, b2c2.data(), b2c2.size());
    chk_buff_eq(o_b3, b3.data(), b3.size());
    chk_buff_eq(o_b3c1, b3c1.data(), b3c1.size());

    // buffer element direct comparison
    chk_range_eq(o_b1, b1);
    chk_range_eq(o_b1c, b1c);
    chk_range_eq(o_b2, b2);
    chk_range_eq(o_b2c1, b2c1);
    chk_range_eq(o_b2c2, b2c2);
    chk_range_eq(o_b3, b3);
    chk_range_eq(o_b3c1, b3c1);
}

class ContiguousBufferTest : public ::testing::Test {
protected:
    
    typedef ContiguousBuffer<int> ibuf_t;
    typedef ContiguousBuffer<const int> cibuf_t;
    typedef ContiguousBuffer<double> dbuf_t;
    typedef ContiguousBuffer<const double> cdbuf_t;

    template<typename T>
    auto take_at_idx(const T &v, cibuf_t idx) {
        vector<typename T::value_type> res;
        for(auto i: idx) res.push_back(v[i]);
        return res;
    }

    template<typename T1, typename T2>
    void chk_type_eq() {
        bool same = std::is_same_v<T1, T2>;
        EXPECT_TRUE(same);
    }
    template<typename T1, typename T2>
    void chk_range_eq(const T1 &t1, const T2 &t2) {
        auto b1 =  std::begin(t1), e1 = std::end(t1);
        auto b2 =  std::begin(t2), e2 = std::end(t2);
        while( b1 != e1 ) {
            ASSERT_NE(b2, e2);
            EXPECT_EQ(*b1++, *b2++);
        }
        EXPECT_EQ(b2, e2);
    }

    int b1[4] {0,1,2,3};
    const int b1c[4] {3,2,1,0};
    std::array<int, 3> b2 {0,1,2};
    const std::array<int, 3> b2c1 {2,1,0};
    std::array<const int, 3> b2c2 {2,1,0};
    std::vector<double> b3 = vector<double>(5.0, 3);
    const std::vector<double> b3c1 = vector<double>(5.0, 3);
};

TEST_F(ContiguousBufferTest, MemberDefinition){
    chk_type_eq< typename ibuf_t::value_t, int >();
    chk_type_eq< typename cibuf_t::value_t, const int >();
    chk_type_eq< typename dbuf_t::value_t, double >();

    chk_type_eq< typename ibuf_t::cvalue_t, const int >();
    chk_type_eq< typename cibuf_t::cvalue_t, const int >();
    chk_type_eq< typename dbuf_t::cvalue_t, const double >();
}

TEST_F(ContiguousBufferTest, Construction){
    ibuf_t cb1 {b1};
    cibuf_t cb11 {b1}, cb12{b1c}, cb13{b2}, cb14{b2c1}, cb15 {b2c2};
    dbuf_t cb2 {b3};
    cdbuf_t cb21 {b3c1};

    chk_range_eq(b1, cb1);
    chk_range_eq(b1, cb11);
    chk_range_eq(b1c, cb12);
    chk_range_eq(b2, cb13);
    chk_range_eq(b2c1, cb14);
    chk_range_eq(b2c2, cb15);
    chk_range_eq(b3, cb2);
    chk_range_eq(b3c1, cb21);
}

TEST_F(ContiguousBufferTest, ApplicationAsIndices) {
    cibuf_t idx1 {b1}, idx2{b1c}, idx3{b2}, idx4{b2c1}, idx5{b2c2};
    
    vector<int> v = {0,1,2,3,4,5};
    vector<int> v1 = {0,1,2,3}, v2 = {3,2,1,0}, v3 = {0,1,2},
        v4 = {2,1,0}, v5 = {2,1,0};

    chk_range_eq( take_at_idx(v, idx1), v1);
    chk_range_eq( take_at_idx(v, idx2), v2);
    chk_range_eq( take_at_idx(v, idx3), v3);
    chk_range_eq( take_at_idx(v, idx4), v4);
    chk_range_eq( take_at_idx(v, idx5), v5);

    chk_range_eq( take_at_idx(v, b1), v1);
    chk_range_eq( take_at_idx(v, b1c), v2);
    chk_range_eq( take_at_idx(v, b2), v3);
    chk_range_eq( take_at_idx(v, b2c1), v4);
    chk_range_eq( take_at_idx(v, b2c2), v5);
}

}
} // namespace HIPP
