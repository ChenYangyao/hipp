/**
create: Yangyao CHEN, 2021/06/16
    [write   ]
    LaplaceDist - laplace distribution functions.
*/

#ifndef _HIPPNUMERICAL_RANDIST_LAPLACE_H_
#define _HIPPNUMERICAL_RANDIST_LAPLACE_H_
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
namespace HIPP::NUMERICAL {

/**
LaplaceDist - laplace distribution functions with with a, i.e.
    p(x) = 1/(2a) exp( -|x/a| ), for x in (-inf, inf).
*/
class LaplaceDist {
public:
    static double pdf(double x, double a) noexcept;
    static double cdf_lower(double x, double a) noexcept;
    static double cdf_upper(double x, double a) noexcept;
    static double invcdf_lower(double P, double a) noexcept;
    static double invcdf_upper(double Q, double a) noexcept;
};

inline double LaplaceDist::pdf(double x, double a) noexcept {
    return gsl_ran_laplace_pdf(x, a);
}
inline double LaplaceDist::cdf_lower(double x, double a) noexcept {
    return gsl_cdf_laplace_P(x, a);
}
inline double LaplaceDist::cdf_upper(double x, double a) noexcept {
    return gsl_cdf_laplace_Q(x, a);
}
inline double LaplaceDist::invcdf_lower(double P, double a) noexcept {
    return gsl_cdf_laplace_Pinv(P, a);
}
inline double LaplaceDist::invcdf_upper(double Q, double a) noexcept {
    return gsl_cdf_laplace_Qinv(Q, a);
}

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANDIST_LAPLACE_H_
