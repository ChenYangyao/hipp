#include <hippio.h>

using namespace HIPP;
using namespace std;
namespace H5 = HIPP::IO::H5;

int main(int argc, char const *argv[]) {

    H5::File f("file1.h5", "w");

    H5::DatasetManager dsets = f.datasets();    // Get the dataset manager of the file

    short s;
    vector<double> d10(10);
    int i5[5];
    long l34[3][4];
    array<float, 3> f3;
    vector<array<int, 3> > i83(8);

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

    auto dset = dsets.open("l34");
    auto dspace = dset.dataspace();
    auto dtype = dset.datatype();
    
    pout << "Dataspace: ndims = ",      dspace.ndims(), 
            ", dims = ",                dspace.dims(), '\n',
            "Datatype: size = ",        dtype.size(), 
            ", class = ",               dtype.get_class(), endl;

    return 0;
}
