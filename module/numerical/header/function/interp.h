#ifndef _HIPPNUMERICAL_INTERP_H_
#define _HIPPNUMERICAL_INTERP_H_
#include "function_base.h"
#include "gsl/gsl_interp.h"
#include "gsl/gsl_spline.h"
namespace HIPP{
namespace NUMERICAL{

namespace _interp_helper{

class BaseInterp {
public:
    typedef gsl_interp_type interp_type_t;
    typedef gsl_interp _interp_t;
    typedef gsl_spline _spline_t;
    typedef gsl_interp_accel _accel_t;

    typedef double value_t;
    typedef std::size_t size_t;

    static const interp_type_t * const LINEAR, 
        * const POLYNOMIAL,
        * const CSPLINE,
        * const CSPLINE_PERIODIC,
        * const AKIMA,
        * const AKIMA_PERIODIC,
        * const STEFFEN;
};

class Accel {
public:
    typedef BaseInterp::_accel_t _accel_t;
    Accel();
    ~Accel();
    Accel & reset();
    
    _accel_t * get() noexcept;
protected:
    _accel_t *_accel;
};

class Spline : public BaseInterp {
public:
    Spline();
    Spline( const double *x, const double *y, size_t size, 
        const interp_type_t *type = LINEAR );
    Spline( const vector<double> &x, const vector<double> &y, 
        const interp_type_t *type = LINEAR );
    ~Spline();
    _HIPPNUMERICAL_GSL_UTIL_DELETE_CP_MV(Spline)

    Spline & reset();
    Spline & reset( const double *x, const double *y, size_t size,
        const interp_type_t *type = LINEAR );
    Spline & reset( const vector<double> &x, const vector<double> &y,
        const interp_type_t *type = LINEAR );

    string name() const;
    static size_t min_size( const interp_type_t *type );
    size_t min_size();
    bool empty() const noexcept;
    int last_errno() const noexcept;

    double operator()( double x );
    double deriv( double x );
    double deriv2( double x );
    double integ( double a, double b );
protected:
    Accel _accel;
    _spline_t *_spline;
    int _last_errno;

    void _free_spline();
    void _init_spline( const double *x, const double *y, size_t size,
        const interp_type_t *type = LINEAR );
};
} // namespace _interp_helper

using _interp_helper::Spline;

} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_INTERP_H_