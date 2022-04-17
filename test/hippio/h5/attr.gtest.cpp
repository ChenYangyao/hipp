#include <h5_test_incl.h>

namespace HIPP::IO::H5 {

namespace {


class AttrTest : public gt::Test, public H5TestFile {
protected:

    string fixture_name() override { return "AttrTest"; }

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

TEST_F(AttrTest, GeneralWrite) {
    vector<int> a = {1,2,3,4,5}, a_in(5);
    long b[2][4] = {{1,2,3,4},{5,6,7,8}}, b_in[2][4];

    auto da = _g0.create_attr_for("a", a),
        db = _g0.create_attr_for("b", b);

    {
        pout << da.dataspace(), endl;
        da.write(a.data(), NATIVE_INT_T);
        da.read(a_in);
        EXPECT_THAT(a_in, gt::ContainerEq(a));
    }
    {
        pout << db.dataspace(), endl;
        db.write(b, NATIVE_LONG_T);
        db.read(b_in);
        EXPECT_THAT(a_in, gt::ContainerEq(a));
    }
}

TEST_F(AttrTest, WriteAutoType) {
    typedef std::array<std::array<int, 2>, 3> arr_t;

    vector<int> a = {1,2,3,4,5}, a_in(5);

    long b[2][4] = {{1,2,3,4},{5,6,7,8}}, b_in[2][4];
    
    arr_t c = { {{1, 2}, {3, 4}, {5, 6}} }, c_in;
    
    vector<arr_t> d = { 
        {{{1,2},{3,4},{5,6}}}, 
        {{{10,11},{12,13},{14,15}}} 
    },
        d_in;
    
    double e = 2.0, e_in;
    
    auto da = _g0.create_attr_for("a", a),
        db = _g0.create_attr_for("b", b),
        dc = _g0.create_attr_for("c", c),
        dd = _g0.create_attr_for("d", d),
        de = _g0.create_attr_for("e", e);

    da.write(a);
    db.write(b);
    dc.write(c);
    dd.write(d);
    de.write(e);

    {
        EXPECT_THAT(da.dataspace().dims().as_vector(), gt::ElementsAre(5));
        EXPECT_EQ(da.datatype().get_class(), Datatype::cINTEGER);
        da.read(a_in);
        EXPECT_EQ(a, a_in);
        pout << "da", endl;
    }
    {
        EXPECT_THAT(db.dataspace().dims().as_vector(), gt::ElementsAre(2,4));
        EXPECT_EQ(db.datatype().get_class(), Datatype::cINTEGER);
        db.read(b_in);
        EXPECT_THAT(b[0], gt::ElementsAreArray(b_in[0]));
        EXPECT_THAT(b[1], gt::ElementsAreArray(b_in[1]));
        pout << "db", endl;
    }
    {
        EXPECT_THAT(dc.dataspace().dims().as_vector(), gt::ElementsAre(3,2));
        EXPECT_EQ(dc.datatype().get_class(), Datatype::cINTEGER);
        dc.read(c_in);
        EXPECT_EQ(c, c_in);
        pout << "dc", endl;
    }
    {
        EXPECT_THAT(dd.dataspace().dims().as_vector(), gt::ElementsAre(2,3,2));
        EXPECT_EQ(dd.datatype().get_class(), Datatype::cINTEGER);
        dd.read(d_in);
        EXPECT_EQ(d, d_in);
        pout << "dd", endl;
    }
    {
        EXPECT_EQ(de.dataspace().get_type(), Dataspace::cSCALAR);
        EXPECT_EQ(de.datatype().get_class(), Datatype::cFLOAT);
        de.read(e_in);
        EXPECT_FLOAT_EQ(e, e_in);
        pout << "de", endl;
    }

}

TEST_F(AttrTest, WriteStr) {
    string s1 = "sssss";                        string s1_in;
    const char *s2 = "ssssss";                  char s2_in[7];
    char s21[8] = "sss";                        char s21_in[8];
    string s3[3] = {"ss", "sss", "ssss"};       string s3_in[3];
    vector<string> s4 = {"ss", "sss", "ssss"};  vector<string> s4_in;
    char s5[4][8] = {"s", "ss", "sss", "abc"};  char s5_in[4][8];

    auto d1 = _g0.create_attr_for_str("s1", s1),
        d2 = _g0.create_attr_for_str("s2", s2),
        d21 = _g0.create_attr_for_str("s21", s21),
        d3 = _g0.create_attr_for_str("s3", s3),
        d4 = _g0.create_attr_for_str("s4", s4),
        d5 = _g0.create_attr_for_str("s5", s5);
    d1.write_str(s1);
    d2.write_str(s2);
    d21.write_str(s21);
    d3.write_str(s3);
    d4.write_str(s4);
    d5.write_str(s5);

    {
        d1.read_str(s1_in);
        EXPECT_EQ(s1, s1_in);
        pout << "d1", endl;
    }
    {
        d2.read_str(s2_in);
        EXPECT_TRUE( strcmp(s2, s2_in) == 0 );
        pout << "d2", endl;
    }
    {
        d21.read_str(s21_in);
        EXPECT_TRUE( strcmp(s21, s21_in) == 0 );
        pout << "d21", endl;
    }
    {
        d3.read_str(s3_in);
        for(int i=0; i<3; ++i)
            EXPECT_EQ(s3[i], s3_in[i]);
        pout << "d3", endl;
    }
    {
        d4.read_str(s4_in);
        EXPECT_THAT(s4, gt::ContainerEq(s4_in));
        pout << "d4", endl;
    }
    {
        d5.read_str(s5_in);
        for(int i=0; i<4; ++i)
            EXPECT_TRUE( strcmp(s5[i], s5_in[i]) == 0 );
        pout << "d5", endl;
    }

    d3.write_str(s3, 3);
    {
        d3.read_str(s3_in);
        for(int i=0; i<3; ++i)
            EXPECT_EQ(s3[i], s3_in[i]);
        pout << "d3", endl;
    }

    d5.write_str(s5, 4);
    {
        d5.read_str(s5_in);
        for(int i=0; i<4; ++i)
            EXPECT_TRUE( strcmp(s5[i], s5_in[i]) == 0 );
        pout << "d5", endl;
    }

    char *s5_p[4] = { s5[0], s5[1], s5[2], s5[3] };
    d5.write_str(s5_p, 4);
    {
        d5.read_str(s5_in);
        for(int i=0; i<4; ++i)
            EXPECT_TRUE( strcmp(s5[i], s5_in[i]) == 0 );
        pout << "d5", endl;
    }
}

TEST_F(AttrTest, MetaInfoGetter) {
    vector<int> a { 1,2,3,4 };
    double b {};

    {
        auto da = _g0.create_attr_for("a", a),
            db = _g0.create_attr_for("b", b);

        da.write(a);
        db.write(b);
    }

    
    EXPECT_TRUE(_g0.attr_exists("a"));
    EXPECT_TRUE(_g0.attr_exists("b"));
    

    auto da = _g0.open_attr("a"), 
        db = _g0.open_attr("b");
    EXPECT_GT(da.get_storage_size(), 0);
    EXPECT_GT(db.get_storage_size(), 0);
    pout << " - get_storage_size", endl;

    EXPECT_EQ(da.get_name(), "a");
    EXPECT_EQ(db.get_name(), "b");
    pout << " - get_name", endl;

    {
        auto info_a = da.get_info(),    
            info_b = db.get_info();

        EXPECT_GT(info_a.data_size, 0);
        EXPECT_GT(info_b.data_size, 0);
    }
    pout << " - get_info", endl;

    {
        auto info_a = _g0.get_attr_info(".", "a"),
            info_b = _g0.get_attr_info(".", "b");

        EXPECT_GT(info_a.data_size, 0);
        EXPECT_GT(info_b.data_size, 0);
    }
    pout << " - get_attr_info", endl;

    {
        vector<string> names;
        names.emplace_back( _g0.get_attr_name(".", 0) );
        names.emplace_back(_g0.get_attr_name(".", 1));
        EXPECT_THAT(names, gt::UnorderedElementsAre("a", "b"));
        
    }
    pout << " - get_attr_name", endl;
}

} // namespace
} // namespace HIPP::IO::H5