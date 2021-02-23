#include <hippcntl.h>
using namespace std;

int main(int argc, char const *argv[]) {
    
    try{
        throw HIPP::ErrLogic(HIPP::ErrLogic::eINVALIDARG);
    }catch(const HIPP::ErrLogic &e){
        HIPP::pout << "Error detail by what(): ", e.what(), '\n',
            "Error detail by whats(): ", e.whats(), '\n',
            "Error number: ", e.get_errno(), endl;
    }

    try{
        throw HIPP::ErrLogic(HIPP::ErrLogic::eINVALIDARG);
        throw HIPP::ErrRuntime(HIPP::ErrRuntime::eRANGE);
    }catch(const HIPP::ErrClass &e){
        HIPP::pout << "Error detail by what(): ", e.what(), '\n',
            "Error detail by whats(): ", e.whats(), '\n',
            "Error number: ", e.get_errno(), endl;
        if( e.get_errno() == e.eLOGIC ){
            // Do something for a logic error, e.g.,
            HIPP::perr << "A logic error", endl;
        }else if( e.get_errno() == e.eRUNTIME ){
            // Do something for a runtime error, e.g.,
            HIPP::perr << "A runtime error", endl;
        }else{
            // Other errors, just re-throw.
            throw;
        }
    }

    return 0;
}

