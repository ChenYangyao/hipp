/**
create: Yangyao CHEN, 2021/06/16
    [write   ]
    ExponentialDist - exponential distribution functions.
*/

#ifndef _HIPPNUMERICAL_RANDIST_EXPONENTIAL_H_
#define _HIPPNUMERICAL_RANDIST_EXPONENTIAL_H_
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
namespace HIPP::NUMERICAL {

/**
ExponentialDist - exponential distribution functions with mean mu, i.e.
    p(x) = 1/mu exp( -x / mu ), for x >= 0.
*/
class ExponentialDist {
public:
    static double pdf(double x, double mu) noexcept;
    static double cdf_lower(double x, double mu) noexcept;
    static double cdf_upper(double x, double mu) noexcept;
    static double invcdf_lower(double P, double mu) noexcept;
    static double invcdf_upper(double Q, double mu) noexcept;
};

inline double ExponentialDist::pdf(double x, double mu) noexcept {
    return gsl_ran_exponential_pdf(x, mu);
}
inline double ExponentialDist::cdf_lower(double x, double mu) noexcept {
    return gsl_cdf_exponential_P(x, mu);
}
inline double ExponentialDist::cdf_upper(double x, double mu) noexcept {
    return gsl_cdf_exponential_Q(x, mu);
}
inline double ExponentialDist::invcdf_lower(double P, double mu) noexcept {
    return gsl_cdf_exponential_Pinv(P, mu);
}
inline double ExponentialDist::invcdf_upper(double Q, double mu) noexcept {
    return gsl_cdf_exponential_Qinv(Q, mu);
}

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANDIST_EXPONENTIAL_H_
