#include <hippio.h>

namespace H5 = HIPP::IO::H5;
using namespace std;
using namespace HIPP;

void resized_string_datatype() {
    string s = "hello, world";              // String to save into file.

    auto str_dtype = H5::C_S1_T.copy();
    str_dtype.resize( s.size() + 1 );       // Reserve the space for `\0`

    // Equivalently, use resized copy
    str_dtype = H5::C_S1_T.resized( s.size() + 1 );

    H5::File file("str_dtype.h5");
    file.create_dataset("s", str_dtype, H5::Dataspace::vSCALAR).write_str(s);

    // Or, equivalently
    file.create_dataset_str("s", s.size()+1).write_str(s);

    // Or, more simply
    file.create_dataset_for_str("s", s).write_str(s);
}

void compound_creation() {
    struct S {
        int a;
        double b[3];
        float c[2][3];
        array<array<long, 3>, 4> d;
    };

    auto dtype = H5::Datatype::create( H5::Datatype::cCOMPOUND, sizeof(S) );
    
    // Or, equivalently
    dtype = H5::Datatype::create_compound(sizeof(S));

    // Insert the members.
    dtype.insert("a", &S::a);
    dtype.insert("b", &S::b);

    // Or, insert by offset and datatype.
    dtype.insert("c", H5::Datatype::offset(&S::c), 
        H5::Datatype::from_type<float[2][3]>());
    dtype.insert("d", H5::Datatype::offset(&S::d), 
        H5::Datatype::from_type<array<array<long, 3>, 4> >());
    
    // The simplest way - construct the COMPOUND datatype in one call.
    dtype = H5::Datatype::create_compound("a", &S::a, 
        "b", &S::b, 
        "c", &S::c, 
        "d", &S::d);

    // Perform I/O with this new datatype.
    S data[10];
    H5::File file("compound_creation.h5");

    file.create_dataset("s-data", dtype, {10}).write(data, dtype);
}

int main(int argc, char const *argv[]) {
    resized_string_datatype();
    compound_creation();
    return 0;
}
