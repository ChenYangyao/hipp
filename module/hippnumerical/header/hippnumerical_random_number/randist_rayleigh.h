/**
create: Yangyao CHEN, 2021/06/16
    [write   ] RayleighDist - Rayleigh distribution functions.
*/

#ifndef _HIPPNUMERICAL_RANDIST_RAYLEIGH_H_
#define _HIPPNUMERICAL_RANDIST_RAYLEIGH_H_
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
namespace HIPP::NUMERICAL {

/**
Rayleigh distribution with scale parameter sigma, i.e.,
    p(x) = x / sigma^2 exp( -x^2/(2 sigma^2) ), for x > 0.
*/
class RayleighDist {
public:
    static double pdf(double x, double sigma) noexcept;
    static double cdf_lower(double x, double sigma) noexcept;
    static double cdf_upper(double x, double sigma) noexcept;
    static double invcdf_lower(double P, double sigma) noexcept;
    static double invcdf_upper(double Q, double sigma) noexcept;
};

inline double RayleighDist::pdf(double x, double sigma) noexcept {
    return gsl_ran_rayleigh_pdf(x, sigma);
}
inline double RayleighDist::cdf_lower(double x, double sigma) noexcept {
    return gsl_cdf_rayleigh_P(x, sigma);
}
inline double RayleighDist::cdf_upper(double x, double sigma) noexcept {
    return gsl_cdf_rayleigh_Q(x, sigma);
}
inline double RayleighDist::invcdf_lower(double P, double sigma) noexcept {
    return gsl_cdf_rayleigh_Pinv(P, sigma);
}
inline double RayleighDist::invcdf_upper(double Q, double sigma) noexcept {
    return gsl_cdf_rayleigh_Qinv(Q, sigma);
}

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANDIST_RAYLEIGH_H_
