#include <hippcntl.h>
using namespace HIPP;

int main(int argc, char const *argv[])
{
    using HIPP::pout;

    int a[3] = {1,2,3};
    pout << "a has three elements, a[0]=", a[0], ", a[1]=", a[1], ", a[2]=", a[2], '\n',
            "a can be directly put into stream: ", pout(a, a+3), endl;
    
    vector<double> b = {1.1, 2.1, 3.1};
    std::unordered_map<string, double> c = {{"bar", 10.0}, {"foo", 20.0}, {"baz", 30.0}};
    pout << "b=(", b, "), c={", c, "}", endl;

    std::tuple<string, int, vector<double>> d = {"a string", 1, {2.0,3.0, 4.0}};
    pout << "d=", d, endl;

    return 0;
}
