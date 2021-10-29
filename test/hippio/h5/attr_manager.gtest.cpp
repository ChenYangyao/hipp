#include <h5_test_incl.h>

namespace HIPP::IO::H5 {

namespace {


class AttrManagerTest : public gt::Test, public H5TestFile {
protected:

    string fixture_name() override { return "AttrManagerTest"; }

    void SetUp() override {
        _file0 = create_file("file0", "w");
        _g0 = _file0.create_group("g0");
        _attrs0 = _g0.attrs();
    }
    void TearDown() override {
        _attrs0.reset();
        _g0.free();
        _file0.free();
        clear_up();
    }

    File _file0;
    Group _g0;
    AttrManager _attrs0;
};

TEST_F(AttrManagerTest, ManagerCreation) {
    AttrManager attrs;
    EXPECT_FALSE(attrs.refobj().has_referenced());

    EXPECT_TRUE(_attrs0.refobj().has_referenced());

    auto f = create_file("ManagerCreation", "w");
    auto g = f.create_group("g0");
    attrs = g.attrs();
    EXPECT_TRUE(attrs.refobj().has_referenced());
}

TEST_F(AttrManagerTest, Put) {
    int x {1};
    vector<long> v5 {1,2,3,4,5};
    vector<std::array<int, 4> > va64 (6);
    
    _attrs0.put("x", x);
    _attrs0.put("v5", v5);
    _attrs0.put("va64", va64);
    EXPECT_NO_THROW(_attrs0.put("v5", v5));

    int in_x;
    vector<long> in_v5;
    vector<std::array<int, 4> > in_va64;
    _g0.open_attr("x").read(in_x);
    _g0.open_attr("v5").read(in_v5);
    _g0.open_attr("va64").read(in_va64);

    EXPECT_EQ(in_x, x);
    EXPECT_THAT(in_v5, gt::ContainerEq(v5));
    EXPECT_THAT(in_va64, gt::ContainerEq(va64));
}

TEST_F(AttrManagerTest, Get) {
    int x {1};
    vector<long> v5 {1,2,3,4,5};
    vector<std::array<int, 4> > va34 (3);

    _g0.create_attr_for("x", x).write(x);
    _g0.create_attr_for("v5", v5).write(v5);
    _g0.create_attr_for("va34", va34).write(va34);

    int in_x;
    vector<long> in_v5;
    vector<std::array<int, 4> > in_va34;
    _attrs0.get("x", in_x);
    _attrs0.get("v5", in_v5);
    _attrs0.get("va34", in_va34);

    EXPECT_EQ(in_x, x);
    EXPECT_THAT(in_v5, gt::ContainerEq(v5));
    EXPECT_THAT(in_va34, gt::ContainerEq(va34));

    in_x = _attrs0.get<int>("x");
    v5 = _attrs0.get<vector<long> >("v5"); 
    in_va34 = _attrs0.get< vector<std::array<int, 4> > >("va34");

    EXPECT_EQ(in_x, x);
    EXPECT_THAT(in_v5, gt::ContainerEq(v5));
    EXPECT_THAT(in_va34, gt::ContainerEq(va34));
}

} // namespace
} // namespace HIPP::IO::H5