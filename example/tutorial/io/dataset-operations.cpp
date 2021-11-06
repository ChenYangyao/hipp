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

    dsets.slab("i83", i_values, {{1, 0}, {2, 3}});
    pout << i_values, endl;           // => 3,4,5,6,7,8
}

void dataset_creation_and_opening() {

}

int main(int argc, char const *argv[])
{
    using_the_dataset_manager();
    return 0;
}
