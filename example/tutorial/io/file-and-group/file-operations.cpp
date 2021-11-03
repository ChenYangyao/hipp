#include <hippio.h>

namespace H5 = HIPP::IO::H5;
using namespace std;
using namespace HIPP;

void file_creation_and_opening() {
    
    // File creation
    H5::File f1 ("f1.h5", "w");     // Old file gets truncated.
    //H5::File f11("f1.h5", "x");   // Will fail because existing.
    H5::File f12("f1.h5", "ac");    // Create, or open if existing

    // File opening
    H5::File f14("f1.h5", "r");     // Read-only.
    H5::File f15("f1.h5", "a");     // Read-and-write.

    H5::File f16;                   // An empty file object.
    assert( ! f16.has_referenced() );

    f16 = H5::File("f1.h5", "a");   // Assign to another file.

    f16.free();                     // Again becomes empty.
    if( f16 ) 
        pout << "Non-empty\n"; 
    else 
        pout << "Empty\n";
}

void group_creation_and_opening() {
    H5::File f1 ("f1.h5", "w");

    auto g1 = f1.create_group("g1", "x");       // Create /g1.
    // auto g11 = f1.create_group("g1", "x");   // Will fail because existing.
    auto g12 = f1.create_group("g1", "ac");     // Create, or open if existing

    auto g2 = g1.create_group("g2", "ac");      // Create /g1/g2.

    auto g13 = f1.open_group("g1");             // Open /g1.
    auto g21 = f1.open_group("g1/g2");          // Open /g1/g2.
    g21 = g1.open_group("g2");                  // Open /g1/g2 via g1.  

}

void link_creation() {
    H5::File f1 ("f1.h5", "w");

    auto g1 = f1.create_group("g1"), 
         g2 = f1.create_group("g2");
    auto d1 = f1.create_dataset_scalar<double>("d1"),
         d2 = g1.create_dataset_scalar<int>("d2");

    g2.create_hard_link("d2", g1, "d2");
    f1.create_soft_link("d2", "/g1/d2");
    f1.create_external_link("d2-ext", "f1.h5", "/g1/d2");

    size_t nlinks_f1 = f1.get_info().nlinks,
        nlinks_g1 = g1.get_info().nlinks,
        nlinks_g2 = g2.get_info().nlinks;

    // See link counts.
    pout << "nlinks of f1,g1,g2 =", 
        nlinks_f1, ',', nlinks_g1, ',', nlinks_g2, endl;

    // Get link names.
    vector<string> names(nlinks_f1);
    for(size_t i=0; i<nlinks_f1; ++i)
        names[i] = f1.get_link_name(".", i);
    pout << "links in f1 = {", names, "}\n";
    
    // Verify the link types.
    assert( f1.get_link_info("d1").type == g1.tHARD );
    assert( f1.get_link_info("d2").type == g1.tSOFT );
    assert( f1.get_link_info("d2-ext").type == g1.tEXTERNAL );

    // Verify the link resolves to an object an retrieve its object type.
    for( auto path: {"d1", "d2", "d2-ext", "g1", "g2", "g1/d2", "g2/d2", "not-existing"} ) {
        if( ! f1.object_exists(path)  ) {
            pout << path, " is dangled\n";
            continue;
        }
    
        auto type = f1.get_object_info(path).type;
        if( type == f1.tGROUP )
            pout << path, " is a group\n";
        else if( type == f1.tDATASET )
            pout << path, " is a dataset\n";
        else 
            pout << path, " has other type\n";
    }
}

void iterating_under_a_group() {
    H5::File f1("f1.h5", "r");

    auto link_cb = [&](H5::Group::link_iter_arg_t &link) -> herr_t {
        pout << "Link ", link.name(), " has type ", link.info().type, '\n';
        return 0;
    };

    hsize_t idx = 0;
    f1.link_iterate(idx, link_cb);
    pout << "no. of links = ", idx, endl;

    f1.link_visit(link_cb);

    auto obj_cb = [&](H5::Group::obj_iter_arg_t &obj) -> herr_t {
        pout << "Object ", obj.name(), " has type ", obj.info().type, '\n';
        return 0;
    };
    f1.object_visit(obj_cb);
}

int main(int argc, char const *argv[]) {
    
    // file_creation_and_opening();
    // group_creation_and_opening();
    link_creation();
    iterating_under_a_group();

    return 0;
}
