#include <h5_test_incl.h>

namespace HIPP::IO::H5 {

namespace {


class DatasetManagerTest : public gt::Test, public H5TestFile {
protected:

    string fixture_name() override { return "DatasetManagerTest"; }

    void SetUp() override {
        _file0 = create_file("file0", "w");
        _g0 = _file0.create_group("g0");
        _dsets0 = _g0.datasets();
    }
    void TearDown() override {
        _dsets0.reset();
        _g0.free();
        _file0.free();
        clear_up();
    }

    File _file0;
    Group _g0;
    DatasetManager _dsets0;
};

TEST_F(DatasetManagerTest, ManagerCreation) {
    DatasetManager dsets;
    EXPECT_FALSE(dsets.refobj().has_referenced());

    EXPECT_TRUE(_dsets0.refobj().has_referenced());

    auto f = create_file("ManagerCreation", "w");
    auto g = f.create_group("g0");
    dsets = g.datasets();
    EXPECT_TRUE(dsets.refobj().has_referenced());
}

TEST_F(DatasetManagerTest, Put) {
    int x {1};
    vector<long> v5 {1,2,3,4,5};
    vector<std::array<int, 4> > va64 (6);
    
    _dsets0.put("x", x);
    _dsets0.put("v5", v5);
    _dsets0.put("va64", va64);
    EXPECT_NO_THROW(_dsets0.put("v5", v5));

    int in_x;
    vector<long> in_v5;
    vector<std::array<int, 4> > in_va64;
    _g0.open_dataset("x").read(in_x);
    _g0.open_dataset("v5").read(in_v5);
    _g0.open_dataset("va64").read(in_va64);

    EXPECT_EQ(in_x, x);
    EXPECT_THAT(in_v5, gt::ContainerEq(v5));
    EXPECT_THAT(in_va64, gt::ContainerEq(va64));
}

TEST_F(DatasetManagerTest, PutSelected) {
    vector<long> v5 {1,2,3,4,5}, v5_in;

    _dsets0.put("v5", v5);

    _dsets0.put_at("v5", 1, {3});
    _dsets0.put_at("v5", 2, {4});
    _g0.open_dataset("v5").read(v5_in);
    EXPECT_THAT(v5_in, gt::ElementsAre(1,2,3,1,2));

    long v5_at_14[3] {10,11,12};
    _dsets0.put_slab("v5", v5_at_14, { {1}, {3} });
    _g0.open_dataset("v5").read(v5_in);
    EXPECT_THAT(v5_in, gt::ElementsAre(1,10, 11, 12, 2));

    long v5_at_0_1_3[3] {100,101,102};
    hsize_t coords[3] {0,1,3};
    _dsets0.put_ats("v5", v5_at_0_1_3, {3, coords});
    _g0.open_dataset("v5").read(v5_in);
    EXPECT_THAT(v5_in, gt::ElementsAre(100,101,11,102, 2));
}

TEST_F(DatasetManagerTest, PutSelected2D) {
    long a34[3][4] = {{1,2,3,4},{5,6,7,8},{9,10,11,12}};
    vector<long> a34_in;

    _dsets0.put("a34", a34);
    
    _dsets0.put_at("a34", 100, {0,0});
    _dsets0.put_at("a34", 101, {0,1});

    hsize_t coords[] = {1,1, 1,2, 1,3};
    long d2put[3] = {200,201,202};
    _dsets0.put_ats("a34", d2put, {3, coords});

    long dslab[4] = {300, 301, 302,303};
    _dsets0.put_slab("a34", dslab, { {1,0}, {2,2} });

    _g0.open_dataset("a34").read(a34_in);
    EXPECT_THAT(a34_in, gt::ElementsAre(
        100,101,3,4,
        300,301,201,202,
        302,303,11,12));
}

TEST_F(DatasetManagerTest, Get) {
    int x {1};
    vector<long> v5 {1,2,3,4,5};
    vector<std::array<int, 4> > va34 (3);

    _g0.create_dataset_for("x", x).write(x);
    _g0.create_dataset_for("v5", v5).write(v5);
    _g0.create_dataset_for("va34", va34).write(va34);

    int in_x;
    vector<long> in_v5;
    vector<std::array<int, 4> > in_va34;
    _dsets0.get("x", in_x);
    _dsets0.get("v5", in_v5);
    _dsets0.get("va34", in_va34);

    EXPECT_EQ(in_x, x);
    EXPECT_THAT(in_v5, gt::ContainerEq(v5));
    EXPECT_THAT(in_va34, gt::ContainerEq(va34));

    in_x = _dsets0.get<int>("x");
    v5 = _dsets0.get<vector<long> >("v5"); 
    in_va34 = _dsets0.get< vector<std::array<int, 4> > >("va34");

    EXPECT_EQ(in_x, x);
    EXPECT_THAT(in_v5, gt::ContainerEq(v5));
    EXPECT_THAT(in_va34, gt::ContainerEq(va34));
}

TEST_F(DatasetManagerTest, GetSelected2D) {
    long a34[3][4] = {
        {1,2,3,4},
        {5,6,7,8},
        {9,10,11,12}
    };
    
    _dsets0.put("a34", a34);
    
    vector<long> a34_in;
    vector< std::array<long, 2> > a34_in_2;
    long a34_in_3;

    _dsets0.at("a34", a34_in_3, {2,3});
    EXPECT_EQ(a34_in_3, 12);
    a34_in_3 = _dsets0.at<long>("a34", {2,3});
    EXPECT_EQ(a34_in_3, 12);

    hsize_t coords[6] = {0,2, 1,1, 2,3};
    _dsets0.ats("a34", a34_in, {3, coords});
    EXPECT_THAT(a34_in, gt::ElementsAre(3, 6, 12));

    a34_in = _dsets0.ats<vector<long> >("a34", {3, coords});
    EXPECT_THAT(a34_in, gt::ElementsAre(3, 6, 12));

    _dsets0.slab("a34", a34_in, {{1,1}, {2,2}});
    EXPECT_THAT(a34_in, gt::ElementsAre(6,7,10,11));
    _dsets0.slab("a34", a34_in, {{1,1}, {2,2}, {1, 2}, {}});
    EXPECT_THAT(a34_in, gt::ElementsAre(6,8,10,12));
    a34_in = _dsets0.slab< vector<long> >("a34", {{1,1}, {2,2}, {1, 2}, {}});
    EXPECT_THAT(a34_in, gt::ElementsAre(6,8,10,12));

    _dsets0.slab("a34", a34_in_2, {{1,1}, {2,2}});
    EXPECT_EQ(a34_in_2.size(), 2);
    EXPECT_THAT(a34_in_2[0], gt::ElementsAre(6,7));
    EXPECT_THAT(a34_in_2[1], gt::ElementsAre(10,11));
}

} // namespace
} // namespace HIPP::IO::H5