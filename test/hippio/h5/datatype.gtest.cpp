#include <gtest/gtest.h>
#include <hippio.h>

namespace HIPP {
namespace IO {
namespace H5 {
namespace {


/* Datatype */
class DatatypeTest: public ::testing::Test {
protected:
    typedef Datatype h5d;
    struct compound_t {
        int a;
        double b[3];
        float c;
        short d[2][3][4];
        long e[2][3][4];
    };

    vector<compound_t> comps;
    File f {nullptr};

    DatatypeTest(): comps(5)
    {
        for(size_t i=0; i<5; ++i){
            auto &e = comps[i];
            e.a = int(i);
            e.c = float(i);
            for(size_t j=0; j<3; ++j) 
                e.b[j] = double(j)+0.1;
            for(size_t j=0; j<2; ++j)
                for(size_t k=0; k<3; ++k)
                    for(size_t r=0; r<4; ++r){
                        e.d[j][k][r] = short(i)*1000+j*100+k*10+r;
                        e.e[j][k][r] = short(i)*1000+j*100+k*10+r;
                    }
        }
    }
    ~DatatypeTest() override {}
    void SetUp() override {}
    void TearDown() override {}

    void open(const string &TestName) {
        f = File("DatatypeTest_"+TestName+".h5", "w");
    }
    void chk_comp( const compound_t &e1, const compound_t &e2 ){
        EXPECT_EQ(e1.a, e2.a);
        for(size_t j=0; j<3; ++j)
            EXPECT_EQ(e1.b[j], e2.b[j]);
        for(size_t j=0; j<2; ++j)
            for(size_t k=0; k<3; ++k)
                for(size_t r=0; r<4; ++r){
                    EXPECT_EQ(e1.d[j][k][r], e2.d[j][k][r]);
                    EXPECT_EQ(e1.e[j][k][r], e2.e[j][k][r]);
                }
    }
    XTable<compound_t> get_xtbl() {
        XTable<compound_t> tbl_manip {
            "a", &compound_t::a, 
            "b", &compound_t::b, 
            "d", &compound_t::d, 
            "DataItemE", &compound_t::e
        };
        return tbl_manip;
    }
};

TEST_F(DatatypeTest, CompoundInsert){
    auto dtype = h5d::create(h5d::cCOMPOUND, sizeof(compound_t));
    dtype.insert("a", h5d::offset(&compound_t::a), NATIVE_INT_T)
        .insert("b", &compound_t::b)
        .insert("d", h5d::offset(&compound_t::d), NATIVE_SHORT_T)
        .insert("DataItemE", &compound_t::e);

    open("CompoundInsert");
    f.create_dataset("comps", dtype, {comps.size()})
        .write(comps.data(), dtype);

    auto d = f.open_dataset("comps");
    ASSERT_EQ(d.dataspace().ndims(), 1);
    ASSERT_EQ(d.dataspace().dims()[0], comps.size());
    vector<compound_t> comps_in(comps.size());
    d.read(comps_in.data(), dtype);
    for(size_t i=0; i<comps.size(); ++i)
        chk_comp(comps_in[i], comps[i]);
}

TEST_F(DatatypeTest, CompoundConvenient){
    auto dtype = h5d::create_compound(
        "a", &compound_t::a, 
        "b", &compound_t::b, 
        "d", &compound_t::d, 
        "DataItemE", &compound_t::e);
    open("CompoundConvenient");
    f.create_dataset("comps", dtype, {comps.size()})
        .write(comps.data(), dtype);

    auto d = f.open_dataset("comps");
    ASSERT_EQ(d.dataspace().ndims(), 1);
    ASSERT_EQ(d.dataspace().dims()[0], comps.size());
    vector<compound_t> comps_in(comps.size());
    d.read(comps_in.data(), dtype);
    for(size_t i=0; i<comps.size(); ++i)
        chk_comp(comps_in[i], comps[i]);
}

/* Data records I/O using XTable */
TEST_F(DatatypeTest, CompoundXTableWR){
    auto tbl_manip = get_xtbl();
    open("CompoundXTableWR");
    tbl_manip.write(comps, f);
    
    vector<compound_t> comps_in = 
        tbl_manip.read(f.open_group("/"));

    ASSERT_EQ(comps_in.size(), comps.size());
    for(size_t i=0; i<comps.size(); ++i)
        chk_comp(comps_in[i], comps[i]);
}

TEST_F(DatatypeTest, CompoundXTableWRRecords){
    auto tbl_manip = get_xtbl();
    open("CompoundXTableWRRecords");
    tbl_manip.write_records(comps, f, "comps");
    
    vector<compound_t> comps_in = 
        tbl_manip.read_records(f.open_group("/"), "comps");

    ASSERT_EQ(comps_in.size(), comps.size());
    for(size_t i=0; i<comps.size(); ++i)
        chk_comp(comps_in[i], comps[i]);
}

} // namespace
}
} // namespace IO
} // namespace HIPP