#include <hippio.h>

using namespace HIPP;
using namespace std;
namespace H5 = HIPP::IO::H5;

int main(int argc, char const *argv[]) {

    H5::File f("file1.h5", "w");            // Create a new file (truncate if existing).

    {
        H5::File f("file1.h5", "a");        // Open an existing file.
    }

    H5::Group g1 = f.create_group("g1");    // Create a new data group.
    auto g2 = g1.create_group("g2-in-g1");  // Use "auto" for type deduction.

    {
        auto g_root = f.open_group("/");
    }
    {
        auto g2 = f.open_group("g1/g2-in-g1");
    }
    {
        auto g2 = g1.open_group("g2-in-g1");
    }

    return 0;
}
