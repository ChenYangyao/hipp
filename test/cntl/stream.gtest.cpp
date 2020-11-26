#include <hippcntl.h>
#include <gtest/gtest.h>
namespace HIPP{
namespace {

/* PSTream */
class CntlStreamPStreamTest: public ::testing::Test {
protected:
    CntlStreamPStreamTest(): ps(os){}
    ostringstream os;
    PStream ps;
};

TEST_F(CntlStreamPStreamTest, UnaryType){
    ps << 1, 2, 3;
    ASSERT_EQ(os.str(), "123");
}
TEST_F(CntlStreamPStreamTest, MultipleType){
    int a = 1; 
    string b = "hello"; 
    const char *c = "no";
    const int d = 2, e = 3;
    ps << "a=", a, ", b=", b, ", c=", c, ", d=", d, endl,
          "e=", std::setw(4), e;
    ASSERT_EQ(os.str(), 
        "a=1, b=hello, c=no, d=2\ne=   3");
}

/* PrtArray */
class CntlStreamPrtArrayTest: public ::testing::Test {
protected:
    CntlStreamPrtArrayTest(): v{1,2,3,4,5,6,7,8}, pa(v){}
    vector<int> v;
    PrtArray<vector<int>::const_iterator> pa;
    ostringstream os;
};

TEST_F(CntlStreamPrtArrayTest, DefaultFormat){
    pa.prt(os);
    ASSERT_EQ(os.str(), "1,2,3,4,5,6,7,8");
}
TEST_F(CntlStreamPrtArrayTest, UseNcol){
    pa.ncol(3);
    os << pa;
    ASSERT_EQ(os.str(), "1,2,3,\n4,5,6,\n7,8");
}
TEST_F(CntlStreamPrtArrayTest, HaveWidth){
    pa.ncol(3).width(4).prt(os);
    ASSERT_EQ(os.str(), "   1,   2,   3,\n   4,   5,   6,\n   7,   8");
}
TEST_F(CntlStreamPrtArrayTest, HaveOffset){
    pa.ncol(3).width(4).prt(os);
    pa.coloffset( v.size() % 3 );
    pa.prt(os);
    ASSERT_EQ(os.str(), "   1,   2,   3,\n   4,   5,   6,\n   7,   8,   1,\n"
        "   2,   3,   4,\n   5,   6,   7,\n   8");
}

/* prt and prt_X */
class CntlStreamPrtTest: public ::testing::Test {
protected:
    ostringstream os;
};

TEST_F(CntlStreamPrtTest, Prt) {
    int a = 1; 
    string b = "hello";
    long c = 2;
    prt(os, a, b, c) << endl;
    ASSERT_EQ(os.str(), "1hello2\n");
}
TEST_F(CntlStreamPrtTest, PrtF) {
    prt_f(os, "%d%s%s%5.1f", 1, " is ", "not ", 2.1);
    auto s = os.str();
    ASSERT_EQ(s, "1 is not   2.1");
}
TEST_F(CntlStreamPrtTest, PrtA) {
    vector<double> a = {1,2,3,4,5};
    prt_a(os, a);
    auto s = os.str();
    ASSERT_EQ(s, "1,2,3,4,5");
}

/* str and str_X */
TEST(CntlStream, Str){
    int a = 1;
    auto s = str(a, " is ", "not ", 2);
    ASSERT_EQ(s, "1 is not 2");
}
TEST(CntlStream, StrF){
    auto s = str_f("%d%s%s%5.1f", 1, " is ", "not ", 2.1);
    ASSERT_EQ(s, "1 is not   2.1");
}
TEST(CntlStream, StrA){
    vector<double> a = {1,2,3,4,5};
    auto s = str_a(a); 
    ASSERT_EQ(s, "1,2,3,4,5");
}
} // namespace


} // namespace HIPP

