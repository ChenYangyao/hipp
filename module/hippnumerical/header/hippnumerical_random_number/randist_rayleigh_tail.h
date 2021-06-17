/**
create: Yangyao CHEN, 2021/06/16
    [write   ] RayleighTailDist - tail of Rayleigh distribution functions.
*/

#ifndef _HIPPNUMERICAL__H_
#define _HIPPNUMERICAL__H_
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
namespace HIPP::NUMERICAL {

/**
Tail of Rayleigh distribution with scale parameter sigma and a lower limit a,
i.e.,
    p(x) = x / sigma^2 exp((a^2-x^2)/(2 sigma^2)), for x > a
*/
class RayleighTailDist {
public:
    static double pdf(double x, double a, double sigma) noexcept;
};

inline double RayleighTailDist::pdf(double x, double a, double sigma) noexcept {
    return gsl_ran_rayleigh_tail_pdf(x, a, sigma);
}

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL__H_
