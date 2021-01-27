#ifndef _HIPPNUMERICAL_MULTIMIN_H_
#define _HIPPNUMERICAL_MULTIMIN_H_
#include "function_base.h"
#include <gsl/gsl_multimin.h>

namespace HIPP::NUMERICAL {

class FMinimizerND {
public:
    typedef GSLMultiminFn::user_fn_t fn_t;
    typedef GSLMultiminFn::value_t value_t;
    typedef GSLMultiminFn::vec_t vec_t;
    typedef gsl_multimin_fminimizer *raw_t;
    typedef const gsl_multimin_fminimizer_type *type_t;

    inline static const type_t 
        NMSIMPLEX = gsl_multimin_fminimizer_nmsimplex,
        NMSIMPLEX2 = gsl_multimin_fminimizer_nmsimplex2,
        NMSIMPLEX2_RAND = gsl_multimin_fminimizer_nmsimplex2rand;
    inline static const value_t 
        DEFAULT_EPSABS = 1.0e-5;
    inline static const size_t 
        DEFAULT_MAX_N_ITERS = 1024;
    inline static const int 
        SUCCESS = GSL_SUCCESS, CONTINUE = GSL_CONTINUE;

    FMinimizerND(size_t dim, type_t T = NMSIMPLEX2);
    FMinimizerND(fn_t fn, const vec_t &x, const vec_t &step_size,
        type_t T = NMSIMPLEX2);
    FMinimizerND(fn_t fn, const vec_t &x, value_t step_size, 
        type_t T = NMSIMPLEX2);
    FMinimizerND(const FMinimizerND &);
    FMinimizerND(FMinimizerND &&);
    ~FMinimizerND();

    FMinimizerND & operator=(const FMinimizerND &);
    FMinimizerND & operator=(FMinimizerND &&);

    ostream & info(ostream &os = cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const FMinimizerND &fmin);

    FMinimizerND & set_type(type_t T = NMSIMPLEX2);
    FMinimizerND & set_fn(fn_t fn, const vec_t &x, const vec_t &step_size);
    FMinimizerND & set_fn(fn_t fn, const vec_t &x, value_t step_size);

    size_t dim() const;
    string name() const;
    type_t type() const;
    raw_t raw() const;
    fn_t & fn() noexcept;
    const fn_t & fn() const noexcept;

    FMinimizerND & iterate();
    size_t iterate_until(size_t max_n_iters=DEFAULT_MAX_N_ITERS, 
        value_t epsabs=DEFAULT_EPSABS);
    bool test(value_t epsabs=DEFAULT_EPSABS) const;
    static bool test(value_t size, value_t epsabs);

    vec_t x_minimum() const;
    value_t f_minimum() const;
    value_t size() const;
private:
    GSLMultiminFn _gsl_fn;
    size_t _dim;
    type_t _fmin_type;
    raw_t  _fmin;

