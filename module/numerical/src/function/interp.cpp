#include "interp.h"
namespace HIPP{
namespace NUMERICAL{
namespace _interp_helper{

const BaseInterp::interp_type_t 
    * const BaseInterp::LINEAR = gsl_interp_linear,
    * const BaseInterp::POLYNOMIAL = gsl_interp_polynomial,
    * const BaseInterp::CSPLINE = gsl_interp_cspline,
    * const BaseInterp::CSPLINE_PERIODIC = gsl_interp_cspline_periodic,
    * const BaseInterp::AKIMA = gsl_interp_akima,
    * const BaseInterp::AKIMA_PERIODIC = gsl_interp_akima_periodic,
    * const BaseInterp::STEFFEN = gsl_interp_steffen;
    
} // namespace _interp_helper

} // namespace NUMERICAL
} // namespace HIPP