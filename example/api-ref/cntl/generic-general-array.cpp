#include <hippcntl.h>
using namespace HIPP;
using namespace std;

int main(int argc, char const *argv[]) {

    int scalar {};
    pout << "int: is_array = ", GeneralArrayTraits<decltype(scalar)>::is_array, "\n";

    vector<int> a1(4);
    GeneralArrayTraits tr1{a1};
    pout << "vector<int>(4):\n",
        "  is_array = ",  tr1.is_array,   ", is_const = ",    tr1.is_const, "\n",
        "  buff = ",      tr1.buff(),     ", size = ",        tr1.size(), "\n",
        "  extents = ",   tr1.extents(),  ", strides = ",    tr1.strides(), "\n";

    array<int, 3> a2 {};
    float a3[2];
    GeneralArrayTraits tr2{a2};
    GeneralArrayTraits tr3{a3};
    pout << "array<int,3>: buff = ", tr2.buff(), ", size = ", tr2.size(), "\n";
    pout << "float[2]: buff = ", tr3.buff(), ", size = ", tr3.size(), "\n";

    double a4[2][3] {};
    const array<array<int, 3>, 2> a5{};
    GeneralArrayTraits tr4{a4};
    GeneralArrayTraits tr5{a5};
    pout << "double[2][3]: extents = ", tr4.extents(), 
        ", strides = ", tr4.strides(), "\n";
    pout << "const array<array<int,3>,2>: is_const = ", tr5.is_const, 
        ", extents = ", tr5.extents(), "\n";

    return 0;
}
