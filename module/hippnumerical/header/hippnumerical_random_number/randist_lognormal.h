/**
create: Yangyao CHEN, 2021/06/14
    [write   ]
    LognormalDist - distribution functions of lognormal.
*/

#ifndef _HIPPNUMERICAL_RANDIST_LOGNORMAL_H_
#define _HIPPNUMERICAL_RANDIST_LOGNORMAL_H_
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
namespace HIPP::NUMERICAL {

/**
LognormalDist - the lognormal distribution for x > 0, i. e.
    p(x) = 1/(x sqrt( 2 pi sigma^2) ) exp( -( ln(x) - zeta )^2 / 2 sigma^2 ).
*/
class LognormalDist {
public:
    static double pdf(double x, double zeta, double sigma) noexcept;
    static double cdf_lower(double x, double zeta, double sigma) noexcept; 
    static double cdf_upper(double x, double zeta, double sigma) noexcept; 
    static double invcdf_lower(double P, double zeta, double sigma) noexcept;
    static double invcdf_upper(double Q, double zeta, double sigma) noexcept;    
};

inline double LognormalDist::pdf(double x, double zeta, double sigma) noexcept {
    return gsl_ran_lognormal_pdf(x, zeta, sigma);
}
inline double LognormalDist::cdf_lower(double x, double zeta, double sigma) 
noexcept {
    return gsl_cdf_lognormal_P(x, zeta, sigma);
}
inline double LognormalDist::cdf_upper(double x, double zeta, double sigma) 
noexcept {
    return gsl_cdf_lognormal_Q(x, zeta, sigma);
}
inline double LognormalDist::invcdf_lower(double P, double zeta, double sigma) 
noexcept {
    return gsl_cdf_lognormal_Pinv(P, zeta, sigma);
}
inline double LognormalDist::invcdf_upper(double Q, double zeta, double sigma) 
noexcept {
    return gsl_cdf_lognormal_Qinv(Q, zeta, sigma);
}


} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANDIST_LOGNORMAL_H_