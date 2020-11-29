#include <gtest/gtest.h>
#include <hippio.h>

namespace HIPP {
namespace IO {
namespace {

/* H5Group */
TEST(H5GroupTest, CreateTopLevel) {
    H5File f("H5GroupTest_CreateTopLevel.h5", "w");
    auto gA = f.create_group("GroupA"),
        gB = f.create_group("GroupB"),
        gRoot = f.open_group("/");
    auto gA2 = f.open_group("GroupA");
}
TEST(H5GroupTest, CreateNested) {
    H5File f("H5GroupTest_CreateNested.h5", "w");
    auto gA = f.create_group("GroupA"),
        gB = gA.create_group("GroupB"),
        gC = gA.create_group("GroupC"),
        gD = gB.create_group("GroupD");
    auto gA2 = f.open_group("GroupA"),
        gD2 = f.open_group("GroupA/GroupB/GroupD"),
        dD3 = gB.open_group("GroupD");
}


} // namespace
} // namespace IO
} // namespace HIPP