    void _alloc();
    void _free();
    void _fmin_set(const vec_t &x, const vec_t &step_size);
    void _fmin_set(const vec_t &x, value_t step_size);
    void _fmin_set(const FMinimizerND &o);
};


inline FMinimizerND::FMinimizerND(size_t dim, type_t T)
: _dim(dim), _fmin_type(T)
{
    _alloc();
}
inline FMinimizerND::FMinimizerND(fn_t fn, const vec_t &x, 
    const vec_t &step_size, type_t T)
: _gsl_fn(x.size(), std::move(fn)), _dim(x.size()), _fmin_type(T)
{
    _alloc();
    _fmin_set(x, step_size);
}
inline FMinimizerND::FMinimizerND(fn_t fn, const vec_t &x, 
    value_t step_size, type_t T)
: _gsl_fn(x.size(), std::move(fn)), _dim(x.size()), _fmin_type(T)
{
    _alloc();
    _fmin_set(x, step_size);
}
inline FMinimizerND::FMinimizerND(const FMinimizerND &o)
: _gsl_fn(o._gsl_fn), _dim(o._dim), _fmin_type(o._fmin_type)
{
    _alloc();
    if( _gsl_fn )
        _fmin_set(o);
}
inline FMinimizerND::FMinimizerND(FMinimizerND &&o)
: _gsl_fn(std::move(o._gsl_fn)), _dim(o._dim), _fmin_type(o._fmin_type)
{
    _alloc();
    if( _gsl_fn )
        _fmin_set(o);
}
inline FMinimizerND::~FMinimizerND(){
    _free();
}
inline FMinimizerND & FMinimizerND::operator=(const FMinimizerND &o){
    if( this != &o ){
        _gsl_fn = o._gsl_fn;
        _dim = o._dim;
        if( _fmin_type != o._fmin_type ){
            _free();
            _fmin_type = o._fmin_type;
            _alloc();
        }
        if( _gsl_fn )
            _fmin_set(o);
    }
    return *this;
}
inline FMinimizerND & FMinimizerND::operator=(FMinimizerND &&o){
    if( this != &o ){
        _gsl_fn = std::move(o._gsl_fn);
        _dim = o._dim;
        if( _fmin_type != o._fmin_type ){
            _free();
            _fmin_type = o._fmin_type;
            _alloc();
        }
        if( _gsl_fn )
            _fmin_set(o);
    }
    return *this;
}
inline ostream & FMinimizerND::info(ostream &os, int fmt_cntl) const{
    PStream ps(os);
    if(fmt_cntl == 0){
        ps << HIPPCNTL_CLASS_INFO_INLINE(FMinimizerND), 
            " method=", name(), ", dim=", dim();
    }else if(fmt_cntl == 1){
        ps << HIPPCNTL_CLASS_INFO(FMinimizerND),
            "  Method: ", name(), ", dim: ", dim(), '\n',
            "  Function: ", ( _gsl_fn?str((void *)&fn()):"not set" ), '\n',
            "  Estimated minimum: f(", x_minimum().to_stl(), ")=", f_minimum(), 
                ", eps=", size(), endl;
    }else{
        ps << "f(", x_minimum().to_stl(), " +/- ", 
            size(), ")=", f_minimum(), endl;
    }
    return os;
}
inline ostream & operator<<(ostream &os, const FMinimizerND &fmin){
    return fmin.info(os);
}
inline FMinimizerND & FMinimizerND::set_type(type_t T){
    if( T != _fmin_type ){
        if( _gsl_fn ){
            value_t s = size();
            vec_t x = x_minimum();
            _free();
            _fmin_type = T;
            _alloc();
            _fmin_set(x, s);
        }else{
            _free();
            _fmin_type = T;
            _alloc();
        }
    }
    return *this;
}
inline FMinimizerND & FMinimizerND::set_fn(fn_t fn, const vec_t &x, 
    const vec_t &step_size)
{
    size_t dim = x.size();
    _gsl_fn.set_fn(dim, std::move(fn));
    if( _dim != dim ){
        _free();
        _dim = dim;
        _alloc();
    }
    _fmin_set(x, step_size);
    return *this;
}
inline FMinimizerND & FMinimizerND::set_fn(fn_t fn, const vec_t &x, 
    value_t step_size)
{
    vec_t s(x.size());
    s.set_all(step_size);
    return set_fn(std::move(fn), x, s);
}
inline size_t FMinimizerND::dim() const {
    return _dim;
}
inline string FMinimizerND::name() const{
    return string( gsl_multimin_fminimizer_name(_fmin) );
}
inline auto FMinimizerND::type() const -> type_t{
    return _fmin_type;
}
inline auto FMinimizerND::raw() const -> raw_t{
    return _fmin;
}
inline auto FMinimizerND::fn() noexcept -> fn_t &{
    return _gsl_fn.get_user_fn();
}
inline auto  FMinimizerND::fn() const noexcept -> const fn_t & {
    return _gsl_fn.get_user_fn();
}
inline FMinimizerND & FMinimizerND::iterate(){
    gsl_multimin_fminimizer_iterate(_fmin);
    return *this;
}
inline size_t FMinimizerND::iterate_until(size_t max_n_iters, value_t epsabs){
    size_t cnt = 0;
    while( cnt < max_n_iters ){
        bool pass = iterate().test(epsabs);
        if(pass) break;
        ++ cnt;
    }
    return cnt;
}
inline bool FMinimizerND::test(value_t epsabs) const{
    return test(size(), epsabs);
}
inline bool FMinimizerND::test(value_t size, value_t epsabs){
    return gsl_multimin_test_size(size, epsabs) == GSL_SUCCESS;
}
inline auto FMinimizerND::x_minimum() const -> vec_t{
    return vec_t(gsl_multimin_fminimizer_x(_fmin), vec_t::COPY);
}
inline auto FMinimizerND::f_minimum() const -> value_t{
    return gsl_multimin_fminimizer_minimum(_fmin);
}
inline auto FMinimizerND::size() const -> value_t{
    return gsl_multimin_fminimizer_size(_fmin);
}
inline void FMinimizerND::_alloc(){
    _fmin = gsl_multimin_fminimizer_alloc(_fmin_type, _dim);
}
inline void FMinimizerND::_free(){
    gsl_multimin_fminimizer_free(_fmin);
}
inline void FMinimizerND::_fmin_set(const vec_t &x, const vec_t &step_size){
    gsl_multimin_fminimizer_set(_fmin, &_gsl_fn.get_gsl_fn(), 
        x.raw(), step_size.raw());
}
inline void FMinimizerND::_fmin_set(const vec_t &x, value_t step_size){
    vec_t s(x.size());
    s.set_all(step_size);
    _fmin_set(x, s);
}
inline void FMinimizerND::_fmin_set(const FMinimizerND &o){
    _fmin_set(o.x_minimum(), o.size());
}

} // namespace HIPP::NUMERICAL

#endif	//_HIPPNUMERICAL_MULTIMIN_H_

