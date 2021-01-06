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
    
    Deriv( fn_t fn = fn_t() );
    _HIPPNUMERICAL_GSL_UTIL_DEFAULT_CP_MV(Deriv)
    ~Deriv();

    Deriv & set_fn( fn_t fn = fn_t());

    fn_t & fn() noexcept;
    const fn_t & fn() const noexcept;

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

inline Deriv::Deriv( fn_t fn ): _gsl_fn(std::move(fn)){
}
inline Deriv::~Deriv() {}

inline Deriv & Deriv::set_fn( fn_t fn ){
    _gsl_fn.set_fn( std::move(fn) );
    return *this;
}

inline Deriv::fn_t & Deriv::fn() noexcept{
    return _gsl_fn.get_user_fn();
}

inline const Deriv::fn_t & Deriv::fn() const noexcept{
    return _gsl_fn.get_user_fn();
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
#endif	//_HIPPNUMERICAL_DERIV_H_