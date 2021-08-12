#include <hippcntl.h>
using namespace HIPP;


int main(int argc, char const *argv[]){
    
    try{
        throw ErrClassIO();
    }catch( const ErrClass &e ){
        perr << e.whats(), '\n',
            "errno=", e.get_errno(), endl;
    }

    try{
        throw ErrClassLogic();
    }catch( const ErrClass &e ){
        perr << e.whats(), '\n',
            "errno=", e.get_errno(), endl;
    }

    return 0;
}
