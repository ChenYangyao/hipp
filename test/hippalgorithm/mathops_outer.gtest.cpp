#include <gtest/gtest.h>
#include <hippalgorithm.h>

namespace HIPP::ALGORITHM {
namespace {

/* OuterProduct */
class MathopsOuterProductTest: public ::testing::Test {
protected:
    MathopsOuterProductTest(){}
    ~MathopsOuterProductTest() override {}
    
    template<typename It>
    size_t n_outer(It b, It e) {
        size_t n = 1;
        while( b != e ) n *= size_t(*b++);
        return n;
    }
    template<typename C>
    size_t n_outer(const C &c){
        return n_outer(std::begin(c), std::end(c));
    }
};

TEST_F(MathopsOuterProductTest, IndicesInt){
    vector<int> maxs = {2,3,4};
    size_t n_dims = maxs.size();
    
    /* total number of output vectors */
    size_t n_out = n_outer(maxs), cnt_out = 0;
    OuterProduct::indices(maxs.begin(), maxs.end(), 
        [&cnt_out](int *p)->void{ ++cnt_out; });
    EXPECT_EQ(cnt_out, n_out);

    /* appearance of each element */
    vector<std::set<int> > s(n_dims);
    OuterProduct::indices(maxs.begin(), maxs.end(),
        [n_dims, &s](int *p)->void{
            for(size_t i=0; i<n_dims; ++i){
                s[i].emplace(p[i]);
            }
        });
    for(size_t i=0; i<n_dims; ++i){
        auto &si = s[i];
        EXPECT_EQ(si.size(), maxs[i]);
        int val = 0;
        for(auto j: si){
            EXPECT_EQ(j, val);
            ++val;
        }
    }

    /* strict order */
    vector<int> last_v (n_dims, -1);
    OuterProduct::indices(maxs.begin(), maxs.end(), 
        [n_dims, &last_v](int *p)->void{
            bool is_larger = false;
            for(size_t i=0; i<n_dims; ++i){
                if( p[i] > last_v[i] ){
                    is_larger = true; break;
                }
            }
            EXPECT_TRUE(is_larger);
            std::copy(p, p+n_dims, last_v.data());
        });
}

} // namespace
} // namespace HIPP::ALGORITHM

