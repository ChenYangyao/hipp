/**
create: Yangyao CHEN, 2021/06/16
    [write   ]
    ExpPowDist - exponential power distribution functions.
*/

#ifndef _HIPPNUMERICAL_RANDIST_EXP_POW_H_
#define _HIPPNUMERICAL_RANDIST_EXP_POW_H_
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
namespace HIPP::NUMERICAL {

/**
ExpPowDist - exponential power distribution functions with scale parameter a and
exponential b, i.e.
    p(x) = 1/(2 a Gamma(1+1/b)) exp( -|x / a|^b ), for x >= 0.
Special cases:
    - b = 1: Laplace distribution.
    - b = 2: Gaussian distribution with a = sqrt(2) sigma.
*/
class ExpPowDist {
public:
    static double pdf(double x, double a, double b) noexcept;
    static double cdf_lower(double x, double a, double b) noexcept;
    static double cdf_upper(double x, double a, double b) noexcept;
};

inline double ExpPowDist::pdf(double x, double a, double b) noexcept {
    return gsl_ran_exppow_pdf(x, a, b);
}
inline double ExpPowDist::cdf_lower(double x, double a, double b) noexcept {
    return gsl_cdf_exppow_P(x, a, b);
}
inline double ExpPowDist::cdf_upper(double x, double a, double b) noexcept {
    return gsl_cdf_exppow_Q(x, a, b);
}

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANDIST_EXP_POW_H_
