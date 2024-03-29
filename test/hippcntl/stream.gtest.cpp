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

    struct printable_t {
        friend ostream & operator<<(ostream &os, const printable_t &a) {
            return os << "Printable";
        }
    };    

    struct any_t {};

    struct callable_t {
        void operator()(ostream &os) const {
            os << "Callable";
        }
    };

    struct obj_with_info_t {
        ostream & info(ostream &os = cout, int  fmt_cntl = 0, 
            int level = 0) const 
        {
            return os << "fmt_cntl=" << fmt_cntl << ", level=" << level;
        }
    };
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
TEST_F(CntlStreamPStreamTest, ContainerType){
    vector<int> a1 = {1,2,3};

    std::array<int, 3> b1 = {4,5,6};
    std::array<long long, 5> b2 = {4,5,6};

    std::set<int> c = {7,8,9};
    int d[3] = {10,11,12};
    ps << a1, ",\n", 
          b1, ",\n", 
          b2, ",\n", 
          c, ",\n", 
          ps(d,d+3), "\n";

    string target = 
R"*(<vector> {1, 2, 3},
<array> {4, 5, 6},
<array> {4, 5, 6, 0, 0},
<set> {7, 8, 9},
{10, 11, 12}
)*";

    ASSERT_EQ(os.str(), target);
}

TEST_F(CntlStreamPStreamTest, MapType){
    std::map<int, string> m {{1,"foo"}, {2, "bar"}, {3, "baz"}};
    ps << "The map is ", m, endl;
    ASSERT_EQ(os.str(), "The map is <map> {1:foo, 2:bar, 3:baz}\n");
}
TEST_F(CntlStreamPStreamTest, TupleType){
    std::tuple<int, long, string> tpl(1,2,"3s");
    std::pair<int, string> pr(4,"5s");
    std::tuple<int, decltype(tpl), decltype(pr), vector<int> >
        comp(0, tpl, pr, {6, 7, 8});
    ps << "Tpl=", tpl, ", pr=", pr, ", comp=", comp, endl;
    ASSERT_EQ(os.str(), "Tpl=(1, 2, 3s), pr=4:5s, comp=(0, (1, 2, 3s), 4:5s, "
        "<vector> {6, 7, 8})\n");
}


TEST_F(CntlStreamPStreamTest, PrintableType) {
    ps << "printable_t: ", printable_t(), '\n';
    string expected_s = str("printable_t: Printable\n");
    ASSERT_EQ(os.str(), expected_s);
}

TEST_F(CntlStreamPStreamTest, AnyType) {
    any_t any_obj;
    ps << "any_t: ", any_obj, '\n';
    string expected_s = str("any_t: <", typeid(any_obj).name(),"> {", 
        (void *)&any_obj, ", size=", sizeof(any_obj), "}\n");
    ASSERT_EQ(os.str(), expected_s);
}

TEST_F(CntlStreamPStreamTest, CallableType) {
    callable_t cb;
    ps << "callable_t: ", ps(cb), '\n';
    string expected_s = str("callable_t: Callable\n");
    ASSERT_EQ(os.str(), expected_s);
}

TEST_F(CntlStreamPStreamTest, ObjectWithInfoType) {
    obj_with_info_t o;
    ps << "obj_with_info_t: ", ps.info_of(o), '\n',
        ps.info_of(o, 0, 1), '\n',
        ps.info_of(o, 1, 0), '\n',
        ps.info_of(o, 1, 1), endl;
    string expected_s = str(
        "obj_with_info_t: fmt_cntl=0, level=0\n"
        "fmt_cntl=0, level=1\n",
        "fmt_cntl=1, level=0\n",
        "fmt_cntl=1, level=1\n");
    ASSERT_EQ(os.str(), expected_s);
}

/* PLogStream */
class CntlStreamPLogStreamTest: public ::testing::Test {
protected:
    CntlStreamPLogStreamTest(): pls(os){}
    
    ostringstream os;
    PLogStream pls;
};


TEST_F(CntlStreamPLogStreamTest, PlainLog) {
    int x = 1;
    vector<int> v {1,2,3};
    pls << "x=", x, ", v=", v, endl;

    string s = "foo, bar, baz";
    long arr[3] = {-1,-2,-3};
    pls << "s=", s, ", arr=", pls(arr, arr+3), "\n";

    string target = 
R"*(x=1, v=<vector> {1, 2, 3}
s=foo, bar, baz, arr={-1, -2, -3}
)*";
    ASSERT_EQ(os.str(), target);
}

