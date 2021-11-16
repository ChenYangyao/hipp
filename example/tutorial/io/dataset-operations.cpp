#include <hippio.h>

namespace H5 = HIPP::IO::H5;
using namespace std;
using namespace HIPP;

void using_the_dataset_manager() {
    H5::File f1("f1.h5", "w");

    auto dsets = f1.datasets();

    assert( !dsets.exists("dataset_A") );

    short s;
    vector<double> d10(10);
    int i5[5];
    long l34[3][4];
    array<float, 3> f3;
    vector<array<int, 3> > i83(8);

    for(size_t i=0; i<8; ++i)
        for(size_t j=0; j<3; ++j)
            i83[i][j] = i*3+j;

    dsets.put("s",      s);                     // Create a new dataset and write to it
    dsets.put("d10",    d10);
    dsets.put("i5",     i5);
    dsets.put("l34",    l34);
    dsets.put("f3",     f3);
    dsets.put("i83",    i83);

    dsets.get("s",      s);                     // Read back the data.
    dsets.get("d10",    d10);
    dsets.get("i5",     i5);
    dsets.get("l34",    l34);
    dsets.get("f3",     f3);
    dsets.get("i83",    i83);

    s = dsets.get<short>("s");
    f3 = dsets.get<array<float, 3> >("f3");

    /* Working with string and strings */
    string str = "foo";
    vector<string> str4 = {"top", "bottom", "left", "right"};
    char raw_str[16] = "foo";
    char raw_str4[4][16] = {"top", "bottom", "left", "right"};
    
    dsets.put_str("str", str);
    dsets.put_str("str4", str4);
    dsets.put_str("raw_str", raw_str);
    dsets.put_str("raw_str4", raw_str4);

    dsets.get_str("str", str);
    dsets.get_str("str4", str4);
    dsets.get_str("raw_str", raw_str);
    dsets.get_str("raw_str4", raw_str4);

    str = dsets.get_str<string>("str");
    str4 = dsets.get_str<vector<string> >("str4");

    /* Working with selection */
    int i_value;
    dsets.at("i83", i_value, {1,2});
    pout << i_value, endl;            // => 5

    vector<int> i_values;
    vector<hsize_t> coords = {
        0,0, 
        1,1, 
        2,2};
    dsets.ats("i83", i_values, {3, coords.data()});
    pout << i_values, endl;           // => 0,4,8

    dsets.slab("i83", i_values,  {{1, 0}, {2, 3}});
    pout << i_values, endl;           // => 3,4,5,6,7,8
}

void datatype_operations() {
    /* Predefined */
    const H5::Datatype &dt_f = H5::NATIVE_DOUBLE_T, 
         &dt_i = H5::NATIVE_INT_T;
    assert( dt_f.get_class() == dt_f.cFLOAT );
    assert( dt_i.get_class() == dt_i.cINTEGER );
    pout << "size of float type = ", dt_f.size(), 
            ", size of int type = ", dt_i.size(), endl;

    auto dt_f2 = dt_f;              // Shallow copy, refer to the same datatype.
    auto dt_f3 = dt_f.copy();       // Deep copy, get a new datatype.
    assert( dt_f == dt_f2 && dt_f.equal(dt_f2) );
    assert( dt_f != dt_f3 && dt_f.equal(dt_f3) );

    /* ATOMIC STRING datatype */
    const char s[] = "foo";
    auto dt_s = H5::C_S1_T.resized(10);
    assert( dt_s.get_class() == dt_s.cSTRING );
    pout << "size = ", dt_s.size(), endl;

    /* ARRAY datatype */
    int arr[3][4];
    auto dt_a1 = H5::NATIVE_INT_T.create_array({3,4});
    auto dt_a2 = H5::Datatype::create_array_for< decltype(arr) >();
    pout << "rank = ",   dt_a1.array_ndims(), 
            ", dims = ", dt_a1.array_dims(), endl;

    /* COMPOUND datatype */
    struct S {
        int a;
        float b;
        double c[2][3];
        std::array<int, 2> d;
    } comp;

    auto dt_comp1 = H5::Datatype::create_compound(sizeof(S));
    dt_comp1.insert("a", dt_comp1.offset(&S::a), H5::NATIVE_INT_T)
        .insert("b", &S::b)
        .insert("c", &S::c)
        .insert("d", &S::d);

    auto dt_comp2 = H5::Datatype::create_compound(
        "a", &S::a,     "b", &S::b,
        "c", &S::c,     "d", &S::d);

    pout << 
        "no. members = ",            dt_comp1.nmembers(), '\n',
        "index of a = ",             dt_comp1.member_index("a"), '\n',
        "name of the 2nd member = ", dt_comp1.member_name(1), '\n',
        "3nd member class,offset = ", dt_comp1.member_class(2), ',', 
            dt_comp1.member_offset(2), '\n';

    /* Convert C++ type to HDF5 datatype */
    auto dt_int =     H5::Datatype::from_type<int>(),
        dt_float =    H5::Datatype::from_type<float>(),
        dt_double34 = H5::Datatype::from_type<double[3][4]>();

    vector<int> buff1;
    std::array<int, 3> buff2;
    int buff3[3][4];
    dt_int = H5::Datatype::from_buff(buff1);
    dt_int = H5::Datatype::from_buff(buff2);
    dt_int = H5::Datatype::from_buff(buff3);
}

