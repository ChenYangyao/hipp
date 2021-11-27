#include <hippio.h>
namespace H5 = HIPP::IO::H5;
using namespace std;
using namespace HIPP;

void using_the_attr_manager() {
    H5::File f1("f1.h5", "w");
    auto attrs =  f1.attrs();

    assert( !attrs.exists("attr_A") );

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

    s = attrs.get<short>("s");
    f3 = attrs.get<array<float, 3> >("f3");

    string str = "foo";
    vector<string> str4 = {"top", "bottom", "left", "right"};
    char raw_str[16] = "foo";
    char raw_str4[4][16] = {"top", "bottom", "left", "right"};

    attrs.put_str("str", str);
    attrs.put_str("str4", str4);
    attrs.put_str("raw_str", raw_str);
    attrs.put_str("raw_str4", raw_str4);

    attrs.get_str("str", str);
    attrs.get_str("str4", str4);
    attrs.get_str("raw_str", raw_str);
    attrs.get_str("raw_str4", raw_str4);

    str = attrs.get_str<string>("str");
    str4 = attrs.get_str<vector<string> >("str4");
}

void attr_creation_and_opening() {
    /* Creation and opening. */
    short s;
    vector<double> d10(10);
    int i5[5];

    {
        H5::File f1("f1.h5", "w");
        f1.create_attr_for("s",   s);
        f1.create_attr_for("d10", d10);
        f1.create_attr_for("i5",  i5);
    }

    {
        H5::File f1("f1.h5", "w");
        f1.create_attr<short>("s",    H5::Dataspace::vSCALAR);
        f1.create_attr<double>("d10", {10});
        f1.create_attr<int>("i5",     {5});
    }

    {
        H5::File f1("f1.h5", "w");
        f1.create_attr("s",   H5::NATIVE_SHORT_T,  H5::Dataspace::vSCALAR);
        f1.create_attr("d10", H5::NATIVE_DOUBLE_T, {10});
        f1.create_attr("i5",  H5::NATIVE_INT_T,    {5});
        
        auto attr_s = f1.open_attr("s");
    }

}

void read_write_data() {
    short s;
    vector<double> d10(10);
    int i5[5];

    H5::File f1("f1.h5", "w");
    auto attr_s =   f1.create_attr_for("s",   s),
         attr_d10 = f1.create_attr_for("d10", d10),
         attr_i5 =  f1.create_attr_for("i5",  i5);

    attr_s.write(s);
    attr_d10.write(d10);
    attr_i5.write(i5);

    attr_s.read(s);
    attr_d10.read(d10);
    attr_i5.read(i5);

    attr_d10.write(d10.data(), H5::NATIVE_DOUBLE_T);
}

int main(int argc, char const *argv[]) {

    using_the_attr_manager();
    attr_creation_and_opening();
    read_write_data();

    return 0;
}
