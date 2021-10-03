#include <hippnumerical.h>

using namespace HIPP;
using namespace HIPP::NUMERICAL;
using namespace std;

void define_variables() {
    pout << emFF, endl;

    SArray<double, 3, 4> a34;           // An array shaped [3][4].

    SArray<double, 3> a3;               // An vector shaped [3].
    
    SVec<double, 3> x3;                 // 3 double values.
    SVecXd<3> y3;                       
    SVec3d z3;                         

    SVec<int, 4> x4;                    // 4 integers.
    SVecXi<4> y4;                       
    SVec4i z4;          
    
    bool same1 = is_same_v<decltype(x3), decltype(y3)>,
        same2 = is_same_v<decltype(x3), decltype(z3)>,
        same3 = is_same_v<decltype(a3), decltype(z3)>;
    assert(same1);
    assert(same2);
    assert(same3);
}

void init_variables() {
    pout << emFF, endl;

    SVec3d a3 = {771,2,3}, b3 = {1,2};        // b3[2] is uninitialized.
    pout << "a3 = ", a3, ", b3 = ", b3, endl;

    double x[3] = {4,5,6};
    SVec3d c3(1),                           // Constant array, {1,1,1}.
        d3(x),                              // Copy from a pointer x.
        e3(x, 2),                           // Copy only 2 elements from x.
        f3(a3);                             // Copy from another SArray.
    pout << "c3 = ", c3, ", d3 = ", d3, ", e3 = ", e3, ", f3 = ", f3, endl;
    
    SArray<int, 2, 3> 
        a23 = {1,2,3,4,5,6},  // Row-majored filling.
        b23(1),               // Constant array.
        c23(&a23[0]),         // Copy from a contiguous buffer.
        d23(&a23[0], 3),      // Copy only the first 3 elements.
        e23(a23);             // Copy from another SArray.
    pout << "a23 = ", a23, ", b23 = ", b23, '\n',
        "c23 = ", c23, ", d23 = ", d23, '\n',
        "e23 = ", e23, endl;

    a3 = c3;                                // Assign from another SArray.
    b3 = 1.0;                               // Assign a scalar to all elements.
    pout << "a3 = ", a3, ", b3 = ", b3, endl;

}

void visit_element() {
    /* Tuple-like API for structured binding. */
    pout << emFF, endl;
    SVec3i a3 = {1,2,3};
    
    auto [x0,x1,x2] = a3;                   // Use structured binding.
    pout << "x0,x1,x2 = ", x0, ",", x1, ",", x2, endl;

    auto &[y0,y1,y2] = a3;                  // Bind as reference.
    y0 += 1; y1 += 2; y2 += 3;  
    pout << "a3 = ", a3, endl;

    SArray<int, 2, 3> a23 = {1,2,3,4,5,6};
    auto [z0, z1] = a23;                    // Bind to the 1st and 2nd rows.
    pout << "z0 = ", z0, ", z1 = ", z1, endl;

    auto &[w0, w1] = a23;
    w0 += a3*10;
    pout << "a23 = ", a23, endl;

    /* Element-wise access. */
    int *p3 = a3.data();
    int (& a3_raw)[3] = a3.raw();
    for(size_t i=0; i<a3.size(); ++i) {
        p3[i] = i;                          // Visit the buffer by pointer.
        a3_raw[i] += 1;                     // Visit by the raw array reference.
    }    
    for(auto it = a3.begin(); it != a3.end(); ++it)
        *it += 1;                           // Visit by the iterator.

    a3[0] = -1;                             // Visit by operator [] indexing.
    a3.at(1) = -2;                          // Visit by at().
    pout << "a3 = ", a3, endl;

    int *p23 = a23.data();
    int (&a23_raw)[2][3] = a23.raw();
    p23[0] = -1;                // (0,0) element.
    a23_raw[0][1] = -2;         // (0,1) element.
    
    a23[2] = -3;                // (0,2) element.
    a23.at(3) = -4;             // (1,0) element.
    
    *(a23.begin() + 4) = -5;    // (1,1) element.
    a23(1,2) = -6;              // (1,2) element.
    pout << "a23 = ", a23, endl;

    /* For 1D SArray. */
    vector<int> vec = a3.to_vector();
    array<int, 3> arr = a3.to_array();
    array<double, 3> arr_d = a3.to_array<double>();     // Change the target type.

    pout << "vec = {", vec, "}, arr = {", arr, "}, arr<double> = {", arr_d, "}", endl; 
}

