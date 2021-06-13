/**
 * creat: Yangyao CHEN, 2020/12/22
 *      [write   ]
 *      GaussianDist - distribution functions (PDF, CDF and its inverse) for 
 *          the normal distribution.
 */ 


#ifndef _HIPPNUMERICAL_RANDIST_GAUSSIAN_H_
#define _HIPPNUMERICAL_RANDIST_GAUSSIAN_H_
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
namespace HIPP::NUMERICAL {

class GaussianDist {
public:
    /** 
     * cdf_lower() and cdf_upper() corresponds to P(x) and Q(x) in GSL.
     * "inv" functions are their inverse.
     */
    static double pdf(double x, double sigma) noexcept;
    static double cdf_lower(double x, double sigma) noexcept; 
    static double cdf_upper(double x, double sigma) noexcept; 
    static double invcdf_lower(double x, double sigma) noexcept;
    static double invcdf_upper(double x, double sigma) noexcept;    

    /* calls for the unit gaussian dist., i.e., sigma = 1 */
    static double pdf_unit_unit(double x) noexcept;
    static double cdf_lower_unit(double x) noexcept;
    static double cdf_upper_unit(double x) noexcept;
    static double invcdf_lower_unit(double x) noexcept;
    static double invcdf_upper_unit(double x) noexcept;    
};

/**
 * Random number distributions are commonly called at high frequency. We inline
 * them to avoid any overhead.
 */
inline double GaussianDist::pdf(double x, double sigma) noexcept {
    return gsl_ran_gaussian_pdf(x, sigma);
}
inline double GaussianDist::cdf_lower(double x, double sigma) noexcept {
    return gsl_cdf_gaussian_P(x, sigma);
}
inline double GaussianDist::cdf_upper(double x, double sigma) noexcept {
    return gsl_cdf_gaussian_Q(x, sigma);
}
inline double GaussianDist::invcdf_lower(double x, double sigma) noexcept {
    return gsl_cdf_gaussian_Pinv(x, sigma);
}
inline double GaussianDist::invcdf_upper(double x, double sigma) noexcept {
    return gsl_cdf_gaussian_Qinv(x, sigma);
}
inline double GaussianDist::pdf_unit_unit(double x) noexcept {
    return gsl_ran_ugaussian_pdf(x);
}
inline double GaussianDist::cdf_lower_unit(double x) noexcept {
    return gsl_cdf_ugaussian_P(x);
}
inline double GaussianDist::cdf_upper_unit(double x) noexcept {
    return gsl_cdf_ugaussian_Q(x);
}
inline double GaussianDist::invcdf_lower_unit(double x) noexcept {
    return gsl_cdf_ugaussian_Pinv(x);
}
inline double GaussianDist::invcdf_upper_unit(double x) noexcept {
    return gsl_cdf_ugaussian_Qinv(x);
}


} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_RANDIST_GAUSSIAN_H_