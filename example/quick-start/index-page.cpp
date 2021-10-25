#include <hippio.h>

namespace H5 = HIPP::IO::H5;
using namespace HIPP;
using namespace std;

int main(int argc, char const *argv[])
{
    struct S {
        int a;
        double b[3];
        float c[2][3];
        array<array<long, 3>, 4> d;
    } data[10];
    
    HIPPIO_H5_XTABLE(S, a, b, c, d).write(data, "file.h5");

    short s;
    vector<double> d10(10);
    int i5[5];
    long l34[3][4];
    std::array<float, 3> f3;

    H5::DatasetManager dsets = H5::File("file1.h5", "w").datasets();
    
    dsets.put("s", s);
    dsets.put("d10", d10);
    dsets.put("i5", i5);
    dsets.put("l34", l34);
    dsets.put("f3", f3);

    return 0;
}
