/**
create: Yangyao CHEN, 2021/06/14
    [write   ]
    UniformRealDist - distribution functions (PDF, CDF and its inverse) for 
    flat/uniform distribution of real number.
*/ 

#ifndef _HIPPNUMERICAL_RANDIST_UNIFORM_REAL_H_
#define _HIPPNUMERICAL_RANDIST_UNIFORM_REAL_H_
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
namespace HIPP::NUMERICAL {

/**
UniformRealDist - the distribution function is uniform in [a, b], i.e.,
    p(x) = 1/(b-a).
*/
class UniformRealDist {
public:
    static double pdf(double x, double a, double b) noexcept;
    static double cdf_lower(double x, double a, double b) noexcept;
    static double cdf_upper(double x, double a, double b) noexcept;
    static double invcdf_lower(double P, double a, double b) noexcept;
    static double invcdf_upper(double Q, double a, double b) noexcept;
};

inline double UniformRealDist::pdf(double x, double a, double b) noexcept {
    return gsl_ran_flat_pdf(x, a, b);
}
inline double UniformRealDist::cdf_lower(double x, double a, double b) 
noexcept {
    return gsl_cdf_flat_P(x, a, b);
}
inline double UniformRealDist::cdf_upper(double x, double a, double b) 
noexcept {
    return gsl_cdf_flat_Q(x, a, b);
}
inline double UniformRealDist::invcdf_lower(double P, double a, double b) 
noexcept {
    return gsl_cdf_flat_Pinv(P, a, b);
}
inline double UniformRealDist::invcdf_upper(double Q, double a, double b) 
noexcept {
    return gsl_cdf_flat_Qinv(Q, a, b);
}


} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANDIST_UNIFORM_REAL_H_