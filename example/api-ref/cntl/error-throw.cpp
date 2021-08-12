#include <hippcntl.h>
using namespace HIPP;

int main(int argc, char const *argv[])
{
    if( argc != 3 ){
        perr << emFLPFB, 
            "  -  invalid no. of arguments.  \n",
            "  -  expected 3, got ", argc, endl;
        throw ErrLogic(ErrLogic::eLENGTH);
    }
    return 0;
}
