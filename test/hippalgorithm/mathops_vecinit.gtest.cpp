#include <gtest/gtest.h>
#include <hippalgorithm.h>

namespace HIPP::ALGORITHM {
namespace {

TEST(MathopsLinSpaced, Integers){
    int N = 8;
    LinSpaced lsp(0, N);
    
    vector<int> c(N);
    lsp >> c.begin();
    ASSERT_EQ(c.size(), N);
    for(int i=0; i<N; ++i){
        EXPECT_EQ(c[i], i);
    }

    int step = 10, start = 11;
    int arr[N];
    LinSpaced(start, N, step) >> arr;
    for(int i=0; i<N; ++i){
        EXPECT_EQ(arr[i], start + i*step);
    }
}

TEST(MathopsLinSpaced, Floats){
    int N = 8;
    double start = 0., step = 2.;
    auto v = LinSpaced(start, N, step).get();

    ASSERT_EQ(v.size(), N);
    double cur_val = start;
    for(int i=0; i<N; ++i){
        EXPECT_EQ(cur_val, v[i]);
        cur_val += step;
    }
}

TEST(MathopsLogSpaced, Floats){
    int N = 11;
    double start = 0., step = 0.2;
    auto v = LogSpaced(start, N, step).get();

    ASSERT_EQ(v.size(), N);
    double cur_val = std::pow(10.0, start), inc = std::pow(10.0, step);
    for(int i=0; i<N; ++i){
        EXPECT_EQ(cur_val, v[i]);
        cur_val *= inc;
    }
}

} // namespace
} // namespace HIPP::ALGORITHM