void arithmetic_logic() {
    pout << '\n', emFF, endl;

    SVec3d a = {0,1,2}, b = {3,4,5};
    pout << "a+1, a-1, a+b, a-b, a*b+10 => ",
        a+1, ',', a-1, ',', a+b, ',', a-b, ',', a*b+10, endl;

    a += b;
    b += 1;
    pout << "a => ", a, ", b => ", b, endl;

    pout << "a==b, a!=b, a>0 => ",
        a==b, ',', a!=b, ',', a>0, endl;
}

void reduction() {
    pout << emFF, endl;

    SVec3d a = {0,1,2};
    pout << "Norm = ", a.norm(), 
        ", squared norm = ", a.squared_norm(), 
        ", 1 norm = ", a.norm(1), endl;

    auto b = a.normalized();            // A normalized copy.
    a.normalize();                      // In-place normalization.
    auto c = a.normalized(1);           // Use 1 norm.
    pout << "a = ", a, ", b = ", b, ", c = ", c, endl;

    pout << "sum = ", a.sum(), 
        ", prod = ", a.prod(), 
        ", mean = ", a.mean(), 
        ", min = ", a.min(), 
        ", max = ", a.max(), 
        ", min and max = ", a.minmax(), 
        ", index of min = ", a.min_index(), endl;

    pout << "any true = ", a.any(), 
        ", all true = ", a.all(), 
        ", any positive = ", (a>0).any(), 
        ", any equal = ", (a==b).any(), endl;

    pout << "a dot b = ", a.dot(b),
        ", a cross b = ", a.cross(b), endl;
}

void apply_function_to_elements() {
    pout << emFF, endl;

    SVec3d a = {-1.1, 1.1, 2.1};
    pout << "floor = ", a.floor(), 
        ", ceil = ", a.ceil(),
        ", trunc = ", a.trunc(), 
        ", abs = ", a.abs(), endl;

    auto sin_a = a.mapped( [](auto x){ return sin(x); } );   // A mapped copy
    a.map( [](double x) {return x+1.0; } );                  // In-place change
    pout << "sin_a = ", sin_a, 
        ", a = ", a, endl;

    // Visit each element by index and value
    a.visit( [](size_t i, auto x){ pout << '(', i, ')', x, '\n'; } );
}

void view() {
    pout << emFF, endl;

    SVec3d a = {-1, -2, -3};
    auto pos_a = a[ a > 0 ];                // A view of all positive elements.

    SVec3b mask = {true, true, false};
    auto first_2 = a[mask];                 // A view of the first 2 elements.

    first_2 += 2;
    pout << a, endl;

    SVec<double, 5> b = {1,2,3,4,5};
    auto slice_b = b.view(b.s_stride, b.s_range(0,5,2));        // A view of the indexing range [0,5) with stride 2.
    slice_b += 10;
    pout << b, endl;

    SArray<int, 3, 3> c = {1,2,3,4,5,6,7,8,9};
    auto slice_c = c.view(c.s_stride, c.s_head(2), c.s_all);    // The first two rows
    slice_c *= -1;
    pout << c, endl;
}

int main(int argc, char const *argv[]) {
    define_variables();
    init_variables();

    visit_element();
    arithmetic_logic();
    reduction();

    apply_function_to_elements();
    
    view();
    return 0;
}
