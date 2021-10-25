#include <hippio.h>

using namespace HIPP;
using namespace std;
namespace H5 = HIPP::IO::H5;

int main(int argc, char const *argv[]) {

    H5::File f("file1.h5", "w");

    auto attrs = f.attrs();                     // Get the attribute manager of the file

    short s;
    vector<double> d10(10);
    int i5[5];
    long l34[3][4];
    array<float, 3> f3;
    vector<array<int, 3> > i83(8);

    attrs.put("s",      s);                     // Create a new attribute and write to it
    attrs.put("d10",    d10);
    attrs.put("i5",     i5);
    attrs.put("l34",    l34);
    attrs.put("f3",     f3);
    attrs.put("i83",    i83);

    attrs.get("s",      s);                     // Read back the data.
    attrs.get("d10",    d10);
    attrs.get("i5",     i5);
    attrs.get("l34",    l34);
    attrs.get("f3",     f3);
    attrs.get("i83",    i83);

    auto attr = attrs.open("l34");
    auto dspace = attr.dataspace();
    auto dtype = attr.datatype();
    
    pout << "Dataspace: ndims = ",      dspace.ndims(), 
            ", dims = ",                dspace.dims(), '\n',
            "Datatype: size = ",        dtype.size(), 
            ", class = ",               dtype.get_class(), endl;

    return 0;
}
