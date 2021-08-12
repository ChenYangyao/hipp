#include <hippnumerical.h>

using namespace HIPP;
using namespace HIPP::NUMERICAL;

int main(int argc, char const *argv[])
{
    double sigma = 2.0;
    pout << "Gaussian distribution p(x) and cumulative P(x):\n";
    for( auto x : {0., 1., 2., 3., 4., 5.} )
        pout << "p(", x, ") = ",  GaussianDist::pdf(x, sigma), 
            ", P(", x, ") = ", GaussianDist::cdf_lower(x, sigma), endl;
    
    return 0;
}

