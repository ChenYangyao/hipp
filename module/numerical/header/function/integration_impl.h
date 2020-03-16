#ifndef _HIPPNUMERICAL_INTEGRATION_IMPL_H_
#define _HIPPNUMERICAL_INTEGRATION_IMPL_H_
#include "integration.h"
namespace HIPP{
namespace NUMERICAL{
namespace _integration_helper{

inline BaseInteg::BaseInteg( fn_t fn, void *extra_data )
    :_gsl_fn(fn, extra_data){
    _HIPPNUMERICAL_GSL_UTIL_MAKE_INIT
}
inline BaseInteg::~BaseInteg() noexcept{}

inline BaseInteg::fn_t & BaseInteg::fn( void * &extra_data ) noexcept 
    { return _gsl_fn.get_user_fn(extra_data); }
inline void BaseInteg::eps( double &eps_abs, double &eps_rel ) const noexcept{
    eps_abs = _eps_abs; eps_rel = _eps_rel;
}

inline double BaseInteg::result() const noexcept { return _result; }
inline double BaseInteg::err_abs() const noexcept { return _err_abs; }
inline size_t BaseInteg::neval() const noexcept { return _neval; }
inline int BaseInteg::last_errno() const noexcept { return _last_errno; }
inline void BaseInteg::_set_fn( fn_t fn, void *extra_data){
    _gsl_fn.set_fn(fn, extra_data);
}
inline void BaseInteg::_set_eps( double eps_abs, double eps_rel ){
    _eps_abs = eps_abs;
    _eps_rel = eps_rel;
}

inline QNG::~QNG() noexcept{}
inline QNG & QNG::set_fn( fn_t fn, void *extra_data){ 
    _set_fn(fn, extra_data); return *this; 
}
inline QNG & QNG::set_eps( double eps_abs, double eps_rel ){ 
    _set_eps(eps_abs, eps_rel); return *this; 
}
inline double QNG::operator()( double a, double b ){ 
    _last_errno = 
        gsl_integration_qng( &_gsl_fn.get_gsl_fn(), a, b, 
            _eps_abs, _eps_rel, &_result, &_err_abs, &_neval );
    return _result;
}

inline QAG::QAG( fn_t fn, void *extra_data, int key, size_t wspace_limit )
: BaseInteg(fn, extra_data), _key(key), 
_wspace_limit(wspace_limit), _wspace(nullptr)
{ }

inline QAG::~QAG(){ 
    free_wspace(); 
}
inline QAG & QAG::set_fn( fn_t fn, void *extra_data ){ 
    _set_fn(fn, extra_data); return *this; 
}
inline QAG & QAG::set_eps( double eps_abs, double eps_rel ){ 
    _set_eps(eps_abs, eps_rel); return *this; 
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
inline double QAG::operator()( double a, double b ){
    _alloc_wspace();
    _last_errno = gsl_integration_qag( 
        &_gsl_fn.get_gsl_fn(), a, b, _eps_abs, _eps_rel, _wspace_limit,
        _key, _wspace, &_result, &_err_abs );
    return _result;
}
inline double QAG::qags( double a, double b ){
    _alloc_wspace();
    _last_errno = gsl_integration_qags( 
        &_gsl_fn.get_gsl_fn(), a, b, _eps_abs, _eps_rel, _wspace_limit,
        _wspace, &_result, &_err_abs );
    return _result;
}
inline double QAG::qagp( double *pts, size_t npts ){
    _alloc_wspace();
    _last_errno = gsl_integration_qagp( &_gsl_fn.get_gsl_fn(), pts, npts,
        _eps_abs, _eps_rel, _wspace_limit, _wspace, &_result,
        &_err_abs );
    return _result;
}
inline double QAG::qagi(){
    _alloc_wspace();
    _last_errno = gsl_integration_qagi( &_gsl_fn.get_gsl_fn(),
        _eps_abs, _eps_rel, _wspace_limit, _wspace, &_result, &_err_abs );
    return _result;
}
inline double QAG::qagiu( double a ){
    _alloc_wspace();
    _last_errno = gsl_integration_qagiu( &_gsl_fn.get_gsl_fn(), a,
        _eps_abs, _eps_rel, _wspace_limit, _wspace, &_result, &_err_abs );
    return _result;
}
inline double QAG::qagil( double b ){
    _alloc_wspace();
    _last_errno = gsl_integration_qagil( &_gsl_fn.get_gsl_fn(), b,
        _eps_abs, _eps_rel, _wspace_limit, _wspace, &_result, &_err_abs );
    return _result;
}
inline void QAG::_alloc_wspace(){
    if( !_wspace )
        _wspace = gsl_integration_workspace_alloc( _wspace_limit );
}


} // namespace _integration_helper
} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_INTEGRATION_IMPL_H_