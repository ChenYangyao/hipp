#ifndef _HIPPNUMERICAL_MIN_H_
#define _HIPPNUMERICAL_MIN_H_
#include "function_base.h"
#include "gsl/gsl_min.h"
namespace HIPP::NUMERICAL {

/**
 * 1-d function minimizer.
 * 
 * Examples
 * ----------
 * // target function fn, bracketing range [0., 6.5], and a estimate of minimum 
 * // location 4.0.
 * auto fn = [](double x)->double{ return std::sin(x); };
 * FMinimizer opt(fn, 4.0, 0., 6.5);    
 * opt.iterate_until();
 * cout << "x_min = " << opt.x_minimum() << endl;
 */
class FMinimizer {
public:
    typedef GSLFn::user_fn_t fn_t;
    typedef double value_t;
    typedef gsl_min_fminimizer *raw_t;
    typedef const gsl_min_fminimizer_type *type_t;
    
    inline static const type_t
        GOLDEN_SECTION = gsl_min_fminimizer_goldensection,
        BRENT = gsl_min_fminimizer_brent,
        QUAD_GOLDEN = gsl_min_fminimizer_quad_golden;

    inline static const value_t 
        DEFAULT_EPSABS = 1.0e-5, 
        DEFAULT_EPSREL = 1.0e-5;
    inline static const size_t 
        DEFAULT_MAX_N_ITERS = 1024;

    inline static const int 
        SUCCESS = GSL_SUCCESS, 
        CONTINUE = GSL_CONTINUE;

    /**
     * (1)  initialize with a method typed T. It is necessary to set the target
     *      function to minimize later by set_fn().
     * (2) & (3) initialize with a method typed T. The minimization will be 
     *      taken at the target function fn. 
     *      @fn: any callable object (function or function pointer, lambda, 
     *          object with overloaded operator(), output of std::bind, ...).
     *      @x_min: an estimate of the minimum location. 
     *      @[x_lower, x_upper]: the bracket region. 
     * 
     * @T: may be GOLDDEN_SECTION, BRENT or QUAD_GOLDEN.
     *      
     * They must satisfy fn(x_lower) > fn(x_min) < fn(x_upper) so that the
     * bracketing is well defined, or otherwise an exception is thrown.
     */
    explicit FMinimizer(type_t T = BRENT);
    FMinimizer(fn_t fn, 
        value_t x_min, value_t x_lower, value_t x_upper, 
        type_t T = BRENT);
    FMinimizer(fn_t fn, 
        value_t x_min, value_t f_min, 
        value_t x_lower, value_t f_lower, 
        value_t x_upper, value_t f_upper,
        type_t T = BRENT);

    /**
     * Copy and move operations.
     * 
     * Copy is deep, i.e., the function is copied and the minimizer is a newly
     * allocated one with same type. 
     * The move operation moves the function, properly allocates the minimizer
     * with the same type.
     * 
     * If function is not empty (i.e., already set on construction or by 
     * set_fn()) after copying or moving, the bracketing range and the current
     * best-estimate minimum of the source object are used. 
     */
    FMinimizer(const FMinimizer &o);
    FMinimizer(FMinimizer &&o);

    ~FMinimizer();

    FMinimizer & operator=(const FMinimizer &o);
    FMinimizer & operator=(FMinimizer &&o);

    /**
     * @fmt_cntl: 0 for an inline priting, 1 for an verbose version, 2 for 
     *      a short displaying of the current optimization state.
     */
    ostream & info(ostream &os = cout, int fmt_cntl=1) const;
    friend ostream & operator<<(ostream &os, const FMinimizer &fmin);

    /**
     * Reset the method or the function to minimize.
     * 
     * If the function is already set, a call of set_type() changes the 
     * method, but the current optimization state is not changed, i.e.,
     * this can be used to get a mixture of methods.
     * 
     * A call of set_fn() always resets the optimization state, i.e., forget
     * the history.
     */
    FMinimizer & set_type(type_t T = BRENT);
    FMinimizer & set_fn(fn_t fn,
        value_t x_min, value_t x_lower, value_t x_upper);
    FMinimizer & set_fn(fn_t fn,
        value_t x_min, value_t f_min, 
        value_t x_lower, value_t f_lower, 
        value_t x_upper, value_t f_upper);

    string name() const;
    type_t type() const;
    raw_t raw() const;
    fn_t & fn() noexcept;
    const fn_t & fn() const noexcept;

