/**
 * This small example demonstrates the printing functions provided by HIPP basic
 * module.
 */
#include <hippcntl.h>
using namespace HIPP;

int main(int argc, char const *argv[]){
    double a = 1, b = 2;
    int c = 3;
    string s = "Sum of ";
    vector<double> arr = {1,2,3,4,5};

    /**
     * prt( ostream &os, ... ) prints any arguments `...` into stream `os`.
     * You may pass any type of variables in `...`, as long as its output 
     * operator `<<` is overloaded.
     * The output stream `os` is also returned, so you may used `<<` to follow
     * it such as `<< endl`.
     */
    prt( cout, s, a, " and ", b, " and ", c, " is ", a+b+c ) << endl;

    /**
     * prt_f( ostream &os, ...) is just like C-style printf(), but it prints
     * the content into stream `os`.
     */
    prt_f( cout, "Up to precision 10, a is %.10f", a ) << endl;

    /**
     * prt_a( ostream &os, ... ) is similar, but used to print an array-like
     * object.
     * 
     * We support more complex array printing through the class PrtArray<>,
     * where you may adjust the separator, newline sequence,field width, number
     * of columns, etc.
     */
    cout << "The array is ";
    prt_a( cout, arr ) << endl;

    /**
     * After calling these printing functions, the shell should display:
     * Sum of 1 and 2 and 3 is 6
     * Up to precision 10, a is 1.0000000000
     * The array is 1,2,3,4,5
     */
    
    return 0;
}