void dimensions_class() {
    /* Constructions. */
    H5::Dimensions dims1,                   // rank = 0.
        dims2 {3,4},                        // rank = 2, dimensions = {3,4}.
        dims3(vector<size_t>{3,4}),         // rank = 2, dimensions = {3,4}.
        dims4(dims3.data(), dims3.ndims()), // copy from a buffer.
        dims5 = dims4;                      // copy from another Dimensions.

    /* Reconstructions. */
    dims1.set({3,4});                       
    dims1.set(vector<size_t>{3,4});
    dims1.set(dims3.data(), dims3.ndims());
    dims1 = H5::Dimensions {3,4};           // copy assignment.
    
    /* Element Access and Modifications. */
    dims1.push_back(5);                     // {3,4,5}.
    dims1.resize(4); dims1[3] = 6;          // {3,4,5,6}.

    H5::Dimensions::value_t *data = dims1.data();   
    size_t ndims = dims1.ndims();
    vector<H5::Dimensions::value_t> &vec = dims1.as_vector();       // get a vector {3,4,5,6}.
    pout << "dims1 = {", pout(data, data+ndims), "}\n";             // print dims1 = {3,4,5,6}.
    pout << "dims1 = {", pout(dims1.begin(), dims1.end()),"}\n";    // the same.
    pout << dims1, endl;                                            // print the object.


    assert( !dims2.empty() );           // empty check.
    assert( dims2 == dims3 );           // equality check.
    assert( dims2.n_elems() == 3*4 );   // total no. of elements.

    H5::Points::value_t coords[] = { 0,1,  2,3, 4,4 };
    H5::Points pts1,            // 0 points.
        pts2(3, coords),        // 3 points whose coordinates are {0,1}, {2,3}, {4,4}, respectively.
        pts3(3, 2, coords);     // also specify the rank of the space.

    pts1.coords();              // => a pointer to the coords array.
    pts1.n_points();            // => no. of points.
    pts1.n_dims();              // => the rank of the space.

    H5::Hyperslab slab1,                        // rank = 0, empty hyperslab.
        slab2(dims2, {2,3}),                    // rank = 2, start = dims2, count = {2,3}.
        slab3(dims2, {2,3}, {5,10}, {2,2});     // also specify stride = {5,10}, block size = {2,2}.

    H5::Dimensions &start = slab1.start(), 
        &count  = slab1.count(),
        &stride = slab1.stride(),
        &block  = slab1.block();
}

void dataspace_operations() {
    /* Predefined dataspaces. */
    auto & dsp_all = H5::Dataspace::vALL,
         & dsp_null = H5::Dataspace::vNULL,
         & dsp_scalar = H5::Dataspace::vSCALAR;

    /* Simple dataspace. */
    H5::Dataspace dsp1 = {2,3},                     // dims = {2,3}.
        dsp2 ( vector<hsize_t>{2,3} ),              // using vector.
        dsp3 ( H5::Dimensions{2,3} ),               // using Dimensions.
        dsp4 = dsp1.copy();                         // exact copy.

    pout << dsp1, '\n', 
            dsp2, '\n', 
            dsp3, '\n',
            dsp4, endl;
    
    assert( dsp_null.get_type() == dsp_null.cNULL );    // Verify datatype class.
    assert( dsp_scalar.get_type() == dsp_scalar.cSCALAR );
    assert( dsp1.get_type() == dsp1.cSIMPLE );              

    int ndims = dsp1.ndims();                       // => 2
    auto dims = dsp1.dims();                        // => {2,3}
    hsize_t size = dsp1.size();                     // => 6

    pout << "ndims = ", dsp1.ndims(), '\n',
            "dims = ", dsp1.dims(), '\n',
            "size = ", dsp1.size(), '\n';
    
    /* Selection. */
    H5::Dataspace dsp5 {10, 10};
    
    dsp5.select_none();
    pout << "no. selected points = \n";
    pout << "  none -> ", dsp5.get_select_npoints(), '\n';
    
    dsp5.select_all();
    pout << "  all -> ", dsp5.get_select_npoints(), '\n';
    
    dsp5.select_hyperslab({ {2,2}, {3,4} });
    pout << "  hyperslab -> ", dsp5.get_select_npoints(), '\n';

    hsize_t coords[] = {1,1,  2,3,  3,5};
    dsp5.select_elements({3, coords});
    pout << "  elements -> ", dsp5.get_select_npoints(), '\n';

    assert( dsp5.get_select_type() == dsp5.selPOINTS_T );
    assert( dsp5.select_valid() );
}

int main(int argc, char const *argv[])
{
    /*
    using_the_dataset_manager();
    datatype_operations();
    dimensions_class();
    */
    dataspace_operations();
    return 0;
}
