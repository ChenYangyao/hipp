#include <hippio.h>

using namespace HIPP;
using namespace std;
namespace H5 = HIPP::IO::H5;

int main(int argc, char const *argv[]) {

    struct S {
        int a;
        double b[3];
        float c[2][3];
        array<array<long, 3>, 4> d;
    } data[10];

    auto xtbl = HIPPIO_H5_XTABLE(S, a, b, c, d);

    /**
    Write operations.
    */    
    xtbl.write(data, "file1.h5");                   // Write to a file named "file1.h5".

    H5::File f("file1.h5", "a");
    xtbl.write(data, f.create_group("S1"));         // Write to a group.
    
    vector<S> vec_data(10);
    xtbl.write(vec_data, f.create_group("S2"));     // Write a vector of records.
    xtbl.write(vec_data.data(), 10, f.create_group("S3"));                      
                                                    // Write a raw buffer of records.
    
    /**
    Read operations.
    */
    vector<S> in_data = xtbl.read("file1.h5");      // Read from a file named "file1.h5".
    in_data = xtbl.read(f.open_group("S1"));        // Read from a group.
    xtbl.read(vec_data, f.open_group("S1"));        // Read into an existing vector.
    
    /**
    I/O with a single dataset with COMPOUND datatype.
    */
    xtbl.write_records(vec_data, f, "S1-records");  // Write to a dataset "S1-records" in f.
    in_data = xtbl.read_records(f, "S1-records");   // Read back.

    xtbl.select_rows(3, 5);                         // Select rows [3, 3+5) for I/O.
    in_data = xtbl.read("file1.h5");        
    pout << "n_rows loaded = ", in_data.size(), endl;

    xtbl.select_fields({"a", "b"});                 // Select fields "a" and "b" for I/O.
    in_data = xtbl.read("file1.h5");               

    xtbl.select_all().select_all_fields();          // Clear the selection.

    return 0;
}
