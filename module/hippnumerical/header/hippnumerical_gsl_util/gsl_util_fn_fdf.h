#ifndef _HIPPNUMERICAL_GSL_UTIL_FN_FDF_H_
#define _HIPPNUMERICAL_GSL_UTIL_FN_FDF_H_
#include "gsl_util_base.h"
#include "gsl_util_error.h"
#include "gsl/gsl_math.h"
namespace HIPP{
namespace NUMERICAL{

class GSLFnFDF{
public:
    typedef double value_t;
    typedef std::function< value_t(value_t x) > user_fn_f_t;
    typedef user_fn_f_t user_fn_df_t;
    typedef std::function< void(value_t x, value_t &f, value_t &df) >
        user_fn_fdf_t;
    typedef gsl_function_fdf gsl_fn_t;

    GSLFnFDF() noexcept;
    GSLFnFDF( user_fn_f_t user_fn_f, user_fn_df_t user_fn_df, 
        user_fn_fdf_t user_fn_fdf);
    
    GSLFnFDF( const GSLFnFDF & );
    GSLFnFDF( GSLFnFDF && );
    GSLFnFDF & operator=( const GSLFnFDF & );
    GSLFnFDF & operator=( GSLFnFDF && );
    ~GSLFnFDF();

    GSLFnFDF & set_fn() noexcept;
    GSLFnFDF & set_fn(user_fn_f_t user_fn_f, user_fn_df_t user_fn_df, 
        user_fn_fdf_t user_fn_fdf);

    value_t eval_f(value_t x) const;
    value_t eval_df(value_t x) const;
    void eval_fdf(value_t x, value_t &f, value_t &df) const;

    user_fn_f_t & get_user_fn_f() noexcept;
    user_fn_df_t & get_user_fn_df() noexcept;
    user_fn_fdf_t & get_user_fn_fdf() noexcept;
    gsl_fn_t & get_gsl_fn() noexcept;

    const user_fn_f_t & get_user_fn_f() const noexcept;
    const user_fn_df_t & get_user_fn_df() const noexcept;
    const user_fn_fdf_t & get_user_fn_fdf() const noexcept;
    const gsl_fn_t & get_gsl_fn() const noexcept;
protected:
    user_fn_f_t _user_fn_f;
    user_fn_df_t _user_fn_df;
    user_fn_fdf_t _user_fn_fdf;
    gsl_fn_t _gsl_fn;
    static value_t _wrapper_fn_f( value_t x, void *param );
    static value_t _wrapper_fn_df( value_t x, void *param );
    static void _wrapper_fn_fdf( 
            value_t x, void *param, value_t *f, value_t *df );
};

inline GSLFnFDF::GSLFnFDF() noexcept: 
GSLFnFDF(user_fn_f_t(), user_fn_df_t(), user_fn_fdf_t())
{}

inline GSLFnFDF::GSLFnFDF( user_fn_f_t user_fn_f, 
    user_fn_df_t user_fn_df, 
    user_fn_fdf_t user_fn_fdf)
: _user_fn_f(std::move(user_fn_f)), _user_fn_df(std::move(user_fn_df)), 
_user_fn_fdf(std::move(user_fn_fdf))
{
    _gsl_fn.f = &_wrapper_fn_f;
    _gsl_fn.df = &_wrapper_fn_df;
    _gsl_fn.fdf = &_wrapper_fn_fdf;
    _gsl_fn.params = this;
}

inline GSLFnFDF::GSLFnFDF( const GSLFnFDF & fn)
:GSLFnFDF(fn._user_fn_f, fn._user_fn_df, fn._user_fn_fdf){}

inline GSLFnFDF::GSLFnFDF( GSLFnFDF && fn)
:GSLFnFDF(std::move(fn._user_fn_f), std::move(fn._user_fn_df), 
std::move(fn._user_fn_fdf)){}

inline GSLFnFDF & GSLFnFDF::operator=( const GSLFnFDF & fn){
    _user_fn_f = fn._user_fn_f;
    _user_fn_df = fn._user_fn_df;
    _user_fn_fdf = fn._user_fn_fdf;
    return *this;
}

inline GSLFnFDF & GSLFnFDF::operator=( GSLFnFDF && fn){
    _user_fn_f = std::move(fn._user_fn_f);
    _user_fn_df = std::move(fn._user_fn_df);
    _user_fn_fdf = std::move(fn._user_fn_fdf);
    return *this;
}

inline GSLFnFDF::~GSLFnFDF(){ }

inline GSLFnFDF & GSLFnFDF::set_fn( user_fn_f_t user_fn_f, 
    user_fn_df_t user_fn_df, user_fn_fdf_t user_fn_fdf)
{
    _user_fn_f = std::move(user_fn_f);
    _user_fn_df = std::move(user_fn_df);
    _user_fn_fdf = std::move(user_fn_fdf);
    return *this;
}

inline GSLFnFDF & GSLFnFDF::set_fn() noexcept {
    return set_fn(user_fn_f_t(), user_fn_df_t(), user_fn_fdf_t());
}

inline auto GSLFnFDF::eval_f( value_t x ) const -> value_t {
    return _user_fn_f(x);
}
inline auto GSLFnFDF::eval_df( value_t x ) const -> value_t {
    return _user_fn_df(x);
}
inline void GSLFnFDF::eval_fdf( value_t x, value_t &f, value_t &df ) const {
    _user_fn_fdf(x, f, df);
}

inline GSLFnFDF::user_fn_f_t & GSLFnFDF::get_user_fn_f() noexcept{
    return _user_fn_f;
}
inline GSLFnFDF::user_fn_df_t & 
GSLFnFDF::get_user_fn_df() noexcept{
    return _user_fn_df;
}
inline GSLFnFDF::user_fn_fdf_t & 
GSLFnFDF::get_user_fn_fdf() noexcept{
    return _user_fn_fdf;
}
inline GSLFnFDF::gsl_fn_t & GSLFnFDF::get_gsl_fn( ) noexcept{
    return _gsl_fn;
}

inline const GSLFnFDF::user_fn_f_t & GSLFnFDF::get_user_fn_f() const noexcept{
    return _user_fn_f;
}
inline const GSLFnFDF::user_fn_df_t & 
GSLFnFDF::get_user_fn_df() const noexcept{
    return _user_fn_df;
}
inline const GSLFnFDF::user_fn_fdf_t & 
GSLFnFDF::get_user_fn_fdf() const noexcept{
    return _user_fn_fdf;
}
inline const GSLFnFDF::gsl_fn_t & GSLFnFDF::get_gsl_fn( ) const noexcept{
    return _gsl_fn;
}

inline auto GSLFnFDF::_wrapper_fn_f( value_t x, void *param ) -> value_t {
    const auto &f = *reinterpret_cast<const GSLFnFDF *>(param);
    return f.eval_f(x);
}
inline auto GSLFnFDF::_wrapper_fn_df( value_t x, void *param ) -> value_t {
    const auto &f = *reinterpret_cast<const GSLFnFDF *>(param);
    return f.eval_df(x);
}
inline void GSLFnFDF::_wrapper_fn_fdf( value_t x, void *param, 
    value_t *f, value_t *df )
{
    const auto &fn = *reinterpret_cast<const GSLFnFDF *>(param);
    fn.eval_fdf(x, *f, *df);
}


} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_GSL_UTIL_FN_FDF_H_