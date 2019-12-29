#include<mystd.h>
using namespace MYSTD;

class A1{
public:
    virtual const char *what() noexcept{ return "A1"; }
};

class A2{
public:
    virtual const char *what() noexcept{ return "A2"; }
};

class B: public A1, public A2{
};

int main(int argc, char const *argv[]){
    
    B b;
    cout << b.what( ) << endl;


    return 0;
}

