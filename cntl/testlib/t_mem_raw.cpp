#include <mystd.h>
#include <hippcntl.h>
using namespace MYSTD;
using namespace HIPP;

int main(int argc, char const *argv[])
{
    prt( cout, 1+1 ) << endl;

    string filename = str( "file", 100, ".dat" );
    cout << filename << endl;

    return 0;
}
