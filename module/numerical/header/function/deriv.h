#ifndef _HIPPNUMERICAL_DERIV_H_
#define _HIPPNUMERICAL_DERIV_H_
#include "function_base.h"
#include "gsl/gsl_deriv.h"
namespace HIPP{
namespace NUMERICAL{

class Deriv{
public:
    typedef GSLFn::user_fn_t fn_t;
    typedef double value_t;
    
    Deriv( fn_t fn = &GSLFn::_null_user_fn, void *extra_data = nullptr );
    _HIPPNUMERICAL_GSL_UTIL_DEFAULT_CP_MV(Deriv)
    ~Deriv();

    Deriv & set_fn( fn_t fn = &GSLFn::_null_user_fn, 
        void *extra_data = nullptr );

    fn_t & fn( void * &extra_data ) noexcept;

    double operator()( double x, double h );
    double forward( double x, double h );
    double backward( double x, double h );

    double result() const noexcept;
    double err_abs() const noexcept;
    int last_errno() const noexcept;
protected:
    GSLFn _gsl_fn;

    double _result = 0., _err_abs = 0.;
    int _last_errno = 0;
};


} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_DERIV_H_