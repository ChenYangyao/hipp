#include <hippmpi.h>

namespace HIPP::MPI {

class TestData {
public:
    void init() {
        x = 0;
        y[0] = 1.; y[1] = 2.; y[2] = 3.;
        z = {4.,5.,6.};
        nz = z.size();
    }
    void init_empty(){
        x = -1;
        y[0] = -1.; y[1] = -1.; y[2] = -1.;
        z = {-1.,-1.,-1.};
        nz = z.size();
    }
    void compare(const TestData &o){
        assert(x == o.x);
        for(int i=0; i<3; ++i){
            assert(y[i] == o.y[i]);
        }
        assert(z.size() == o.z.size());
        for(size_t i=0; i<z.size(); ++i){
            assert(z[i] == o.z[i]);
        }
    }

    int x;
    double y[3];
    vector<float> z;
    int nz;
};

} // namespace HIPP::MPI