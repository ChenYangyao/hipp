#include <hippio.h>

using namespace HIPP;
using namespace std;
namespace H5 = HIPP::IO::H5;

int main(int argc, char *argv[]) {
    double d34[3][4] = {};
    vector<int> i10(10);
    string s = "Hello HIPP!";
    
    H5::File fout("data.h5", "w");
    
    auto dsets = fout.datasets();
    dsets.put("d34", d34);
    dsets.put("i10", i10);
    dsets.put_str("s", s);

    dsets.get("d34", d34);
    dsets.get("i10", i10);
    dsets.get_str("s", s);

    pout << "d34 = {", pout(d34[0], d34[0]+12), "}\n",
            "i10 = {", i10, "}\n",
            "s = ", s, "\n";

    return 0;
}
