/**
create: Yangyao CHEN, 2021/06/14
    [write   ] ...
    GaussianTailDist - distribution functions of gaussian tail distribution.
*/

#ifndef _HIPPNUMERICAL_RANDIST_GAUSSIAN_TAIL_H_
#define _HIPPNUMERICAL_RANDIST_GAUSSIAN_TAIL_H_
#include <gsl/gsl_randist.h>
namespace HIPP::NUMERICAL {

/**
GaussianTailDist - for the gaussian tail distribution, i.e., its pdf is the 
upper tail of a full normal distribution N(x|0, sigma), with x > a.
*/
class GaussianTailDist {
public:
    static double pdf(double x, double a, double sigma) noexcept;
    static double pdf_unit(double x, double a) noexcept;
};

inline double GaussianTailDist::pdf(double x, double a, double sigma) noexcept {
    return gsl_ran_gaussian_tail_pdf(x, a, sigma);
}
inline double GaussianTailDist::pdf_unit(double x, double a) noexcept {
    return gsl_ran_ugaussian_tail_pdf(x, a);
}


} // namespace HIPP::NUMERICAL

#endif	//_HIPPNUMERICAL_RANDIST_GAUSSIAN_TAIL_H_