    /**
     * iterate(): walk a single step.
     * 
     * iterate_until(): iterates until the stop criteria is achieved, i.e., 
     * already at maximal number of iterations or converged within 
     * epsabs and epsrel.
     * The number iterations actually performed is returned. If the returned 
     * is less than (i.e., "<") max_n_iters, convegence is achived, otherwise
     * the maximal number of iterations is achieved.
     * 
     * test(): check if the optimizer is currently converged.
     */
    FMinimizer & iterate();
    size_t iterate_until(size_t max_n_iters=DEFAULT_MAX_N_ITERS, 
        value_t epsabs=DEFAULT_EPSABS, value_t epsrel=DEFAULT_EPSREL);
    bool test(value_t epsabs=DEFAULT_EPSABS, 
        value_t epsrel=DEFAULT_EPSREL) const;
    static bool test(value_t a, value_t b, value_t epsabs, value_t epsrel);
    
    /**
     * Query the current best-estimate of minimum location, and the bracketing
     * range, or the function values at them.
     */
    value_t x_minimum() const;
    value_t x_lower() const;
    value_t x_upper() const;
    value_t f_minimum() const;
    value_t f_lower() const;
    value_t f_upper() const;
private:
    GSLFn _gsl_fn;
    type_t _fmin_type;
    raw_t _fmin;

