#include <hippcntl.h>
using namespace HIPP;

struct A {};
struct B { 
    friend ostream & operator<<(ostream &os, const B &b) {
        return os << "B object";
    }
};

int main(int argc, char const *argv[])
{
    int a[3] = {1,2,3};
    pout << "a[0]=", a[0], ", a[1]=", a[1], ", a[2]=", a[2], '\n',
            "a = ", pout(a, a+3), endl;
    
    vector<double> b = {1.1, 2.1, 3.1};
    std::unordered_map<string, double> c = {{"bar", 10.0}, {"foo", 20.0}, {"baz", 30.0}};
    std::tuple<string, int, vector<double>> d = {"a string", 1, {2.0,3.0, 4.0}};
    
    pout << "b = (", b, ")\n",
        "c = {", c, "}\n",
        "d = ", d, endl;

    
    pout << "A() = ", A(), '\n',
        "B() = ", B(), endl;

    return 0;
}
