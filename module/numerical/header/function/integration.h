#ifndef _HIPPNUMERICAL_INTEGRATION_H_
#define _HIPPNUMERICAL_INTEGRATION_H_
#include "function_base.h"
#include "gsl/gsl_integration.h"
namespace HIPP{
namespace NUMERICAL{
namespace _integration_helper{

class BaseInteg{
public:
    typedef GSLFn::user_fn_t fn_t;
    typedef double value_t;
    typedef std::size_t size_t;

    BaseInteg( fn_t fn = &GSLFn::_null_user_fn, void *extra_data = nullptr );
    _HIPPNUMERICAL_GSL_UTIL_DEFAULT_CP_MV(BaseInteg)
    ~BaseInteg() noexcept;

    fn_t & fn( void * &extra_data ) noexcept;
    void eps( double &eps_abs, double &eps_rel ) const noexcept;

    double result() const noexcept;
    double err_abs() const noexcept;
    size_t neval() const noexcept;
    int last_errno() const noexcept;
protected:
    GSLFn _gsl_fn;
    double _eps_abs = 1.0e-6, _eps_rel = 1.0e-6;
    
    double _result = 0., _err_abs = 0.;
    size_t _neval = 0;
    int _last_errno = 0;

    void _set_fn( fn_t fn = GSLFn::_null_user_fn, void *extra_data = nullptr );
    void _set_eps( double eps_abs = 1.0e-6, double eps_rel = 1.0e-6 );
};

class QNG: public BaseInteg{
public:
    using BaseInteg::BaseInteg;
    _HIPPNUMERICAL_GSL_UTIL_DEFAULT_CP_MV(QNG)
    ~QNG() noexcept;
    
    QNG & set_fn( fn_t fn = GSLFn::_null_user_fn, void *extra_data = nullptr );
    QNG & set_eps( double eps_abs = 1.0e-6, double eps_rel = 1.0e-6 );
    double operator()( double a, double b );
};

class QAG: public BaseInteg{
public:
    enum : int { GAUSS15 = GSL_INTEG_GAUSS15,
        GAUSS21 = GSL_INTEG_GAUSS21,
        GAUSS31 = GSL_INTEG_GAUSS31,
        GAUSS41 = GSL_INTEG_GAUSS41,
        GAUSS51 = GSL_INTEG_GAUSS51,
        GAUSS61 = GSL_INTEG_GAUSS61 };
    typedef gsl_integration_workspace wspace_t;

    QAG( fn_t fn = GSLFn::_null_user_fn, void *extra_data = nullptr, 
        int key = GAUSS15, size_t wspace_limit = 1024 );
    _HIPPNUMERICAL_GSL_UTIL_DELETE_CP_MV(QAG)
    ~QAG();
    
    QAG & set_fn( fn_t fn = GSLFn::_null_user_fn, void *extra_data = nullptr );
    QAG & set_eps( double eps_abs = 1.0e-6, double eps_rel = 1.0e-6 );
    QAG & set_key( int key = GAUSS15 );
    QAG & set_wspace( size_t limit = 1024 );
    QAG & free_wspace();

    int key() const noexcept;
    wspace_t & wspace() noexcept;

    double operator()( double a, double b );
    double qags( double a, double b );
    double qagp( double *pts, size_t npts );
    double qagi();
    double qagiu( double a );
    double qagil( double b );
protected:
    int _key;
    size_t _wspace_limit;
    wspace_t *_wspace;
    void _alloc_wspace();
};

} // namespace _integration_helper


class Integration{
public:
    typedef _integration_helper::QNG QNG;
    typedef _integration_helper::QAG QAG;
};


} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_INTEGRATION_H_