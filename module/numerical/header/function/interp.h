#ifndef _HIPPNUMERICAL_INTERP_H_
#define _HIPPNUMERICAL_INTERP_H_
#include "function_base.h"
#include "gsl/gsl_interp.h"
#include "gsl/gsl_spline.h"
namespace HIPP{
namespace NUMERICAL{

namespace _interp_helper{

class BaseInterp {
public:
    typedef gsl_interp_type interp_type_t;
    typedef gsl_interp _interp_t;
    typedef gsl_spline _spline_t;
    typedef gsl_interp_accel _accel_t;

    typedef double value_t;
    typedef std::size_t size_t;

    static const interp_type_t 
        * const LINEAR, 
        * const POLYNOMIAL,
        * const CSPLINE,
        * const CSPLINE_PERIODIC,
        * const AKIMA,
        * const AKIMA_PERIODIC,
        * const STEFFEN;
};

class Accel {
public:
    typedef BaseInterp::_accel_t _accel_t;
    Accel();
    ~Accel();

    /* reset the state of accel (i.e., forget previous memory) */
    Accel & reset();
    
    _accel_t * get() noexcept;
protected:
    _accel_t *_accel;
};

class Spline : public BaseInterp {
public:
    Spline();
    Spline( const double *x, const double *y, size_t size, 
        const interp_type_t *type = LINEAR );
    Spline( const vector<double> &x, const vector<double> &y, 
        const interp_type_t *type = LINEAR );
    ~Spline();
    _HIPPNUMERICAL_GSL_UTIL_DELETE_CP_MV(Spline)

    /**
     * 1. reset to an empty state.
     * 2 or 3. reinitialize the spline.
     * 
     * These can be called at any time, or even multiple times.
     */
    Spline & reset();
    Spline & reset( const double *x, const double *y, size_t size,
        const interp_type_t *type = LINEAR );
    Spline & reset( const vector<double> &x, const vector<double> &y,
        const interp_type_t *type = LINEAR );

    string name() const;
    static size_t min_size( const interp_type_t *type );
    size_t min_size();
    bool empty() const noexcept;
    int last_errno() const noexcept;

    double operator()( double x );
    double deriv( double x );
    double deriv2( double x );
    double integ( double a, double b );
protected:
    Accel _accel;
    _spline_t *_spline;
    int _last_errno;

    /* can be call at any time, or even multiple times */
    void _free_spline();
    /* only call it when empty */
    void _init_spline( const double *x, const double *y, size_t size,
        const interp_type_t *type = LINEAR );
};
} // namespace _interp_helper

using _interp_helper::Spline;








/* implementations */
namespace _interp_helper {

inline Accel::Accel(): _accel(){
    _accel = gsl_interp_accel_alloc();
}
inline Accel::~Accel(){
    gsl_interp_accel_free( _accel );
}
inline Accel & Accel::reset(){
    gsl_interp_accel_reset( _accel );
    return *this;
}
inline Accel::_accel_t * Accel::get() noexcept{
    return _accel;
}

inline Spline::Spline(): _spline(nullptr), _last_errno(0){}

inline Spline::Spline( const double *x, const double *y, size_t size,
    const interp_type_t *type )
:Spline()
{
    _init_spline(x, y, size, type);
}

inline Spline::Spline( const vector<double> &x, const vector<double> &y, 
    const interp_type_t *type )
:Spline(x.data(), y.data(), x.size(), type)
{  }

inline Spline::~Spline(){ _free_spline(); }

inline Spline & Spline::reset(){
    _accel.reset();
    _free_spline();
    _last_errno = 0;
    return *this;
}
inline Spline & Spline::reset( const double *x, const double *y, size_t size,
    const interp_type_t *type ){
    reset();
    _init_spline( x, y, size, type );
    return *this;
}
inline Spline & Spline::reset( const vector<double> &x, const vector<double> &y,
    const interp_type_t *type ){
    return reset( x.data(), y.data(), x.size(), type );
}
inline string Spline::name() const{
    return string( gsl_spline_name( _spline ) );
}
inline Spline::size_t Spline::min_size( const interp_type_t *type ){
    return gsl_interp_type_min_size( type );
}
inline size_t Spline::min_size( ){
    return gsl_spline_min_size( _spline );
}
inline bool Spline::empty( ) const noexcept{
    return !_spline;
}
inline int Spline::last_errno() const noexcept{
    return _last_errno;
}
inline double Spline::operator()( double x ){
    double val;
    _last_errno = gsl_spline_eval_e( _spline, x, _accel.get(), &val );
    ErrGSL::check( _last_errno, emFLPFB, 
        "  ... arg ", x, " out of range" );
    return val;
}
inline double Spline::deriv( double x ){
    double val;
    _last_errno = gsl_spline_eval_deriv_e( _spline, x, _accel.get(), &val );
    ErrGSL::check( _last_errno, emFLPFB, 
        "  ... arg ", x, " out of range" );
    return val;
}
inline double Spline::deriv2( double x ){
    double val;
    _last_errno = gsl_spline_eval_deriv2_e( _spline, x, _accel.get(), &val );
    ErrGSL::check( _last_errno, emFLPFB, 
        "  ... arg ", x, " out of range" );
    return val;
}
inline double Spline::integ( double a, double b ){
    double val;
    _last_errno = gsl_spline_eval_integ_e( _spline, a, b, _accel.get(), &val );
    ErrGSL::check( _last_errno, emFLPFB, 
        "  ... arg range [", a, ", ", b, "] out of bound" );
    return val;
}

inline void Spline::_free_spline(){
    if( _spline ){
        gsl_spline_free( _spline );
        _spline = nullptr;
    }
}
inline void Spline::_init_spline( const double *x, const double *y, size_t size,
    const interp_type_t *type ){
    _spline = gsl_spline_alloc( type, size );
    _last_errno = gsl_spline_init( _spline, x, y, size );
}

} // namespace _interp_helper


} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERICAL_INTERP_H_