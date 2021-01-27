#ifndef _HIPPNUMERICAL_GSL_UTIL_MULTIMIN_FN_H_
#define _HIPPNUMERICAL_GSL_UTIL_MULTIMIN_FN_H_
#include "gsl_util_base.h"
#include "gsl_util_error.h"
#include "gsl_util_vector.h"
#include "gsl/gsl_multimin.h"

namespace HIPP::NUMERICAL {

class GSLMultiminFn {
public:
    typedef double value_t;
    typedef GSLVector<value_t> vec_t;
    typedef std::function< double(const vec_t &x) > user_fn_t;
    typedef gsl_multimin_function gsl_fn_t;

    GSLMultiminFn(size_t n = 0, user_fn_t user_fn = user_fn_t());
    GSLMultiminFn(const GSLMultiminFn &);
    GSLMultiminFn(GSLMultiminFn &&);
    ~GSLMultiminFn();
    GSLMultiminFn & operator=(const GSLMultiminFn &);
    GSLMultiminFn & operator=(GSLMultiminFn &&);

    GSLMultiminFn & set_fn(size_t n, user_fn_t user_fn = user_fn_t());
    value_t operator()(const vec_t &x) const;

    user_fn_t & get_user_fn() noexcept;
    gsl_fn_t & get_gsl_fn() noexcept;
    const user_fn_t & get_user_fn() const noexcept;
    const gsl_fn_t & get_gsl_fn() const noexcept;
    size_t dim() const noexcept;

    bool empty() const noexcept;
    explicit operator bool () const noexcept;

    static value_t _wrapper_fn(vec_t::craw_t x, void *params);
protected:
    user_fn_t _user_fn;
    gsl_fn_t _gsl_fn;
};

inline GSLMultiminFn::GSLMultiminFn(size_t n, user_fn_t user_fn) 
: _user_fn(std::move(user_fn))
{
    _gsl_fn.f = &_wrapper_fn;
    _gsl_fn.n = n;
    _gsl_fn.params = this;
}
inline GSLMultiminFn::GSLMultiminFn(const GSLMultiminFn &o)
: GSLMultiminFn(o.dim(), o._user_fn){}
inline GSLMultiminFn::GSLMultiminFn(GSLMultiminFn &&o)
: GSLMultiminFn(o.dim(), std::move(o._user_fn)){}
inline GSLMultiminFn::~GSLMultiminFn(){}
inline GSLMultiminFn & GSLMultiminFn::operator=(const GSLMultiminFn &o){
    _user_fn = o._user_fn;
    _gsl_fn.n = o._gsl_fn.n;
    return *this;
}
inline GSLMultiminFn & GSLMultiminFn::operator=(GSLMultiminFn &&o){
    _user_fn = std::move(o._user_fn);
    _gsl_fn.n = o._gsl_fn.n;
    return *this;
}
inline GSLMultiminFn & GSLMultiminFn::set_fn(size_t n, user_fn_t user_fn) {
    _user_fn = std::move(user_fn);
    _gsl_fn.n = n;
    return *this;
}
inline auto GSLMultiminFn::operator()(const vec_t &x) const -> value_t {
    return _user_fn(x);
}
inline auto GSLMultiminFn::get_user_fn() noexcept -> user_fn_t & {
    return _user_fn;
}
inline auto GSLMultiminFn::get_gsl_fn() noexcept -> gsl_fn_t & {
    return _gsl_fn;
}
inline auto GSLMultiminFn::get_user_fn() const noexcept -> const user_fn_t & {
    return _user_fn;
}
inline auto GSLMultiminFn::get_gsl_fn() const noexcept -> const gsl_fn_t & {
    return _gsl_fn;
}
inline size_t GSLMultiminFn::dim() const noexcept {
    return _gsl_fn.n;
}

inline bool GSLMultiminFn::empty() const noexcept {
    return !static_cast<bool>(_user_fn);
}
inline GSLMultiminFn::operator bool () const noexcept {
    return static_cast<bool>(_user_fn);
}

inline auto GSLMultiminFn::_wrapper_fn(vec_t::craw_t x, void *params) 
-> value_t 
{
    const auto &f = *reinterpret_cast<const GSLMultiminFn *>(params);
    vec_t vec(x);
    return f(vec);
}

} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_GSL_UTIL_MULTIMIN_FN_H_