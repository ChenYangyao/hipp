#include <mystd.h>
using namespace MYSTD;

namespace AA
{
class A{
public:
    template<typename T>
    static void pp(int ad){ cout << __func__ << " and " << __PRETTY_FUNCTION__ << endl; }
};    
} // namespace AA


int main(int argc, char const *argv[]){
    AA::A::pp<int>(1);
    
    return 0;
}
