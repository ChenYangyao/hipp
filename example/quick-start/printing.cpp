/* printing.cpp */
#include <hippcntl.h>
using namespace std;

int main(int argc, char const *argv[]) 
{
    double a = 1;
    int b = 2;
    string s = "The sum of ";
    vector<int> arr = {1,2,3,4,5};

    /* print arguments of any type to standard out */
    HIPP::pout << s, a, " and ", b, " is ", a+b, endl;

    /* print any STL container that supports iteration into stream */
    HIPP::pout << "arr is ", arr, endl;

    /* transform variables into string */
    string s_joined = 
        HIPP::str(s, a, " and ", b, " is ", a+b) 
            + ", and arr is " 
            + HIPP::str_a(arr);
    HIPP::pout << s_joined, endl;

    return 0;
}
