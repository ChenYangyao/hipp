#include <gtest/gtest.h>
#include <hippio.h>

namespace HIPP {
namespace IO {
namespace {

/* H5Group */
TEST(IOHDF5H5Group, CreateTopLevel) {
    H5File f("HDF5H5GroupCreateTopLevel.h5", "w");
    auto gA = f.create_group("GroupA"),
        gB = f.create_group("GroupB"),
        gRoot = f.open_group("/");
    auto gA2 = f.open_group("GroupA");
}
TEST(IOHDF5H5Group, CreateNested) {
    H5File f("HDF5H5GroupCreateNested.h5", "w");
    auto gA = f.create_group("GroupA"),
        gB = gA.create_group("GroupB"),
        gC = gA.create_group("GroupC"),
        gD = gB.create_group("GroupD");
    auto gA2 = f.open_group("GroupA"),
        gD2 = f.open_group("GroupA/GroupB/GroupD"),
        dD3 = gB.open_group("GroupD");
}

/* H5Dataset */
class IOHDF5H5Dataset: public ::testing::Test {
protected:
    IOHDF5H5Dataset()
    : buff( new double[5] )
    {
        for(int i=0; i<5; ++i) buff[i] = i;
    }
    ~IOHDF5H5Dataset() override {
        delete [] buff;
    }
    void open(const string &TestName) {
        f = H5File("IOHDF5H5Dataset"+TestName+".h5", "w");
        g = f.create_group("base");
    }
    H5File f {nullptr};
    H5Group g {nullptr};
    vector<int> vec {1,2,3,4,5,6,7,8};
    double *buff;
    vector<hsize_t> dimvec = {2, 4}, dimbuff = {5};
};

TEST_F(IOHDF5H5Dataset, WriteToFile) {
    open("WriteToFile");
    f.create_dataset<int>("i24", dimvec).write(vec);
    
    auto i24 = f.open_dataset("i24");
    auto i24_dims = i24.dataspace().dims();
    ASSERT_EQ(i24_dims.size(), 2);
    ASSERT_EQ(i24_dims[0], 2);
    ASSERT_EQ(i24_dims[1], 4);
    ASSERT_EQ(i24.dataspace().size(), 8);

    vector<int> i24_in;
    i24.read(i24_in);
    ASSERT_EQ(i24_in.size(), 8);
    for(size_t i=0; i<i24_in.size(); ++i){
        ASSERT_EQ(i24_in[i], vec[i]);
    }
}

TEST_F(IOHDF5H5Dataset, WriteToGroup) {
    open("WriteToGroup");
    g.create_dataset<double>("d5", dimbuff).write(buff);
    
    auto d5 = g.open_dataset("d5");
    vector<double> d5_in;
    d5.read(d5_in);
    ASSERT_EQ(d5_in.size(), 5);
    for(size_t i=0; i<d5_in.size(); ++i){
        ASSERT_EQ(d5_in[i], buff[i]);
    }
    d5.read(d5_in.data());
    for(size_t i=0; i<d5_in.size(); ++i){
        ASSERT_EQ(d5_in[i], buff[i]);
    }
}


} // namespace
} // namespace IO
} // namespace HIPP