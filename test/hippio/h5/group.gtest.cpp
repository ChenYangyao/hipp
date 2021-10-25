#include <h5_test_incl.h>

namespace HIPP::IO::H5 {

namespace {

class GroupTest : public gt::Test, public H5TestFile {
protected:

    string fixture_name() override { return "GroupTest"; }

    void SetUp() override {
        _file0 = create_file("file0", "w");
    }
    void TearDown() override {
        _file0.free();
        clear_up();
    }

    File _file0 {nullptr};
};

TEST_F(GroupTest, GroupCreation){
    auto root = _file0.open_group("/");
    ASSERT_THAT(root, gt::An<Group>());

    auto g0 = _file0.create_group("g0");
    ASSERT_THAT(_file0, gt::An<File>());
    ASSERT_THAT(g0, gt::An<Group>());

    auto g00 = g0.create_group("g00");
    ASSERT_THAT(g00, gt::An<Group>());
    
    auto g000 = g00.create_group("g000"),
        g01 = g0.create_group("g01"),
        g02 = g0.create_group("g02");

    ASSERT_THROW( g0.create_group("g00", "x"), ErrH5 );
    ASSERT_THROW( g0.create_group("g01", "x"), ErrH5 );
    ASSERT_THROW( g0.create_group("g02", "t"), ErrLogic );
    ASSERT_NO_THROW( g0.open_group("g00/g000") );
}

TEST_F(GroupTest, GroupInfo) {
    Group::info_t info;
    auto g0 = _file0.create_group("g0");
    
    _file0.get_info(info);
    ASSERT_EQ(info.nlinks, 1);

    g0.get_info(info);
    ASSERT_EQ(info.nlinks, 0);

    auto g00 = g0.create_group("g00"),
        g01 = g0.create_group("g01"),
        g000 = g00.create_group("g000");
    g0.get_info(info);
    ASSERT_EQ(info.nlinks, 2);
    g0.get_info(".", info);
    ASSERT_EQ(info.nlinks, 2);
    g0.get_info("g00", info);
    ASSERT_EQ(info.nlinks, 1);
    g0.get_info("g00/g000", info);
    ASSERT_EQ(info.nlinks, 0);

    EXPECT_THROW(g0.get_info("g02", info), ErrH5);
    
    auto d00 = g0.create_dataset<double>("d00", {1,2});
    g0.get_info(info);
    ASSERT_EQ(info.nlinks, 3);

    auto d0 = _file0.create_dataset<double>("d0", {1,2});
    _file0.get_info(info);
    ASSERT_EQ(info.nlinks, 2);
}

TEST_F(GroupTest, ObjectInfo) {
    Group::obj_info_t info;
    auto g0 = _file0.create_group("g0");
    auto g00 = g0.create_group("g00");
    auto g000 = g00.create_group("g000");
    auto d00 = g0.create_dataset<double>("d00", {1, 2});

    _file0.get_object_info(info);
    EXPECT_EQ(info.type, g0.tGROUP);
    EXPECT_EQ(info.num_attrs, 0);
    
    g0.get_object_info(info);
    EXPECT_EQ(info.type, g0.tGROUP);
    EXPECT_EQ(info.num_attrs, 0);
    
    g00.get_object_info(info);
    EXPECT_EQ(info.type, g00.tGROUP);
    EXPECT_EQ(info.num_attrs, 0);
    
    d00.get_info(info);
    EXPECT_EQ(info.type, d00.tDATASET);
    EXPECT_EQ(info.num_attrs, 0);
    d00.create_attr_scalar<double>("a");
    d00.get_info(info);
    EXPECT_EQ(info.num_attrs, 1);

    _file0.get_object_info(".", info);
    EXPECT_EQ(info.type, g0.tGROUP);
    EXPECT_EQ(info.num_attrs, 0);

    g0.get_object_info(".", info);
    EXPECT_EQ(info.type, g0.tGROUP);
    EXPECT_EQ(info.num_attrs, 0);
    
    g0.get_object_info("g00", info);
    EXPECT_EQ(info.type, g0.tGROUP);
    EXPECT_EQ(info.num_attrs, 0);


    g0.get_object_info("g00/g000", info);
    EXPECT_EQ(info.type, g0.tGROUP);
    EXPECT_EQ(info.num_attrs, 0);
    
    EXPECT_THROW(g0.get_object_info("g01", info), ErrH5);

    g0.create_attr<double>("a", {2,2});
    g0.get_object_info(info);
    EXPECT_EQ(info.num_attrs, 1);
}

TEST_F(GroupTest, SubObjectExists) {
    auto g0 = _file0.create_group("g0");
    auto d0 = _file0.create_dataset<double>("d0",{4,5});
    auto a0 = _file0.create_attr<double>("a0", {2,3});
    auto g00 = g0.create_group("g00");
    auto d00 = g0.create_dataset<int>("d00", {2,3});
    auto a00 = g0.create_attr<int>("a00", {2,3});

    EXPECT_TRUE(_file0.link_exists("g0"));
    EXPECT_TRUE(_file0.link_exists("d0"));
    EXPECT_TRUE(g0.link_exists("g00"));
    EXPECT_TRUE(g0.link_exists("d00"));

    EXPECT_FALSE(_file0.link_exists("g1"));
    EXPECT_FALSE(_file0.link_exists("d1"));
    EXPECT_FALSE(g0.link_exists("g01"));
    EXPECT_FALSE(g0.link_exists("d01"));

    EXPECT_TRUE(_file0.dataset_exists("d0"));
    EXPECT_TRUE(_file0.group_exists("g0"));
    EXPECT_TRUE(g0.dataset_exists("d00"));
    EXPECT_TRUE(g0.group_exists("g00"));

    EXPECT_FALSE(_file0.dataset_exists("g0"));
    EXPECT_FALSE(_file0.dataset_exists("d1"));
    EXPECT_FALSE(_file0.group_exists("d0"));
    EXPECT_FALSE(_file0.group_exists("g1"));
    EXPECT_FALSE(g0.dataset_exists("g00"));
    EXPECT_FALSE(g0.dataset_exists("d01"));
    EXPECT_FALSE(g0.group_exists("d00"));
    EXPECT_FALSE(g0.group_exists("g01"));

    EXPECT_TRUE(_file0.attr_exists("a0"));
    EXPECT_TRUE(g0.attr_exists("a00"));

    EXPECT_FALSE(_file0.attr_exists("a1"));
    EXPECT_FALSE(g0.attr_exists("a01"));
}

TEST_F(GroupTest, DatasetCreationGeneralCall) {
    auto g = _file0.create_group("g0");
    {
        auto d_d23 = g.create_dataset("d23", NATIVE_DOUBLE_T, {2,3});
        EXPECT_THROW(g.create_dataset("d23", NATIVE_DOUBLE_T, {2,3}, 
            "x"), ErrH5);
        EXPECT_NO_THROW(g.create_dataset("d23", NATIVE_DOUBLE_T, {2,3}));
        EXPECT_NO_THROW(g.create_dataset("d23", NATIVE_DOUBLE_T, {2,3}, "ca"));
        EXPECT_THROW(g.create_dataset("d23", NATIVE_DOUBLE_T, {2,3}, "g"), 
            ErrLogic);
        EXPECT_NO_THROW(g.open_dataset("d23"));

        auto dims = d_d23.dataspace().dims();
        size_t ndim = dims.ndims(), n_elems = dims.n_elems();
        EXPECT_EQ(ndim, 2);
        EXPECT_EQ(n_elems, 6);
        EXPECT_THAT(dims.as_vector(), gt::ElementsAre(2,3));

        auto dtype = d_d23.datatype();
        EXPECT_EQ(dtype.get_class(), dtype.cFLOAT);
        EXPECT_EQ(dtype.size(), sizeof(double));
    }
    {
        auto d_d23 = g.open_dataset("d23");
        auto dims = d_d23.dataspace().dims();
        size_t ndim = dims.ndims(), n_elems = dims.n_elems();
        EXPECT_EQ(ndim, 2);
        EXPECT_EQ(n_elems, 6);
        EXPECT_THAT(dims.as_vector(), gt::ElementsAre(2,3));

        auto dtype = d_d23.datatype();
        EXPECT_EQ(dtype.get_class(), dtype.cFLOAT);
        EXPECT_EQ(dtype.size(), sizeof(double));
    }
}

TEST_F(GroupTest, DatasetCreationWithAutoType) {
    auto g = _file0.create_group("g0");
    g.create_dataset<double>("d23", {2,3});
    
    auto d_d23 = g.open_dataset("d23");
    auto dims = d_d23.dataspace().dims();
    size_t ndim = dims.ndims(), n_elems = dims.n_elems();
    EXPECT_EQ(ndim, 2);
    EXPECT_EQ(n_elems, 6);
    EXPECT_THAT(dims.as_vector(), gt::ElementsAre(2,3));

    auto dtype = d_d23.datatype();
    EXPECT_EQ(dtype.get_class(), dtype.cFLOAT);
    EXPECT_EQ(dtype.size(), sizeof(double));

    g.create_dataset<int>("i23", {2,3});
    
    auto d_i23 = g.open_dataset("i23");
    dims = d_i23.dataspace().dims();
    ndim = dims.ndims(), n_elems = dims.n_elems();
    EXPECT_EQ(ndim, 2);
    EXPECT_EQ(n_elems, 6);
    EXPECT_THAT(dims.as_vector(), gt::ElementsAre(2,3));

    dtype = d_i23.datatype();
    EXPECT_EQ(dtype.get_class(), dtype.cINTEGER);
    EXPECT_EQ(dtype.size(), sizeof(int));
}

TEST_F(GroupTest, DatasetCreationScalar) {
    auto g = _file0.create_group("g0");
    g.create_dataset_scalar<double>("d0");
    g.create_dataset_scalar<int>("i0");
    
    auto d0 = g.open_dataset("d0");
    auto i0 = g.open_dataset("i0");

    auto ds = d0.dataspace();
    EXPECT_EQ(ds.get_type(), ds.cSCALAR);
    ds = i0.dataspace();
    EXPECT_EQ(ds.get_type(), ds.cSCALAR);

    auto dtype = d0.datatype();
    EXPECT_EQ(dtype.get_class(), dtype.cFLOAT);
    EXPECT_EQ(dtype.size(), sizeof(double));

    dtype = i0.datatype();
    EXPECT_EQ(dtype.get_class(), dtype.cINTEGER);
    EXPECT_EQ(dtype.size(), sizeof(int));
}

TEST_F(GroupTest, DatasetCreationStr) {
    auto g = _file0.create_group("g0");
    g.create_dataset_str("s0", 50);
    g.create_dataset_str("s1", 8, 50);

    auto s0 = g.open_dataset("s0"),
        s1 = g.open_dataset("s1");

    auto dsp0 = s0.dataspace(),
        dsp1 = s1.dataspace();
    EXPECT_EQ(dsp0.get_type(), dsp0.cSCALAR);

    EXPECT_EQ(dsp1.get_type(), dsp1.cSIMPLE);
    EXPECT_EQ(dsp1.size(), 8);
    EXPECT_THAT(dsp1.dims().as_vector(), gt::ElementsAre(8));

    auto dtype0 = s0.datatype(),
        dtype1 = s1.datatype();
    EXPECT_EQ(dtype0.get_class(), dtype0.cSTRING);
    EXPECT_EQ(dtype1.get_class(), dtype1.cSTRING);
    EXPECT_EQ(dtype0.size(), 50);
    EXPECT_EQ(dtype1.size(), 50);
}

TEST_F(GroupTest, DatasetCreationFor) {
    auto g = _file0.create_group("g0");
    
    int i0;
    vector<double> d5(5);
    long l34[3][4];
    vector<std::array<int, 3>> i63(6);

    g.create_dataset_for("i0", i0);
    g.create_dataset_for("d5", d5);
    EXPECT_THROW(g.create_dataset_for("d5", d5, "x"), ErrH5);
    EXPECT_THROW(g.create_dataset_for("d5", d5, "g"), ErrLogic);
    g.create_dataset_for("l34", l34);
    g.create_dataset_for("i63", i63);

    auto 
        d_i0 = g.open_dataset("i0"),
        d_d5 = g.open_dataset("d5"),
        d_l34 = g.open_dataset("l34"),
        d_i63 = g.open_dataset("i63");
    
    {
        auto dsp = d_i0.dataspace();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
    }
    {
        auto dims = d_d5.dataspace().dims();
        auto ndim = dims.ndims(); auto n_elems = dims.n_elems();
        EXPECT_EQ(ndim, 1);
        EXPECT_EQ(n_elems, 5);
        EXPECT_THAT(dims.as_vector(), gt::ElementsAre(5));
    }
    {
        auto dims = d_l34.dataspace().dims();
        auto ndim = dims.ndims(); auto n_elems = dims.n_elems();
        EXPECT_EQ(ndim, 2);
        EXPECT_EQ(n_elems, 12);
        EXPECT_THAT(dims.as_vector(), gt::ElementsAre(3,4));
    }
    {
        auto dims = d_i63.dataspace().dims();
        auto ndim = dims.ndims(); auto n_elems = dims.n_elems();
        EXPECT_EQ(ndim, 2);
        EXPECT_EQ(n_elems, 18);
        EXPECT_THAT(dims.as_vector(), gt::ElementsAre(6,3));
    }
}

TEST_F(GroupTest, DatasetCreationForScalar) {
    auto g = _file0.create_group("g0");

    int i0;
    long l34[3][4];
    std::array<int, 3> i3 {};

    g.create_dataset_for_scalar("i0", i0);
    pout << "create for i0", endl;
    g.create_dataset_for_scalar("l34", l34);
    pout << "create for l34", endl;
    g.create_dataset_for_scalar("i3", i3);
    pout << "create for i3", endl;

    {
        auto dset = g.open_dataset("i0");
        auto dsp = dset.dataspace();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        
    }
    pout << "verify for i0", endl;
    {
        auto dset = g.open_dataset("l34");
        auto dsp = dset.dataspace();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        auto dt = dset.datatype();
        EXPECT_EQ(dt.get_class(), dt.cARRAY);
        EXPECT_EQ(dt.array_ndims(), 2);
        pout << "Array ndims = ", dt.array_ndims(), endl;
        EXPECT_THAT(dt.array_dims().as_vector(), gt::ElementsAre(3,4));
    }
    pout << "verify for l34", endl;
    {
        auto dset = g.open_dataset("i3");
        auto dsp = dset.dataspace();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        auto dt = dset.datatype();
        EXPECT_EQ(dt.get_class(), dt.cARRAY);
        EXPECT_EQ(dt.array_ndims(), 1);
        EXPECT_THAT(dt.array_dims().as_vector(), gt::ElementsAre(3));
    }
    pout << "verify for i3", endl;
}

TEST_F(GroupTest, DatasetCreationForStr) {
    auto g = _file0.create_group("g0");

    string s1 = "sssss";
    const char *s2 = "ssssss";
    char s21[8] = "sss";
    string s3[3] = {"ss", "sss", "ssss"};
    vector<string> s4 = {"ss", "sss", "ssss"};
    char s5[4][8] = {"s", "ss", "sss", "abc"};
    
    auto d1 = g.create_dataset_for_str("s1", s1),
        d2 = g.create_dataset_for_str("s2", s2),
        d21 = g.create_dataset_for_str("s21", s21),
        d3 = g.create_dataset_for_str("s3", s3),
        d4 = g.create_dataset_for_str("s4", s4),
        d5 = g.create_dataset_for_str("s5", s5);
    
    {
        auto dt = d1.datatype(); auto dsp = d1.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 6);
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
    }
    {
        auto dt = d2.datatype(); auto dsp = d2.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 7);
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
    }
    {
        auto dt = d21.datatype(); auto dsp = d21.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 4);
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
    }
    {
        auto dt = d3.datatype(); auto dsp = d3.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 5);
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(3));
    }
    {
        auto dt = d4.datatype(); auto dsp = d4.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 5);
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(3));
    }
    {
        auto dt = d5.datatype(); auto dsp = d5.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 8);
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(4));
    }
}

