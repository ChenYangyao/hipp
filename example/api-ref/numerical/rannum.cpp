#include <hippnumerical.h>

using namespace HIPP;
using namespace HIPP::NUMERICAL;

int main(int argc, char const *argv[])
{
    /* Call the object to get a random number or a sequence of them. */
    double r1 = randn();                    /* return a single random number */

    vector<double> r2 = randn(3);           /* return a vector of 3 numbers */

    double r3[3];
    randn(r3, r3+3);                        /* fill 3 number into a range */

    /* Call get() method to use different parameters. */
    double r4 = randn.get(1.0, 3.0);        /* a random number with mean=1, stddev=3 */

    pout << r1, '\n',
        r2, '\n',
        pout(r3,r3+3), '\n',
        r4, endl;

    return 0;
}
