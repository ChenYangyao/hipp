#include <hippcntl.h>
using namespace HIPP;

int main(int argc, char const *argv[]){
    try{
        ErrSystem::check( 5, "Main::Check" );
    }catch( ErrSystem &e ){
        ErrSystem::err_app_t &eapp = e;
        ErrSystem::err_class_t &ec = e;
        cout << eapp.get_errno() << endl;
        cout << ec.get_errno() << endl;
        cout << e.get_errno() << endl;
    }
    
    return 0;
}
