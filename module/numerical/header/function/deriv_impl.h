#ifndef _HIPPNUMERICAL_DERIV_IMPL_H_
#define _HIPPNUMERICAL_DERIV_IMPL_H_
#include "deriv.h"
namespace HIPP{
namespace NUMERICAL{


inline Deriv::Deriv( fn_t fn, void *extra_data ): _gsl_fn(fn, extra_data){
    _HIPPNUMERICAL_GSL_UTIL_MAKE_INIT
}
inline Deriv::~Deriv(){ }

inline Deriv & Deriv::set_fn( fn_t fn, void *extra_data ){
    _gsl_fn.set_fn( fn, extra_data );
    return *this;
}

inline Deriv::fn_t & Deriv::fn( void * &extra_data ) noexcept{
    return _gsl_fn.get_user_fn( extra_data );
}

inline double Deriv::operator()( double x, double h ){
    _last_errno = gsl_deriv_central( 
        &_gsl_fn.get_gsl_fn(), x, h, &_result, &_err_abs );
    return _result;
}
inline double Deriv::forward( double x, double h ){
    _last_errno = gsl_deriv_forward( 
        &_gsl_fn.get_gsl_fn(), x, h, &_result, &_err_abs );
    return _result;
}
inline double Deriv::backward( double x, double h ){
    _last_errno = gsl_deriv_backward( 
        &_gsl_fn.get_gsl_fn(), x, h, &_result, &_err_abs );
    return _result;
}

inline double Deriv::result() const noexcept{ return _result; }
inline double Deriv::err_abs() const noexcept{ return _err_abs; }
inline int Deriv::last_errno() const noexcept{ return _last_errno; }


} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_DERIV_IMPL_H_