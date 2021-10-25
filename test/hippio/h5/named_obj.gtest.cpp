#include <h5_test_incl.h>

namespace HIPP::IO::H5 {

namespace {


class NamedObjTest : public gt::Test, public H5TestFile {
protected:

    string fixture_name() override { return "NamedObjTest"; }

    void SetUp() override {
        _file0 = create_file("file0", "w");
        _g0 = _file0.create_group("g0");
        _d0 = _g0.create_dataset<double>("d0", {3,4});
    }
    void TearDown() override {
        _d0.free();
        _g0.free();
        _file0.free();
        clear_up();
    }

    File _file0;
    Group _g0;
    Dataset _d0;
};

TEST_F(NamedObjTest, AttrCreation) {
    auto l8 = _file0.create_attr("l8", NATIVE_LONG_T, {8});
    auto i32 = _file0.create_attr("i32", NATIVE_INT_T, {3,2});
    auto d0 = _file0.create_attr("d0", NATIVE_DOUBLE_T, Dataspace::vSCALAR);
    EXPECT_THROW(
        _file0.create_attr("i32", NATIVE_DOUBLE_T, Dataspace::vSCALAR, "x"),
        ErrH5);
    EXPECT_THROW(
        _file0.create_attr("i32", NATIVE_DOUBLE_T, Dataspace::vSCALAR, "g"),
        ErrLogic);
    EXPECT_NO_THROW(
        _file0.create_attr("i32", NATIVE_DOUBLE_T, Dataspace::vSCALAR, "ac"));

    
    {
        auto dsp = l8.dataspace();
        auto dt = l8.datatype();
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_EQ(dsp.ndims(), 1);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(8));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(long));
    }
    {
        auto dsp = i32.dataspace();
        auto dt = i32.datatype();
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_EQ(dsp.ndims(), 2);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(3,2));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
    }
    {
        auto dsp = d0.dataspace();
        auto dt = d0.datatype();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        EXPECT_EQ(dt.get_class(), dt.cFLOAT);
        EXPECT_EQ(dt.size(), sizeof(double));
    }
}

TEST_F(NamedObjTest, AttrCreationAutoType) {
    auto l8 = _file0.create_attr<long>("l8", {8});
    auto i32 = _file0.create_attr<int>("i32", {3,2});
    auto d0 = _file0.create_attr<double>("d0", Dataspace::vSCALAR);
    EXPECT_THROW(
        _file0.create_attr("i32", NATIVE_DOUBLE_T, Dataspace::vSCALAR, "x"),
        ErrH5);
    EXPECT_THROW(
        _file0.create_attr("i32", NATIVE_DOUBLE_T, Dataspace::vSCALAR, "g"),
        ErrLogic);
    EXPECT_NO_THROW(
        _file0.create_attr("i32", NATIVE_DOUBLE_T, Dataspace::vSCALAR, "ac"));

    {
        auto dsp = l8.dataspace();
        auto dt = l8.datatype();
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_EQ(dsp.ndims(), 1);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(8));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(long));
    }
    {
        auto dsp = i32.dataspace();
        auto dt = i32.datatype();
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_EQ(dsp.ndims(), 2);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(3,2));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
    }
    {
        auto dsp = d0.dataspace();
        auto dt = d0.datatype();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        EXPECT_EQ(dt.get_class(), dt.cFLOAT);
        EXPECT_EQ(dt.size(), sizeof(double));
    }
}

TEST_F(NamedObjTest, AttrCreationScalar) {
    auto l = _g0.create_attr_scalar<long>("l"),
        i = _g0.create_attr_scalar<int>("i"),
        d = _g0.create_attr_scalar<double>("d");
    {
        auto dsp = l.dataspace();
        auto dt = l.datatype();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(long));
    }
    {
        auto dsp = i.dataspace();
        auto dt = i.datatype();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
    }
    {
        auto dsp = d.dataspace();
        auto dt = d.datatype();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        EXPECT_EQ(dt.get_class(), dt.cFLOAT);
        EXPECT_EQ(dt.size(), sizeof(double));
    }
}

