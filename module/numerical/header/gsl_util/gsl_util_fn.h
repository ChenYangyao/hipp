#ifndef _HIPPNUMERICAL_GSL_UTIL_FN_H_
#define _HIPPNUMERICAL_GSL_UTIL_FN_H_
#include "gsl_util_base.h"
#include "gsl_util_error.h"
#include "gsl/gsl_math.h"
namespace HIPP{
namespace NUMERICAL{

class GSLFn{
public:
    typedef std::function< double(double x, void *data) > user_fn_t;
    typedef gsl_function gsl_fn_t;

    GSLFn( user_fn_t user_fn = &_null_user_fn, void *data = nullptr );

    GSLFn( const GSLFn & );
    GSLFn( GSLFn && );
    GSLFn & operator=( const GSLFn & );
    GSLFn & operator=( GSLFn && );
    ~GSLFn() noexcept;

    void set_fn( user_fn_t user_fn = &_null_user_fn, void *data = nullptr );
    
    double operator()( double x );

    user_fn_t & get_user_fn( void *&data ) noexcept;
    gsl_fn_t & get_gsl_fn( ) noexcept;
    
    static double _wrapper_fn( double x, void *param );
    static double _null_user_fn( double x, void *param );
protected:
    struct param_t{
        user_fn_t user_fn;
        void *user_data;
        param_t( user_fn_t _user_fn, void *_user_data)
        :user_fn(_user_fn), user_data(_user_data){ }
    };
    param_t _param;
    gsl_fn_t _gsl_fn;
};

inline double GSLFn::_wrapper_fn( double x, void *param ){
    param_t *p = (param_t *)param;
    return p->user_fn( x, p->user_data );
}
inline double GSLFn::_null_user_fn( double x, void *param ){
    ErrLogic::throw_( ErrLogic::eDEFAULT, emFLPFB,
        " user function not provided\n" );
    return 0.;
}

inline GSLFn::GSLFn( user_fn_t user_fn, void *data )
:_param( user_fn, data ){
    _gsl_fn.function = &_wrapper_fn;
    _gsl_fn.params = &_param;
}
inline GSLFn::GSLFn( const GSLFn & fn) 
:GSLFn( fn._param.user_fn, fn._param.user_data ){ }

inline GSLFn::GSLFn( GSLFn && fn)
:GSLFn( std::move(fn._param.user_fn), fn._param.user_data ){ }

inline GSLFn & GSLFn::operator=( const GSLFn & fn){
    if( this != &fn ){
        _param = fn._param;
    }
    return *this;
}

inline GSLFn & GSLFn::operator=( GSLFn && fn){
    if( this != &fn ){
        _param.user_fn = std::move( fn._param.user_fn );
        _param.user_data = fn._param.user_data;
    }
    return *this;
}

inline GSLFn::~GSLFn() noexcept{}

inline void GSLFn::set_fn( user_fn_t user_fn, void *data ){
    _param.user_fn = user_fn;
    _param.user_data = data;
}
inline double GSLFn::operator()( double x ){
    return _param.user_fn(x, _param.user_data);
}
inline GSLFn::user_fn_t & GSLFn::get_user_fn( void *&data ) noexcept{
    data = _param.user_data;
    return _param.user_fn;
}
inline GSLFn::gsl_fn_t & GSLFn::get_gsl_fn( ) noexcept{
    return _gsl_fn;
}


} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_GSL_UTIL_FN_H_