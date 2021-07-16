/**
create: Yangyao CHEN, 2021/06/14
    [write   ] BivariateGaussian - normal distribution in 2D space.
*/
#ifndef _HIPPNUMERICAL_RANDIST_BIVARIATE_GAUSSIAN_H_
#define _HIPPNUMERICAL_RANDIST_BIVARIATE_GAUSSIAN_H_
#include <gsl/gsl_randist.h>
namespace HIPP::NUMERICAL {

class BivariateGaussianDist {
public:
    static double pdf(double x, double y, 
        double sigma_x, double sigma_y, double rho) noexcept;
};

inline double BivariateGaussianDist::pdf(double x, double y, 
    double sigma_x, double sigma_y, double rho) noexcept
{
    return gsl_ran_bivariate_gaussian_pdf(x, y, sigma_x, sigma_y, rho);
}

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANDIST_BIVARIATE_GAUSSIAN_H_
