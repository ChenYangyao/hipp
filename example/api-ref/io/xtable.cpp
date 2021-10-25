#include <hippio.h>

namespace H5 = HIPP::IO::H5;
using namespace HIPP;
using namespace std;


int main(int argc, char const *argv[]) {

    /**
    The structured element type.
    */
    struct S {
        int a;
        double b[3];
        float c[2][3];
        array<array<long, 3>, 4> d;
    };

    /**
    Define a table for I/O.
    */
    H5::XTable xtbl {
        "a",    &S::a,
        "b",    &S::b,
        "c",    &S::c,
        "d",    &S::d};

    /**
    XTable is able to write/read any ContiguousBuffer of records.
    */
    S data[10];
    vector<S> vec_data(10);

    /**
    Write into the root group of a file of given name.
    */
    xtbl.write(data, "xtbl.h5");
    xtbl.write(vec_data, "xtbl.h5");

    /**
    Write to a group.
    */
    H5::File fout("xtbl.h5");
    xtbl.write(data, fout.create_group("S-datasets"));

    /**
    Write as a single dataset with COMPOUND datatype.
    */
    xtbl.write_records(data, fout, "S-records");

    /**  
    Read back the data.
    */
    vector<S> data_in = xtbl.read(fout.open_group("S-datasets"));

    /**
    Define the above XTable using shortcut macro.
    */
    auto xtbl2 = HIPPIO_H5_XTABLE(S, a, b, c, d);
    HIPPIO_H5_XTABLE_DEF(xtbl3, S, a, b, c, d);
    
    return 0;
}


