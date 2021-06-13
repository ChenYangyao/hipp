#include <gtest/gtest.h>
#include <hippio.h>

namespace HIPP {
namespace IO {
namespace {

class H5GroupLinkTest: public ::testing::Test {
protected:
    H5GroupLinkTest():f(nullptr), ga(nullptr), 
        gaa(nullptr), gab(nullptr), gac(nullptr),
        gb(nullptr), dad(nullptr){}
    void open(const string &filename){
        f = H5File(filename, "w");
        ga = f.create_group("a");
        gaa = ga.create_group("a");
        gab = ga.create_group("b");
        gac = ga.create_group("c");
        dad = ga.create_dataset<double>("d", {3,3});
        gb = f.create_group("b");
    }
    H5File f;
    H5Group ga, gaa, gab, gac, gb;
    H5Dataset dad;
};

/* H5Group */
TEST(H5GroupTest, CreateTopLevel) {
    H5File f("H5GroupTest_CreateTopLevel.h5", "w");
    auto gA = f.create_group("GroupA"),
        gB = f.create_group("GroupB"),
        gRoot = f.open_group("/");
    auto gA2 = f.open_group("GroupA");
}
TEST(H5GroupTest, CreateNested) {
    H5File f("H5GroupTest_CreateNested.h5", "w");
    auto gA = f.create_group("GroupA"),
        gB = gA.create_group("GroupB"),
        gC = gA.create_group("GroupC"),
        gD = gB.create_group("GroupD");
    auto gA2 = f.open_group("GroupA"),
        gD2 = f.open_group("GroupA/GroupB/GroupD"),
        dD3 = gB.open_group("GroupD");
}
TEST_F(H5GroupLinkTest, NumberOfLinks){
    open("H5GroupLinkTest_NumberOfLinks.h5");
    EXPECT_EQ(f.open_group("/").n_links(), 2);
    EXPECT_EQ(f.open_group("a").n_links(), 4);
}
TEST_F(H5GroupLinkTest, Keys){
    open("H5GroupLinkTest_Keys.h5");
    auto keys_root = f.open_group("/").keys(),
        keys_a = f.open_group("a").keys();

    ASSERT_EQ(keys_root.size(), 2);
    std::sort(keys_root.begin(), keys_root.end());
    EXPECT_EQ(keys_root[0], "a");
    EXPECT_EQ(keys_root[1], "b");
    
    ASSERT_EQ(keys_a.size(), 4);
    std::sort(keys_a.begin(), keys_a.end());
    EXPECT_EQ(keys_a[0], "a");
    EXPECT_EQ(keys_a[1], "b");
    EXPECT_EQ(keys_a[2], "c");
    EXPECT_EQ(keys_a[3], "d");
}

} // namespace
} // namespace IO
} // namespace HIPP