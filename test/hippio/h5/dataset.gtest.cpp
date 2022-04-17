#include <h5_test_incl.h>

namespace HIPP {
namespace IO {
namespace H5 {
namespace {

/* Dataset */
class DatasetTest: public gt::Test, public H5TestFile {
protected:
    string fixture_name() override { return "DatasetTest"; }

    void SetUp() override {
        _file0 = create_file("file0", "w");
        _g0 = _file0.create_group("g0");
    }
    void TearDown() override {
        _g0.free();
        _file0.free();
        clear_up();
    }

    File _file0;
    Group _g0;
};

TEST_F(DatasetTest, GeneralWrite) {
    vector<int> a = {1,2,3,4,5}, a_in(5);
    long b[2][4] = {{1,2,3,4},{5,6,7,8}}, b_in[2][4];

    auto da = _g0.create_dataset_for("a", a),
        db = _g0.create_dataset_for("b", b);

    {
        pout << da.dataspace(), endl;
        da.write(a.data(), NATIVE_INT_T);
        da.read(a_in);
        EXPECT_THAT(a_in, gt::ContainerEq(a));
        pout << "1";
        da.write(a.data(), NATIVE_INT_T, {5});
        da.read(a_in);
        EXPECT_THAT(a_in, gt::ContainerEq(a));
        pout << "2";
        da.write(a.data(), NATIVE_INT_T, {5}, {5});
        da.read(a_in);
        EXPECT_THAT(a_in, gt::ContainerEq(a));
        pout << "3\n";
        EXPECT_THROW(da.write(a.data(), NATIVE_INT_T, {4}), ErrH5);
    }

    {
        pout << db.dataspace(), endl;
        db.write(b, NATIVE_LONG_T);
        db.read(b_in);
        EXPECT_THAT(a_in, gt::ContainerEq(a));
        pout << "1";
        db.write(b, NATIVE_LONG_T, {2,4});
        db.read(b_in);
        EXPECT_THAT(a_in, gt::ContainerEq(a));
        pout << "2";
        db.write(b, NATIVE_LONG_T, {2,4}, {2,4});
        db.read(b_in);
        EXPECT_THAT(a_in, gt::ContainerEq(a));
        pout << "3\n";
        EXPECT_THROW(db.write(b, NATIVE_LONG_T, {2, 3}), ErrH5);
    }
}

TEST_F(DatasetTest, WriteAutoType) {
    typedef std::array<std::array<int, 2>, 3> arr_t;

    vector<int> a = {1,2,3,4,5}, a_in(5);

    long b[2][4] = {{1,2,3,4},{5,6,7,8}}, b_in[2][4];
    
    arr_t c = { {{1, 2}, {3, 4}, {5, 6}} }, c_in;
    
    vector<arr_t> d = { {{{1,2},{3,4},{5,6}}}, {{{10,11},{12,13},{14,15}}} },
        d_in;
    
    double e = 2.0, e_in;
    
    auto da = _g0.create_dataset_for("a", a),
        db = _g0.create_dataset_for("b", b),
        dc = _g0.create_dataset_for("c", c),
        dd = _g0.create_dataset_for("d", d),
        de = _g0.create_dataset_for("e", e);
    pout << "spaces\n";
    pout << "  da=", da.dataspace(), ", db=", db.dataspace(), 
        ", dc=", dc.dataspace(),
        ", dd=", dd.dataspace(), ", de=", de.dataspace(), endl;

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

TEST_F(DatasetTest, WriteHyperslab) {
    typedef std::array<std::array<int, 2>, 3> arr_t;
    vector<int> a = {1,2,3,4,5}, a_in(5),
        a_hs = {11,13,15};
    long b[2][4] = {{1,2,3,4},{5,6,7,8}}, b_in[2][4],
        b_hs[2][2] = {{10,11},{12,13}};
    
    arr_t c = { {{1, 2}, {3, 4}, {5, 6}} }, c_in;
    int c_hs[2][2] = {{11,12},{13,14}};
    
    vector<std::array<int, 2> > d = { {1,2}, {3,4}, {5,6}, {7,8}},
        d_in;
    vector<int> d_hs = {11,12,13,14};

    auto da = _g0.create_dataset_for("a", a),
        db = _g0.create_dataset_for("b", b),
        dc = _g0.create_dataset_for("c", c),
        dd = _g0.create_dataset_for("d", d);

    da.write(a);
    db.write(b);
    dc.write(c);
    dd.write(d);
    {
        da.write_hyperslab(a_hs, { {1}, {3} });
        da.read(a_in);
        EXPECT_THAT(a_in, gt::ElementsAre(1,11,13,15,5));
        da.write_hyperslab(a_hs, { {0}, {3}, {2}, {1} });
        da.read(a_in);
        EXPECT_THAT(a_in, gt::ElementsAre(11,11,13,15,15));
        pout << "da", endl;
    }
    {
        db.write_hyperslab(b_hs, { {0, 1}, {2, 2} });
        db.read(b_in);
        EXPECT_THAT(b_in[0], gt::ElementsAre(1,10,11,4));
        EXPECT_THAT(b_in[1], gt::ElementsAre(5,12,13,8));
        db.write_hyperslab(b_hs, { {0, 0}, {1, 1}, {}, {2, 2} });
        db.read(b_in);
        EXPECT_THAT(b_in[0], gt::ElementsAre(10,11,11,4));
        EXPECT_THAT(b_in[1], gt::ElementsAre(12,13,13,8));
        pout << "db", endl;
    }
    {
        dc.write_hyperslab(c_hs, { {1, 0}, {2, 2} });
        dc.read(c_in);
        EXPECT_THAT(c_in[0], gt::ElementsAre(1,2));
        EXPECT_THAT(c_in[1], gt::ElementsAre(11,12));
        EXPECT_THAT(c_in[2], gt::ElementsAre(13,14));
        dc.write_hyperslab(c_hs, { {0, 0}, {2, 2}, {2, 1}, {} });
        dc.read(c_in);
        EXPECT_THAT(c_in[0], gt::ElementsAre(11,12));
        EXPECT_THAT(c_in[1], gt::ElementsAre(11,12));
        EXPECT_THAT(c_in[2], gt::ElementsAre(13,14));
        pout << "dc", endl;
    }
    {
        dd.write_hyperslab(d_hs, { {1, 0}, {2, 2} });
        dd.read(d_in);
        d[1] = {11,12};
        d[2] = {13,14};
        EXPECT_EQ(d, d_in);
        pout << "dd", endl;
    }
}

TEST_F(DatasetTest, WriteElements) {
    std::array<std::array<int, 4>, 5> a = {
        {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16}, {17,18,19,20}}
    }, a_in;

    auto d = _g0.create_dataset_for("a", a);
    d.write(a);

    vector<hsize_t> pts {0,0, 0,1, 1,1, 1,2, 2,2, 2,3};
    int a_elems[6] = {21,22,23,24,25,26};
    d.write_elements(a_elems, {6, pts.data()});
    d.write_element(100, {4, 1});
    d.write_element(101, {4, 3});

    a[0][0] = 21; a[0][1] = 22;
    a[1][1] = 23; a[1][2] = 24;
    a[2][2] = 25; a[2][3] = 26;
    a[4][1] = 100; a[4][3] = 101;
    d.read(a_in);
    EXPECT_THAT(a_in, gt::ContainerEq(a));
}

TEST_F(DatasetTest, ReadHyperslab) {
    std::array<std::array<int, 4>, 5> a = {
        {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16}, {17,18,19,20}}
    };

