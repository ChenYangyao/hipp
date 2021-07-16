#include <gtest/gtest.h>
#include <hippio.h>

namespace HIPP {
namespace IO {
namespace {

class H5XTableTest: public ::testing::Test { 
protected:
    struct record_t {
        int i;
        double d;
        int i5[5];
        double d34[3][4];
        std::array<int, 5> arr_i5;
        std::array<std::array<double, 4>, 3> arr_d34;
        record_t(int _i, double _d, int _i5, double _d34, 
            int _arr_i5, double _arr_d34)
        : i(_i), d(_d)
        {
            for(int i=0; i<5; ++i){
                i5[i] = _i5 + i;
                arr_i5[i] = _arr_i5 + i;
            }
            for(int i=0; i<3; ++i){
                for(int j=0; j<4; ++j){
                    d34[i][j] = _d34 + i*j;
                    arr_d34[i][j] = _arr_d34 + i*j;
                }
            }
        }
        record_t(){}
    };

    typedef vector<record_t> records_t;

    H5XTable<record_t> io_manip{
        "i", &record_t::i,
        "d", &record_t::d,
        "i5", &record_t::i5,
        "d34", &record_t::d34,
        "arr_i5", &record_t::arr_i5,
        "arr_d34", &record_t::arr_d34
    };

    records_t rs_1 {
        {1,2,3,4,5,6},{10,20,30,40,50,60},{100,200,300,400,500,600},
        {-1,-2,-3,-4,-5,-6}
    };

    void chk_eq(const record_t &r1, const record_t &r2) {
        EXPECT_EQ(r1.i, r2.i);
        EXPECT_EQ(r1.d, r2.d);
        for(int i=0; i<5; ++i){
            EXPECT_EQ(r1.i5[i], r2.i5[i]);
            EXPECT_EQ(r1.arr_i5[i], r2.arr_i5[i]);
        }
        for(int i=0; i<3; ++i){
            for(int j=0; j<4; ++j){
                EXPECT_EQ(r1.d34[i][j], r2.d34[i][j]);
                EXPECT_EQ(r1.arr_d34[i][j], r2.arr_d34[i][j]);
            }
        }
    }
    void chk_eq(const records_t &r1, const records_t &r2) {
        ASSERT_EQ(r1.size(), r2.size());
        for(size_t i=0; i<r1.size(); ++i){
            chk_eq(r1[i],r2[i]);
        }
    }
};

TEST_F(H5XTableTest, RecordIO) {
    string fname = "H5TableTest_RecordIO.h5";
    {
        H5File f(fname, "w");
        io_manip.write_records(rs_1, f, "rs");
    }

    vector<record_t> rs_ld = io_manip.read_records(fname, "rs");
    chk_eq(rs_1, rs_ld);
};

TEST_F(H5XTableTest, SplitDatasetIO) {
    string fname = "H5TableTest_SplitDatasetIO.h5";
    {
        H5File f(fname, "w");
        io_manip.write(rs_1, f.create_group("rs"));
    }
    {
        vector<record_t> rs_ld = io_manip.read(
            H5File(fname, "r").open_group("rs"));
        chk_eq(rs_1, rs_ld);
    }
    {
        vector<record_t> rs_ld;
        io_manip.read(
            rs_ld, H5File(fname, "r").open_group("rs"));
        chk_eq(rs_1, rs_ld);
    }
};

}
}
}