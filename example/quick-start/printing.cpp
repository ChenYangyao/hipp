/* printing.cpp */
#include <hippcntl.h>
using namespace std;

int main(int argc, char const *argv[]) 
{
    double a = 1;
    int b = 2;
    string s = "The sum of ";

    /* print arguments of any type to standard out */
    HIPP::pout << s, a, " and ", b, " is ", a+b, endl;

    /* print any STL container that supports iteration into stream */
    vector<int> arr {1,2,3,4,5};
    HIPP::prt_a(cout, arr) << endl;

    /* transform variables into string */
    string s_joined = HIPP::str(s, a, " and ", b, " is ", a+b, '\n')
        + HIPP::str_a(arr) + '\n';
    HIPP::pout << s_joined;

    return 0;
}
