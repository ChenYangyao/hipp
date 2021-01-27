#ifndef _HIPPNUMERICAL_GSL_UTIL_FN_H_
#define _HIPPNUMERICAL_GSL_UTIL_FN_H_
#include "gsl_util_base.h"
#include "gsl_util_error.h"
#include "gsl/gsl_math.h"
namespace HIPP{
namespace NUMERICAL{

class GSLFn{
public:
    typedef double value_t;
    typedef std::function< value_t(value_t x) > user_fn_t;
    typedef gsl_function gsl_fn_t;

    GSLFn(user_fn_t user_fn = user_fn_t());
    GSLFn( const GSLFn & );
    GSLFn( GSLFn && );
    ~GSLFn();
    
    GSLFn & operator=( const GSLFn & );
    GSLFn & operator=( GSLFn && );

    GSLFn & set_fn(user_fn_t user_fn = user_fn_t());

    value_t operator()(value_t x) const;

    user_fn_t & get_user_fn() noexcept;
    gsl_fn_t & get_gsl_fn() noexcept;
    const user_fn_t & get_user_fn() const noexcept;
    const gsl_fn_t & get_gsl_fn() const noexcept;

    bool empty() const noexcept;
    explicit operator bool () const noexcept;
    
    static value_t _wrapper_fn(value_t x, void *param);
protected:
    user_fn_t _user_fn;
    gsl_fn_t _gsl_fn;
};

inline GSLFn::GSLFn( user_fn_t user_fn)
: _user_fn( std::move(user_fn) ){
    _gsl_fn.function = &_wrapper_fn;
    _gsl_fn.params = this;
}

inline GSLFn::GSLFn(const GSLFn & fn) 
:GSLFn(fn._user_fn){}

inline GSLFn::GSLFn( GSLFn && fn)
:GSLFn( std::move(fn._user_fn) ){ }

inline GSLFn & GSLFn::operator=( const GSLFn & fn){
    _user_fn = fn._user_fn;
    return *this;
}

inline GSLFn & GSLFn::operator=( GSLFn && fn){
    _user_fn = std::move(fn._user_fn);
    return *this;
}

inline GSLFn::~GSLFn(){}

inline GSLFn & GSLFn::set_fn( user_fn_t user_fn ){
    _user_fn = std::move(user_fn);
    return *this;
}

inline auto GSLFn::operator()( value_t x ) const -> value_t {
    return _user_fn(x);
}

inline GSLFn::user_fn_t & GSLFn::get_user_fn() noexcept{
    return _user_fn;
}

inline GSLFn::gsl_fn_t & GSLFn::get_gsl_fn() noexcept{
    return _gsl_fn;
}

inline const GSLFn::user_fn_t & GSLFn::get_user_fn() const noexcept{
    return _user_fn;
}

inline const GSLFn::gsl_fn_t & GSLFn::get_gsl_fn() const noexcept{
    return _gsl_fn;
}

inline bool GSLFn::empty() const noexcept {
    return ! static_cast<bool>(_user_fn);
}
inline GSLFn::operator bool () const noexcept {
    return static_cast<bool>(_user_fn);
}
inline auto GSLFn::_wrapper_fn( value_t x, void *param ) -> value_t {
    const auto &f = *reinterpret_cast<const GSLFn *>(param);
    return f(x);
}

} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_GSL_UTIL_FN_H_