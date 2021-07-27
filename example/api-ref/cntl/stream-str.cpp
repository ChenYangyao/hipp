#include <hippcntl.h>
using namespace HIPP;

int main(int argc, char const *argv[])
{
    for(int i=0; i<10; ++i){
        ofstream ofs(HIPP::str("file.", i, ".txt") );
        /* ... write to ofs ... */
    }

    for(int i=0; i<10; ++i){
        double redshift = 0.01 * i;
        ofstream ofs(HIPP::str_f("snapshot.%03d.z%.2f.txt", i, redshift));
        /* ... write to ofs */
    }
    return 0;
}