TEST_F(CntlStreamPLogStreamTest, LogWithPushPop) {
    pls << "Outermost environment", endl;
    ASSERT_EQ(pls.stack_height(), 0);
    {
        auto g = pls.push_g("Enter", " main subroutine");
        int x = 1;
        vector<int> v {1,2,3};
        pls << "Begin processing main\n";
        pls << "x=", x, ", v=", v, endl;
        ASSERT_EQ(pls.stack_height(), 1);
        {
            auto g = pls.push_g("Enter sub subroutine");
            g.hint_pop_on();
            string s = "foo, bar, baz";
            long arr[3] = {-1,-2,-3};
            pls << "Begin processing sub\n";
            
            pls << "s=", s, ", arr=", pls(arr, arr+3), "\n";
            pls += "Continuing", endl;
            
            pls << "Another entry ... ";
            pls <= "in the same line ... ";
            pls <= "and line", endl;
            
            pls << "End processing sub", endl;
            ASSERT_EQ(pls.stack_height(), 2);
        }
        pls << "End processing main", endl;
    }
    
    pls << "Return to outermost environment", endl;
    pls += "Continuing", endl;
    
    pls << "Another entry ... ";
    pls <= "in the same line ... ";
    pls <= "and line", endl;

    string target = 
R"*(Outermost environment
Enter main subroutine
  |- Begin processing main
  |- x=1, v=<vector> {1, 2, 3}
  |- Enter sub subroutine
    |- Begin processing sub
    |- s=foo, bar, baz, arr={-1, -2, -3}
       Continuing
    |- Another entry ... in the same line ... and line
    |- End processing sub
    |- Exit (stack=2)
  |- End processing main
Return to outermost environment
Continuing
Another entry ... in the same line ... and line
)*";
    ASSERT_EQ(os.str(), target);
}

TEST_F(CntlStreamPLogStreamTest, ChangePrefixStyle) {
    pls.set_indent(4).set_entry_prefix("-- ");
    pls << "Outermost environment", endl;
    {
        auto g = pls.push_g("Enter", " main subroutine");
        int x = 1;
        vector<int> v {1,2,3};
        pls << "Begin processing main\n";
        pls << "x=", x, ", v=", v, endl;

        {
            auto g = pls.push_g("Enter sub subroutine");
            g.hint_pop_on();
            string s = "foo, bar, baz";
            long arr[3] = {-1,-2,-3};
            pls << "Begin processing sub\n";
            pls << "s=", s, ", arr=", pls(arr, arr+3), "\n";
            pls << "End processing sub", endl;
        }
        pls << "End processing main", endl;
    }
    pls << "Return to outermost environment", endl;

    string target = 
R"*(Outermost environment
Enter main subroutine
    -- Begin processing main
    -- x=1, v=<vector> {1, 2, 3}
    -- Enter sub subroutine
        -- Begin processing sub
        -- s=foo, bar, baz, arr={-1, -2, -3}
        -- End processing sub
        -- Exit (stack=2)
    -- End processing main
Return to outermost environment
)*";
    ASSERT_EQ(os.str(), target);
}

TEST_F(CntlStreamPLogStreamTest, AddFilter) {
    pls.set_level_used(pls.LV_INFO);
    pls << "Outermost environment", endl;
    {
        auto g = pls.push_at(pls.LV_INFO, "Enter", " main subroutine");
        int x = 1;
        vector<int> v {1,2,3};
        pls << "Begin processing main\n";
        pls << "x=", x, ", v=", v, endl;

        {
            auto g = pls.push_at(pls.LV_DEBUG, "Enter sub subroutine");
            g.hint_pop_on();
            string s = "foo, bar, baz";
            long arr[3] = {-1,-2,-3};
            pls << "Begin processing sub\n";
            pls << "s=", s, ", arr=", pls(arr, arr+3), "\n";
            pls << "End processing sub", endl;
        }
        pls << "End processing main", endl;
    }
    pls << "Return to outermost environment", endl;

    string target = 
R"*(Outermost environment
Enter main subroutine
  |- Begin processing main
  |- x=1, v=<vector> {1, 2, 3}
  |- End processing main
Return to outermost environment
)*";
    ASSERT_EQ(os.str(), target);
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
TEST(CntlStreamTest, Str){
    int a = 1;
    auto s = str(a, " is ", "not ", 2);
    ASSERT_EQ(s, "1 is not 2");
}

TEST(CntlStreamTest, StrF){
    auto s = str_f("%d%s%s%5.1f", 1, " is ", "not ", 2.1);
    ASSERT_EQ(s, "1 is not   2.1");

    string fmt = "%d%s%s%5.1f";
    s = str_f(fmt, 1, " is ", "not ", 2.1);
    ASSERT_EQ(s, "1 is not   2.1");
}

TEST(CntlStreamTest, StrA){
    vector<double> a = {1,2,3,4,5};
    auto s = str_a(a); 
    ASSERT_EQ(s, "1,2,3,4,5");
}
} // namespace


} // namespace HIPP