    auto d = _g0.create_dataset_for("a", a);
    d.write(a);

    vector<int> a_in;
    vector<std::array<int, 2> > a_in_1;
    d.read_hyperslab(a_in, {{0,0},{2,2}});
    EXPECT_THAT(a_in, gt::ElementsAre(1,2,5,6));

    d.read_hyperslab(a_in_1, {{0,0},{2,2}});
    EXPECT_THAT(a_in_1[0], gt::ElementsAre(1,2));
    EXPECT_THAT(a_in_1[1], gt::ElementsAre(5,6));

    d.read_hyperslab(a_in, {{0,0}, {2,2}, {2,2}, {}});
    EXPECT_THAT(a_in, gt::ElementsAre(1, 3, 9, 11));

    d.read_hyperslab(a_in, {{0,0}, {2,2}, {2,2}, {2,2}});
    EXPECT_THAT(a_in, gt::ElementsAre(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13,14,15,16));
}

TEST_F(DatasetTest, ReadElements) {
    std::array<std::array<int, 4>, 5> a = {
        {{1,2,3,4}, {5,6,7,8}, {9,10,11,12}, {13,14,15,16}, {17,18,19,20}}
    };

    auto d = _g0.create_dataset_for("a", a);
    d.write(a);

    vector<hsize_t> pts {0,0, 0,1, 1,1, 1,2, 2,2, 2,3};
    vector<int> a_in;
    d.read_elements(a_in, {6, pts.data()});
    EXPECT_THAT(a_in, gt::ElementsAre(1,2,6,7,11,12));
    
    int a_in_1, a_in_2;
    d.read_element(a_in_1, {0,0});
    d.read_element(a_in_2, {2,3});
    EXPECT_EQ(a_in_1, 1);
    EXPECT_EQ(a_in_2, 12);
}

TEST_F(DatasetTest, WriteStr) {
    string s1 = "sssss";                        string s1_in;
    const char *s2 = "ssssss";                  char s2_in[7];
    char s21[8] = "sss";                        char s21_in[8];
    string s3[3] = {"ss", "sss", "ssss"};       string s3_in[3];
    vector<string> s4 = {"ss", "sss", "ssss"};  vector<string> s4_in;
    char s5[4][8] = {"s", "ss", "sss", "abc"};  char s5_in[4][8];

    auto d1 = _g0.create_dataset_for_str("s1", s1),
        d2 = _g0.create_dataset_for_str("s2", s2),
        d21 = _g0.create_dataset_for_str("s21", s21),
        d3 = _g0.create_dataset_for_str("s3", s3),
        d4 = _g0.create_dataset_for_str("s4", s4),
        d5 = _g0.create_dataset_for_str("s5", s5);
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


} // namespace
}
} // namespace IO
} // namespace HIPP