TEST_F(NamedObjTest, AttrCreationStr) {
    _d0.create_attr_str("s0", 50);
    _d0.create_attr_str("s1", 8, 50);

    auto s0 = _d0.open_attr("s0"),
        s1 = _d0.open_attr("s1");

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

TEST_F(NamedObjTest, AttrCreationFor) {
    int i0;
    vector<double> d5(5);
    long l34[3][4];
    vector<std::array<int, 3>> i63(6);

    _g0.create_attr_for("i0", i0);
    _g0.create_attr_for("d5", d5);
    EXPECT_THROW(_g0.create_attr_for("d5", d5, "x"), ErrH5);
    EXPECT_THROW(_g0.create_attr_for("d5", d5, "g"), ErrLogic);
    _g0.create_attr_for("l34", l34);
    _g0.create_attr_for("i63", i63);

    auto a_i0 = _g0.open_attr("i0"),
        a_d5 = _g0.open_attr("d5"),
        a_l34 = _g0.open_attr("l34"),
        a_i63 = _g0.open_attr("i63");
    
    {
        auto dsp = a_i0.dataspace();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
    }
    {
        auto dims = a_d5.dataspace().dims();
        auto ndim = dims.ndims(); auto n_elems = dims.n_elems();
        EXPECT_EQ(ndim, 1);
        EXPECT_EQ(n_elems, 5);
        EXPECT_THAT(dims.as_vector(), gt::ElementsAre(5));
    }
    {
        auto dims = a_l34.dataspace().dims();
        auto ndim = dims.ndims(); auto n_elems = dims.n_elems();
        EXPECT_EQ(ndim, 2);
        EXPECT_EQ(n_elems, 12);
        EXPECT_THAT(dims.as_vector(), gt::ElementsAre(3,4));
    }
    {
        auto dims = a_i63.dataspace().dims();
        auto ndim = dims.ndims(); auto n_elems = dims.n_elems();
        EXPECT_EQ(ndim, 2);
        EXPECT_EQ(n_elems, 18);
        EXPECT_THAT(dims.as_vector(), gt::ElementsAre(6,3));
    }
}

TEST_F(NamedObjTest, AttrCreationForScalar) {
    int i0;
    long l34[3][4];
    std::array<int, 3> i3 {};

    _g0.create_attr_for_scalar("i0", i0);
    _g0.create_attr_for_scalar("l34", l34);
    _g0.create_attr_for_scalar("i3", i3);

    {
        auto attr = _g0.open_attr("i0");
        auto dsp = attr.dataspace();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
    }
    {
        auto attr = _g0.open_attr("l34");
        auto dsp = attr.dataspace();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        auto dt = attr.datatype();
        EXPECT_EQ(dt.get_class(), dt.cARRAY);
        EXPECT_EQ(dt.array_ndims(), 2);
        EXPECT_THAT(dt.array_dims().as_vector(), gt::ElementsAre(3,4));
    }
    {
        auto attr = _g0.open_attr("i3");
        auto dsp = attr.dataspace();
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
        auto dt = attr.datatype();
        EXPECT_EQ(dt.get_class(), dt.cARRAY);
        EXPECT_EQ(dt.array_ndims(), 1);
        EXPECT_THAT(dt.array_dims().as_vector(), gt::ElementsAre(3));
    }
}

TEST_F(NamedObjTest, AttrCreationForStr) {

    string s1 = "sssss";
    const char *s2 = "ssssss";
    char s21[8] = "sss";
    string s3[3] = {"ss", "sss", "ssss"};
    vector<string> s4 = {"ss", "sss", "ssss"};
    char s5[4][8] = {"s", "ss", "sss", "abc"};
    
    auto a1 = _g0.create_attr_for_str("s1", s1),
        a2 = _g0.create_attr_for_str("s2", s2),
        a21 = _g0.create_attr_for_str("s21", s21),
        a3 = _g0.create_attr_for_str("s3", s3),
        a4 = _g0.create_attr_for_str("s4", s4),
        a5 = _g0.create_attr_for_str("s5", s5);
    
    {
        auto dt = a1.datatype(); auto dsp = a1.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 6);
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
    }
    {
        auto dt = a2.datatype(); auto dsp = a2.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 7);
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
    }
    {
        auto dt = a21.datatype(); auto dsp = a21.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 4);
        EXPECT_EQ(dsp.get_type(), dsp.cSCALAR);
    }
    {
        auto dt = a3.datatype(); auto dsp = a3.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 5);
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(3));
    }
    {
        auto dt = a4.datatype(); auto dsp = a4.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 5);
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(3));
    }
    {
        auto dt = a5.datatype(); auto dsp = a5.dataspace();
        EXPECT_EQ(dt.get_class(), dt.cSTRING);
        EXPECT_EQ(dt.size(), 8);
        EXPECT_EQ(dsp.get_type(), dsp.cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(4));
    }
}


} // namespace
} // namespace HIPP::IO::H5