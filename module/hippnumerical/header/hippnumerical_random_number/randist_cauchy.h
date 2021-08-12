/**
create: Yangyao CHEN, 2021/06/16
    [write   ] CauchyDist - Cauchy distribution (also known as Lorentz 
        distribution) functions.
*/

#ifndef _HIPPNUMERICAL__H_
#define _HIPPNUMERICAL__H_
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
namespace HIPP::NUMERICAL {

/**
Cauchy distribution (also known as Lorentz distribution) with scale parameter 
a, i.e.,
    p(x) = 1 / ( a pi ( 1+ (x/a)^2 ) ), for x in (-inf, inf).
*/
class CauchyDist {
public:
    static double pdf(double x, double a) noexcept;
    static double cdf_lower(double x, double a) noexcept;
    static double cdf_upper(double x, double a) noexcept;
    static double invcdf_lower(double P, double a) noexcept;
    static double invcdf_upper(double Q, double a) noexcept;
};

inline double CauchyDist::pdf(double x, double a) noexcept {
    return gsl_ran_cauchy_pdf(x, a);
}
inline double CauchyDist::cdf_lower(double x, double a) noexcept {
    return gsl_cdf_cauchy_P(x, a);
}
inline double CauchyDist::cdf_upper(double x, double a) noexcept {
    return gsl_cdf_cauchy_Q(x, a);
}
inline double CauchyDist::invcdf_lower(double P, double a) noexcept {
    return gsl_cdf_cauchy_Pinv(P, a);
}
inline double CauchyDist::invcdf_upper(double Q, double a) noexcept {
    return gsl_cdf_cauchy_Qinv(Q, a);
}

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL__H_
