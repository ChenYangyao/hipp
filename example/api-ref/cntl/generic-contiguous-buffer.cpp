#include <hippcntl.h>
using namespace HIPP;

int main(int argc, char const *argv[]) {

    std::vector<int> a1(4);
    const std::array<double, 4> a2 {};
    float a3[4] {};
    long *a4 = new long [4];                // raw memory buffer
    
    ContiguousBuffer<int> b11 {a1};
    ContiguousBuffer<const int> b12{a1};    // cast to const buffer

    ContiguousBuffer<const double> b21{a2};         
    ContiguousBuffer b22 {a2};              // auto deduced to <const double>
    // ContiguousBuffer<double> b23 {a2};   // ERROR, compile failure - cannot cast const to non-const

    ContiguousBuffer<float> b3 {a3};               
    ContiguousBuffer<long> b4 {a4, 4}; 

    pout << "b11 = ", b11, "\nb12 = ", b12, "\nn21 = ", b21,
        "\nb22 = ", b22, "\nb3 = ", b3, "\nb4 = ", b4, endl;

    auto [p1, n1] = b11;                    // Get buffer address and size using structured binding,
    
    const double *p2 = b21.buff;            // or by visiting members.
    size_t n2 = b21.size;                   
    
    auto beg3 = b3.begin(), end3 = b3.end();  // Get iterators.
    
    pout << "contents in a1 = {", pout(p1, p1+n1), 
        "}, a2 = {", pout(p2, p2+n2), 
        "}, a3 = {", pout(beg3, end3), "}\n";
    
    delete [] a4;
    
    return 0;
}
