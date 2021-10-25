#include <h5_test_incl.h>

namespace HIPP::IO::H5 {
namespace {

class DatapacketTest : public gt::Test {

};

TEST_F(DatapacketTest, Scalar) {
    int a;
    double b;
    long c;

    {
        auto [p, dsp, dt] = Datapacket {a};
        EXPECT_EQ(p, &a);
        EXPECT_TRUE(dsp = Dataspace::vSCALAR);
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(a));
    }
    {
        auto [p, dsp, dt] = Datapacket {b};
        EXPECT_EQ(p, &b);
        EXPECT_TRUE(dsp = Dataspace::vSCALAR);
        EXPECT_EQ(dt.get_class(), dt.cFLOAT);
        EXPECT_EQ(dt.size(), sizeof(b));
    }
    {
        auto [p, dsp, dt] = Datapacket {c};
        EXPECT_EQ(p, &c);
        EXPECT_TRUE(dsp = Dataspace::vSCALAR);
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(c));
    }
}

TEST_F(DatapacketTest, GenArrayOfScalar) {
    vector<double> a(5);
    long b[6][3];
    std::array<std::array<int, 2>, 4> c;

    {
        auto [p, dsp, dt] = Datapacket {a};
        EXPECT_EQ(p, a.data());
        EXPECT_EQ(dsp.get_type(), dsp.obj_raw().cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(5));
        EXPECT_EQ(dt.get_class(), dt.cFLOAT);
        EXPECT_EQ(dt.size(), sizeof(double));
    }
    {
        auto [p, dsp, dt] = Datapacket {b};
        EXPECT_EQ(p, &b[0][0]);
        EXPECT_EQ(dsp.get_type(), dsp.obj_raw().cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(6,3));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(long));
    }
    {
        auto [p, dsp, dt] = Datapacket {c};
        EXPECT_EQ(p, &c[0][0]);
        EXPECT_EQ(dsp.get_type(), dsp.obj_raw().cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(4, 2));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
    }
}

TEST_F(DatapacketTest, GenArrayOfRawArray) {
    vector< std::array<int, 3> > a(5);
    vector< std::array<std::array<long, 3>, 4> > b(6);

    {
        auto [p, dsp, dt] = Datapacket {a};
        EXPECT_EQ(p, a.data());
        EXPECT_EQ(dsp.get_type(), dsp.obj_raw().cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(5,3));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(int));
    }

    {
        auto [p, dsp, dt] = Datapacket {b};
        EXPECT_EQ(p, b.data());
        EXPECT_EQ(dsp.get_type(), dsp.obj_raw().cSIMPLE);
        EXPECT_THAT(dsp.dims().as_vector(), gt::ElementsAre(6, 4, 3));
        EXPECT_EQ(dt.get_class(), dt.cINTEGER);
        EXPECT_EQ(dt.size(), sizeof(long));
    }
}


} // namespace
} // namespace HIPP::IO::H5