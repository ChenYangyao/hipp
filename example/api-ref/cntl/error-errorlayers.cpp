#include <hippcntl.h>
using namespace HIPP;

int main(int argc, char const *argv[]){

    try {
        throw ErrLogic(ErrLogic::eLENGTH);
    }catch( const ErrLogic &e ){
        /* priting the detail of error */
        pout << e.whats(), endl;

        /* retrieve its errno for application, errno for class, and errno */
        pout << e.ErrApp::get_errno(), ", ", 
                e.ErrClass::get_errno(), ", ",
                e.get_errno(), endl;
    }
    
    return 0;
}
