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

    BaseInteg( fn_t fn = fn_t());
    _HIPPNUMERICAL_GSL_UTIL_DEFAULT_CP_MV(BaseInteg)
    ~BaseInteg();

    BaseInteg & set_fn( fn_t fn = fn_t() );
    BaseInteg & set_eps(value_t epsabs=1.0e-5, value_t epsrel=1.0e-5) noexcept;
    fn_t & fn() noexcept;
    const fn_t & fn() const noexcept;
    std::pair<value_t, value_t> eps() const noexcept;

    double result() const noexcept;
    double errabs() const noexcept;
    size_t neval() const noexcept;
    int last_errno() const noexcept;
protected:
    GSLFn _gsl_fn;
    double _epsabs = 1.0e-6, _epsrel = 1.0e-6;
    
    double _result = 0., _errabs = 0.;
    size_t _neval = 0;
    int _last_errno = 0;
};

class QNG: public BaseInteg{
public:
    using BaseInteg::BaseInteg;
    _HIPPNUMERICAL_GSL_UTIL_DEFAULT_CP_MV(QNG)
    ~QNG();
    
    QNG & set_fn( fn_t fn = fn_t() );
    QNG & set_eps(value_t epsabs=1.0e-5, value_t epsrel=1.0e-5) noexcept;
    double operator()( double a, double b );
};

class QAG: public BaseInteg{
public:
    enum : int { 
        GAUSS15 = GSL_INTEG_GAUSS15,
        GAUSS21 = GSL_INTEG_GAUSS21,
        GAUSS31 = GSL_INTEG_GAUSS31,
        GAUSS41 = GSL_INTEG_GAUSS41,
        GAUSS51 = GSL_INTEG_GAUSS51,
        GAUSS61 = GSL_INTEG_GAUSS61 };
    typedef gsl_integration_workspace wspace_t;

    QAG( fn_t fn = fn_t(), int key = GAUSS15, size_t wspace_limit = 1024 );
    _HIPPNUMERICAL_GSL_UTIL_DELETE_CP_MV(QAG)
    ~QAG();
    
    QAG & set_fn( fn_t fn = fn_t() );
    QAG & set_eps(value_t epsabs=1.0e-5, value_t epsrel=1.0e-5) noexcept;
    QAG & set_key( int key = GAUSS15 );
    QAG & set_wspace(size_t limit = 1024);
    QAG & free_wspace();        // may called at any time, even multiple times

    int key() const noexcept;
    wspace_t & wspace() noexcept;
    const wspace_t & wspace() const noexcept;

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




/* implementations */
namespace _integration_helper {
inline BaseInteg::BaseInteg( fn_t fn )
:_gsl_fn(std::move(fn)){}

inline BaseInteg::~BaseInteg(){}
inline BaseInteg & BaseInteg::set_fn(fn_t fn) {
    _gsl_fn.set_fn(std::move(fn));
    return *this;
}
inline BaseInteg & BaseInteg::set_eps(value_t epsabs, value_t epsrel) noexcept{
    _epsabs = epsabs;
    _epsrel = epsrel;
    return *this;
}

inline auto BaseInteg::fn() noexcept -> fn_t & 
{ return _gsl_fn.get_user_fn(); }
inline auto BaseInteg::fn() const noexcept -> const fn_t & 
{ return _gsl_fn.get_user_fn(); }
inline auto BaseInteg::eps() const noexcept -> std::pair<value_t, value_t>{
    return {_epsabs, _epsrel};
}

inline double BaseInteg::result() const noexcept { return _result; }
inline double BaseInteg::errabs() const noexcept { return _errabs; }
inline size_t BaseInteg::neval() const noexcept { return _neval; }
inline int BaseInteg::last_errno() const noexcept { return _last_errno; }


inline QNG::~QNG(){}
inline QNG & QNG::set_fn( fn_t fn){
    BaseInteg::set_fn(std::move(fn));
    return *this;
}
inline QNG & QNG::set_eps(value_t epsabs, value_t epsrel) noexcept {
    BaseInteg::set_eps(epsabs, epsrel);
    return *this;
}
inline double QNG::operator()( double a, double b ){ 
    _last_errno = 
        gsl_integration_qng( &_gsl_fn.get_gsl_fn(), a, b, 
            _epsabs, _epsrel, &_result, &_errabs, &_neval );
    return _result;
}

inline QAG::QAG( fn_t fn, int key, size_t wspace_limit )
: BaseInteg(std::move(fn)), _key(key), _wspace_limit(wspace_limit), 
_wspace(nullptr)
{ }

inline QAG::~QAG(){ 
    free_wspace(); 
}
inline QAG & QAG::set_fn( fn_t fn ){
    BaseInteg::set_fn(std::move(fn));
    return *this;
}
inline QAG & QAG::set_eps(value_t epsabs, value_t epsrel) noexcept {
    BaseInteg::set_eps(epsabs, epsrel);
    return *this;
}
inline QAG & QAG::set_key( int key ){
    _key = key; return *this; 
}
inline QAG & QAG::set_wspace( size_t limit){
    if( limit != _wspace_limit ){
        free_wspace();
        _wspace_limit = limit;
    }
    return *this;
}
inline QAG & QAG::free_wspace(){ 
    if( _wspace ){
        gsl_integration_workspace_free( _wspace );  _wspace = nullptr;
    }
    return *this; 
}

inline int QAG::key() const noexcept{ return _key; }
inline QAG::wspace_t & QAG::wspace() noexcept { return *_wspace; }
inline const QAG::wspace_t & QAG::wspace() const noexcept { return *_wspace; }
inline double QAG::operator()( double a, double b ){
    _alloc_wspace();
    _last_errno = gsl_integration_qag( 
        &_gsl_fn.get_gsl_fn(), a, b, _epsabs, _epsrel, _wspace_limit,
        _key, _wspace, &_result, &_errabs );
    return _result;
}
inline double QAG::qags( double a, double b ){
    _alloc_wspace();
    _last_errno = gsl_integration_qags( 
        &_gsl_fn.get_gsl_fn(), a, b, _epsabs, _epsrel, _wspace_limit,
        _wspace, &_result, &_errabs );
    return _result;
}
inline double QAG::qagp( double *pts, size_t npts ){
    _alloc_wspace();
    _last_errno = gsl_integration_qagp( &_gsl_fn.get_gsl_fn(), pts, npts,
        _epsabs, _epsrel, _wspace_limit, _wspace, &_result,
        &_errabs );
    return _result;
}
inline double QAG::qagi(){
    _alloc_wspace();
    _last_errno = gsl_integration_qagi( &_gsl_fn.get_gsl_fn(),
        _epsabs, _epsrel, _wspace_limit, _wspace, &_result, &_errabs );
    return _result;
}
inline double QAG::qagiu( double a ){
    _alloc_wspace();
    _last_errno = gsl_integration_qagiu( &_gsl_fn.get_gsl_fn(), a,
        _epsabs, _epsrel, _wspace_limit, _wspace, &_result, &_errabs );
    return _result;
}
inline double QAG::qagil( double b ){
    _alloc_wspace();
    _last_errno = gsl_integration_qagil( &_gsl_fn.get_gsl_fn(), b,
        _epsabs, _epsrel, _wspace_limit, _wspace, &_result, &_errabs );
    return _result;
}
inline void QAG::_alloc_wspace(){
    if( !_wspace )
        _wspace = gsl_integration_workspace_alloc( _wspace_limit );
}
} // namespace _integration_helper


} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_INTEGRATION_H_