TEST_F(GroupTest, ObjectRaw) {
    auto g = _file0.create_group("g0"), 
        g_copy = g;
    
    EXPECT_EQ(g.obj_raw().raw(), g.raw());
    
    EXPECT_TRUE(g.has_referenced());
    EXPECT_TRUE(g_copy.has_referenced());

    EXPECT_TRUE(g == g_copy);
    EXPECT_FALSE(g != g_copy);

    g.free();
    EXPECT_FALSE(g.has_referenced());
    EXPECT_FALSE(g == g_copy);

    g_copy = Group {nullptr};
    EXPECT_FALSE(g_copy.has_referenced());
    EXPECT_TRUE(g == g_copy);

}

class GroupLinkTest: public ::testing::Test {
protected:
    GroupLinkTest():f(nullptr), ga(nullptr), 
        gaa(nullptr), gab(nullptr), gac(nullptr),
        gb(nullptr), dad(nullptr){}
    void open(const string &filename){
        f =File(filename, "w");
        ga = f.create_group("a");
        gaa = ga.create_group("a");
        gab = ga.create_group("b");
        gac = ga.create_group("c");
        dad = ga.create_dataset<double>("d", {3,3});
        gb = f.create_group("b");
    }
    File f;
    Group ga, gaa, gab, gac, gb;
    Dataset dad;
};

/* Group */
TEST(GroupTest1, CreateTopLevel) {
    File f("GroupTest_CreateTopLevel.h5", "w");
    auto gA = f.create_group("GroupA"),
        gB = f.create_group("GroupB"),
        gRoot = f.open_group("/");
    auto gA2 = f.open_group("GroupA");
}
TEST(GroupTest1, CreateNested) {
    File f("GroupTest_CreateNested.h5", "w");
    auto gA = f.create_group("GroupA"),
        gB = gA.create_group("GroupB"),
        gC = gA.create_group("GroupC"),
        gD = gB.create_group("GroupD");
    auto gA2 = f.open_group("GroupA"),
        gD2 = f.open_group("GroupA/GroupB/GroupD"),
        dD3 = gB.open_group("GroupD");
}
/*
TEST_F(GroupLinkTest, NumberOfLinks){
    open("GroupLinkTest_NumberOfLinks.h5");
    EXPECT_EQ(f.open_group("/").n_links(), 2);
    EXPECT_EQ(f.open_group("a").n_links(), 4);
}
TEST_F(GroupLinkTest, Keys){
    open("GroupLinkTest_Keys.h5");
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
*/
} // namespace
} // namespace HIPP::IO::H5