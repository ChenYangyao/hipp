#include <hippio.h>

namespace H5 = HIPP::IO::H5;
using namespace std;
using namespace HIPP;

void file_creation_and_opening() {
    
    // File creation
    H5::File f1 ("f1.h5", "w");     // Old file gets truncated.
    //H5::File f11("f1.h5", "x");   // Will fail because existing.
    H5::File f12("f1.h5", "ac");    // Open the existing file.

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
    auto g12 = f1.create_group("g1", "ac");     // Open the existing group.

    auto g2 = g1.create_group("g2", "ac");      // Create /g1/g2.

    auto g13 = f1.open_group("g1");             // Open /g1.
    auto g21 = f1.open_group("g1/g2");          // Open /g1/g2.
    g21 = g1.open_group("g2");                  // Open /g1/g2 via g1.  

}

int main(int argc, char const *argv[]) {
    
    file_creation_and_opening();
    group_creation_and_opening();

    return 0;
}
