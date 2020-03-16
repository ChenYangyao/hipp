#ifndef _HIPPNUMERICAL_INTERP_IMPL_H_
#define _HIPPNUMERICAL_INTERP_IMPL_H_
#include "interp.h"
namespace HIPP{
namespace NUMERICAL{
namespace _interp_helper{

inline Accel::Accel(): _accel(){
    _HIPPNUMERICAL_GSL_UTIL_MAKE_INIT
    _accel = gsl_interp_accel_alloc();
}
inline Accel::~Accel(){
    gsl_interp_accel_free( _accel );
}
inline Accel & Accel::reset(){
    gsl_interp_accel_reset( _accel );
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
#endif	//_HIPPNUMERICAL_INTERP_IMPL_H_
