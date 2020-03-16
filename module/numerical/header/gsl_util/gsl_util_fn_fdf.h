#ifndef _HIPPNUMERICAL_GSL_UTIL_FN_FDF_H_
#define _HIPPNUMERICAL_GSL_UTIL_FN_FDF_H_
#include "gsl_util_base.h"
#include "gsl_util_error.h"
#include "gsl/gsl_math.h"
namespace HIPP{
namespace NUMERICAL{

class GSLFnFDF{
public:
    typedef std::function< double(double x, void *data) > user_fn_f_t;
    typedef user_fn_f_t user_fn_df_t;
    typedef std::function< void(double x, void *data, double &f, double &df) >
        user_fn_fdf_t;
    typedef gsl_function_fdf gsl_fn_t;

    GSLFnFDF( user_fn_f_t user_fn_f = &_null_user_fn_f, 
        user_fn_df_t user_fn_df = &_null_user_fn_df, 
        user_fn_fdf_t user_fn_fdf = &_null_user_fn_fdf, 
        void *data = nullptr );
    
    GSLFnFDF( const GSLFnFDF & );
    GSLFnFDF( GSLFnFDF && );
    GSLFnFDF & operator=( const GSLFnFDF & );
    GSLFnFDF & operator=( GSLFnFDF && );
    ~GSLFnFDF() noexcept;

    void set_fn( user_fn_f_t user_fn_f = &_null_user_fn_f, 
        user_fn_df_t user_fn_df = &_null_user_fn_df, 
        user_fn_fdf_t user_fn_fdf = &_null_user_fn_fdf, 
        void *data = nullptr );

    double eval_f( double x );
    double eval_df( double x );
    void eval_fdf( double &x, double &f, double &df );

    user_fn_f_t & get_user_fn_f( void *&data ) noexcept;
    user_fn_df_t & get_user_fn_df( void *&data ) noexcept;
    user_fn_fdf_t & get_user_fn_fdf( void *&data ) noexcept;
    gsl_fn_t & get_gsl_fn( ) noexcept;
protected:
    struct param_t{
        user_fn_f_t user_fn_f;
        user_fn_df_t user_fn_df;
        user_fn_fdf_t user_fn_fdf;
        void *user_data;
        param_t( user_fn_f_t _user_fn_f, 
            user_fn_df_t _user_fn_df, user_fn_fdf_t _user_fn_fdf, 
            void *_user_data)
        :user_fn_f(_user_fn_f), user_fn_df(_user_fn_df), 
        user_fn_fdf(_user_fn_fdf), user_data(_user_data){ }
    };
    param_t _param;
    gsl_fn_t _gsl_fn;
    static double _wrapper_fn_f( double x, void *param );
    static double _wrapper_fn_df( double x, void *param );
    static void _wrapper_fn_fdf( 
            double x, void *param, double *f, double *df );
    static double _null_user_fn_f( double x, void *param );
    static double _null_user_fn_df( double x, void *param );
    static void _null_user_fn_fdf( double x, void *param, 
        double &f, double &df );
};


inline GSLFnFDF::GSLFnFDF( user_fn_f_t user_fn_f, 
    user_fn_df_t user_fn_df, 
    user_fn_fdf_t user_fn_fdf, 
    void *data )
:_param( user_fn_f, user_fn_df, user_fn_fdf, data )    
{
    _gsl_fn.f = &_wrapper_fn_f;
    _gsl_fn.df = &_wrapper_fn_df;
    _gsl_fn.fdf = &_wrapper_fn_fdf;
    _gsl_fn.params = &_param;
}

inline GSLFnFDF::GSLFnFDF( const GSLFnFDF & fn)
:GSLFnFDF( fn._param.user_fn_f, fn._param.user_fn_df, fn._param.user_fn_fdf, 
fn._param.user_data ){ }

inline GSLFnFDF::GSLFnFDF( GSLFnFDF && fn)
:GSLFnFDF( std::move(fn._param.user_fn_f), 
    std::move(fn._param.user_fn_df), std::move(fn._param.user_fn_fdf), 
    fn._param.user_data )
{ }

inline GSLFnFDF & GSLFnFDF::operator=( const GSLFnFDF & fn){
    if( this != &fn ){
        _param = fn._param;
    }
    return *this;
}

inline GSLFnFDF & GSLFnFDF::operator=( GSLFnFDF && fn){
    if( this != &fn ){
        _param.user_fn_f = std::move( fn._param.user_fn_f );
        _param.user_fn_df = std::move( fn._param.user_fn_df );
        _param.user_fn_fdf = std::move( fn._param.user_fn_fdf );
        _param.user_data = fn._param.user_data;
    }
    return *this;
}

inline GSLFnFDF::~GSLFnFDF() noexcept{ }

inline void GSLFnFDF::set_fn( user_fn_f_t user_fn_f, 
    user_fn_df_t user_fn_df, 
    user_fn_fdf_t user_fn_fdf, 
    void *data ){
    _param.user_fn_f = user_fn_f;
    _param.user_fn_df = user_fn_df;
    _param.user_fn_fdf = user_fn_fdf;
    _param.user_data = data;
}

inline double GSLFnFDF::eval_f( double x ){
    return _param.user_fn_f(x, _param.user_data);
}
inline double GSLFnFDF::eval_df( double x ){
    return _param.user_fn_df(x, _param.user_data);
}
inline void GSLFnFDF::eval_fdf( double &x, double &f, double &df ){
    _param.user_fn_fdf(x, _param.user_data, f, df);
}

inline GSLFnFDF::user_fn_f_t & GSLFnFDF::get_user_fn_f( void *&data ) noexcept{
    data= _param.user_data;
    return _param.user_fn_f;
}
inline GSLFnFDF::user_fn_df_t & 
GSLFnFDF::get_user_fn_df( void *&data ) noexcept{
    data= _param.user_data;
    return _param.user_fn_df;
}
inline GSLFnFDF::user_fn_fdf_t & 
GSLFnFDF::get_user_fn_fdf( void *&data ) noexcept{
    data= _param.user_data;
    return _param.user_fn_fdf;
}
inline GSLFnFDF::gsl_fn_t & GSLFnFDF::get_gsl_fn( ) noexcept{
    return _gsl_fn;
}

inline double GSLFnFDF::_wrapper_fn_f( double x, void *param ){
    param_t *p = (param_t *)param;
    return p->user_fn_f( x, p->user_data );
}
inline double GSLFnFDF::_wrapper_fn_df( double x, void *param ){
    param_t *p = (param_t *)param;
    return p->user_fn_df( x, p->user_data );
}
inline void GSLFnFDF::_wrapper_fn_fdf( double x, void *param, 
    double *f, double *df )
{
    param_t *p = (param_t *)param;
    return p->user_fn_fdf( x, p->user_data, *f, *df );
}

inline double GSLFnFDF::_null_user_fn_f( double x, void *param ){
    ErrLogic::throw_( ErrLogic::eDEFAULT, emFLPFB,
        " user function f not provided\n" );
    return 0.;
}
inline double GSLFnFDF::_null_user_fn_df( double x, void *param ){
    ErrLogic::throw_( ErrLogic::eDEFAULT, emFLPFB,
        " user function df not provided\n" );
    return 0.;
}
inline void GSLFnFDF::_null_user_fn_fdf( double x, void *param, 
    double &f, double &df ){
    ErrLogic::throw_( ErrLogic::eDEFAULT, emFLPFB,
        " user function fdf not provided\n" );
}


} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_GSL_UTIL_FN_FDF_H_