    void _alloc();
    void _free();
    void _fmin_set(value_t x_min, value_t x_lower, value_t x_upper);
    void _fmin_set(value_t x_min, value_t f_min, 
        value_t x_lower, value_t f_lower, 
        value_t x_upper, value_t f_upper);
    void _fmin_set(const FMinimizer &o);
};


/* implementations */

inline FMinimizer::FMinimizer(type_t T) 
: _fmin_type(T)
{
    _alloc();
}
inline FMinimizer::FMinimizer(fn_t fn,
    value_t x_min, value_t x_lower, value_t x_upper, 
    type_t T)
:_gsl_fn(std::move(fn)), _fmin_type(T)
{
    _alloc();
    _fmin_set(x_min, x_lower, x_upper);    
}
inline FMinimizer::FMinimizer(fn_t fn,
    value_t x_min, value_t f_min, 
    value_t x_lower, value_t f_lower, 
    value_t x_upper, value_t f_upper,
    type_t T)
:_gsl_fn(std::move(fn)), _fmin_type(T)
{
    _alloc();
    _fmin_set(x_min, f_min, x_lower, f_lower, x_upper, f_upper);
}
inline FMinimizer::FMinimizer(const FMinimizer &o)
: _gsl_fn(o._gsl_fn), _fmin_type(o._fmin_type)
{
    _alloc();
    if( _gsl_fn )
        _fmin_set(o);
}
inline FMinimizer::FMinimizer(FMinimizer &&o)
: _gsl_fn(std::move(o._gsl_fn)), _fmin_type(o._fmin_type)
{
    _alloc();
    if( _gsl_fn )
        _fmin_set(o);
}
inline FMinimizer::~FMinimizer() {
    _free();
}
inline FMinimizer & FMinimizer::operator=(const FMinimizer &o) {
    if( this != &o ){
        _gsl_fn = o._gsl_fn;
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
inline FMinimizer & FMinimizer::operator=(FMinimizer &&o) {
    if( this != &o ){
        _gsl_fn = std::move(o._gsl_fn);
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
inline ostream & FMinimizer::info(ostream &os, int fmt_cntl) const {
    PStream ps(os);
    if(fmt_cntl == 0){
        ps << HIPPCNTL_CLASS_INFO_INLINE(FMinimizer), " method=", name();
    }else if(fmt_cntl == 1){
        ps << HIPPCNTL_CLASS_INFO(FMinimizer),
            "  Method: ", name(), '\n',
            "  Function: ", ( _gsl_fn?str((void *)&fn()):"not set" ), '\n',
            "  Estimated minimum: f(", x_minimum(), ")=", f_minimum(), 
            ", bracketed within: f(", x_lower(), ")=", f_lower(), 
            ", f(", x_upper(), ")=", f_upper(), endl;
    }else{
        ps << "f(", x_lower(), ", ", x_minimum(), ", ", x_upper(), ")=(", 
            f_lower(), ", ", f_minimum(), ", ", f_upper(), ")", endl;
    }
    return os;
}
inline ostream & operator<<(ostream &os, const FMinimizer &fmin) {
    return fmin.info(os);
}
inline FMinimizer & FMinimizer::set_type(type_t T) {
    if( T != _fmin_type ){
        if( _gsl_fn ){
            value_t _x_min = x_minimum(),
                _f_min = f_minimum(),
                _x_lo = x_lower(),
                _f_lo = f_lower(),
                _x_hi = x_upper(),
                _f_hi = f_upper();
            _free();
            _fmin_type = T;
            _alloc();
            _fmin_set(_x_min, _f_min, _x_lo, _f_lo, _x_hi, _f_hi);
        }else{
            _free();
            _fmin_type = T;
            _alloc();
        }
    }
    return *this;
}
inline FMinimizer & FMinimizer::set_fn(fn_t fn,
    value_t x_min, value_t x_lower, value_t x_upper)
{
    _gsl_fn.set_fn(std::move(fn));
    _fmin_set(x_min, x_lower, x_upper);
    return *this;
}
inline FMinimizer & FMinimizer::set_fn(fn_t fn,
    value_t x_min, value_t f_min, 
    value_t x_lower, value_t f_lower, 
    value_t x_upper, value_t f_upper)
{
    _gsl_fn.set_fn(std::move(fn));
    _fmin_set(x_min, f_min, x_lower, f_lower, x_upper, f_upper);
    return *this;
}

inline string FMinimizer::name() const {
    return string( gsl_min_fminimizer_name(_fmin) );
}
inline auto FMinimizer::type() const -> type_t { return _fmin_type; }
inline auto FMinimizer::raw() const -> raw_t { return _fmin; }
inline auto FMinimizer::fn() noexcept -> fn_t & { return _gsl_fn.get_user_fn(); }
inline auto FMinimizer::fn() const noexcept -> const fn_t & { 
    return _gsl_fn.get_user_fn(); 
}
inline FMinimizer & FMinimizer::iterate() {
    gsl_min_fminimizer_iterate(_fmin);
    return *this;
}
inline size_t FMinimizer::iterate_until(size_t max_n_iters, 
    value_t epsabs, value_t epsrel)
{
    size_t cnt = 0;
    while( cnt < max_n_iters ){
        bool pass = iterate().test(epsabs, epsrel);
        if( pass ) break;
        ++cnt;
    }
    return cnt;
}
inline bool FMinimizer::test(value_t epsabs, value_t epsrel) const{
    return test( x_lower(), x_upper(), epsabs, epsrel );
}
inline bool FMinimizer::test(value_t a, value_t b, 
    value_t epsabs, value_t epsrel)
{
    return gsl_min_test_interval(a, b, epsabs, epsrel) == GSL_SUCCESS;
}
inline auto FMinimizer::x_minimum() const -> value_t {
    return gsl_min_fminimizer_x_minimum(_fmin);
}
inline auto FMinimizer::x_lower() const -> value_t {
    return gsl_min_fminimizer_x_lower(_fmin);
}
inline auto FMinimizer::x_upper() const -> value_t {
    return gsl_min_fminimizer_x_upper(_fmin);
}
inline auto FMinimizer::f_minimum() const -> value_t {
    return gsl_min_fminimizer_f_minimum(_fmin);
}
inline auto FMinimizer::f_lower() const -> value_t {
    return gsl_min_fminimizer_f_lower(_fmin);
}
inline auto FMinimizer::f_upper() const -> value_t {
    return gsl_min_fminimizer_f_upper(_fmin);
}
inline void FMinimizer::_alloc() {
    _fmin = gsl_min_fminimizer_alloc(_fmin_type);
}
inline void FMinimizer::_free() {
    gsl_min_fminimizer_free(_fmin);
}
inline void FMinimizer::_fmin_set(value_t x_min, value_t x_lower, 
    value_t x_upper)
{
    gsl_min_fminimizer_set( _fmin, &_gsl_fn.get_gsl_fn(), 
        x_min, x_lower, x_upper );
}
inline void FMinimizer::_fmin_set(value_t x_min, value_t f_min, 
    value_t x_lower, value_t f_lower, 
    value_t x_upper, value_t f_upper)
{
    gsl_min_fminimizer_set_with_values(_fmin, &_gsl_fn.get_gsl_fn(),
        x_min, f_min, x_lower, f_lower, x_upper, f_upper);
}
inline void FMinimizer::_fmin_set(const FMinimizer &o) {
    _fmin_set(o.x_minimum(), o.f_minimum(), o.x_lower(), o.f_lower(), 
        o.x_upper(), o.f_upper());
}


} // namespace HIPP::NUMERICAL
#endif	//_HIPPNUMERICAL_MIN_H_