#include <hippcntl.h>
#include <gtest/gtest.h>
namespace HIPP {
namespace {

class TickerTest: public ::testing::Test {
protected:
    TickerTest(){}
    ~TickerTest() override {}
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(TickerTest, TimePointAndDuration){
    Ticker tk;
    auto t0 = tk.last_time_point();
    double dt = tk.duration();
    auto t1 = tk.last_time_point();
    double dt_exp = std::chrono::duration_cast<Ticker::dur_t>(t1 - t0).count();
    
    EXPECT_EQ(dt, dt_exp);
}

TEST_F(TickerTest, Recording){
    Ticker tk;
    tk.tick(0);
    tk.tick(1);
    tk.tick(1);
    tk.tick("Test");
    
    EXPECT_EQ(tk.query_all().size(), 3);
    vector<string> msgs = {"", "", "Test"};
    for(int i=0; i<3; ++i){
        EXPECT_EQ( msgs[i], tk.query(i).rec_str );
        EXPECT_GE( tk.query(i).dur.count(), 0. );
    }
}

}
}