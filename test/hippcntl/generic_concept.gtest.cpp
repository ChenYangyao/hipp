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
    using t1 = helper_t::extents_to_array_t<int,3>;
    using t1A = helper_t::extents_to_array_t<A,3>;
    using t3 = helper_t::extents_to_array_t<int,3, 4, 5>;
    using t3A = helper_t::extents_to_array_t<A,3, 4, 5>;
    chk_type_eq<t0, int>();
    chk_type_eq<t0A, A>();
    chk_type_eq<t1, int[3]>();
    chk_type_eq<t1A, A[3]>();
    chk_type_eq<t3, int[3][4][5]>();
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

}
} // namespace HIPP
