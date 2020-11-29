#include <gtest/gtest.h>
#include <hippio.h>

namespace HIPP {
namespace IO {
namespace {

/* H5Dataset */
class H5DatasetTest: public ::testing::Test {
protected:
    H5DatasetTest()
    : buff( new double[5] )
    {
        for(int i=0; i<5; ++i) buff[i] = i;
    }
    ~H5DatasetTest() override {
        delete [] buff;
    }
    void open(const string &TestName) {
        f = H5File("H5DatasetTest_"+TestName+".h5", "w");
        g = f.create_group("base");
    }
    H5File f {nullptr};
    H5Group g {nullptr};
    vector<int> vec {1,2,3,4,5,6,7,8};
    double *buff;
    vector<hsize_t> dimvec = {2, 4}, dimbuff = {5};
};

TEST_F(H5DatasetTest, WriteToFile) {
    open("WriteToFile");
    f.create_dataset<int>("i24", dimvec).write(vec);
    
    auto i24 = f.open_dataset("i24");
    auto i24_dims = i24.dataspace().dims();
    ASSERT_EQ(i24_dims.size(), 2);
    EXPECT_EQ(i24_dims[0], 2);
    EXPECT_EQ(i24_dims[1], 4);
    EXPECT_EQ(i24.dataspace().size(), 8);

    vector<int> i24_in;
    i24.read(i24_in);
    ASSERT_EQ(i24_in.size(), 8);
    for(size_t i=0; i<i24_in.size(); ++i){
        EXPECT_EQ(i24_in[i], vec[i]);
    }
}

TEST_F(H5DatasetTest, WriteToGroup) {
    open("WriteToGroup");
    g.create_dataset<double>("d5", dimbuff).write(buff);
    
    auto d5 = g.open_dataset("d5");
    vector<double> d5_in;
    d5.read(d5_in);
    ASSERT_EQ(d5_in.size(), 5);
    for(size_t i=0; i<d5_in.size(); ++i){
        EXPECT_EQ(d5_in[i], buff[i]);
    }
    d5.read(d5_in.data());
    for(size_t i=0; i<d5_in.size(); ++i){
        EXPECT_EQ(d5_in[i], buff[i]);
    }
}


} // namespace
} // namespace IO
} // namespace HIPP