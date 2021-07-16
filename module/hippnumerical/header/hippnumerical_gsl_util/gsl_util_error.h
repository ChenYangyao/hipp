#ifndef _HIPPNUMERIC_GSL_UTIL_ERROR_H_
#define _HIPPNUMERIC_GSL_UTIL_ERROR_H_
#include "gsl_util_base.h"
namespace HIPP{
namespace NUMERICAL{

/**
ErrGSL - the exception thrown from the GSL error.

When linking with the HIPP numeric library, GSL error handler is set to the 
HIPP default, i.e.,  a short message is printed and an exception typed ErrGSL 
is thrown. User can turn off it by calling the method set_err_handler_off(), 
or change the handler by set_err_handler(new_handler).
*/
class ErrGSL: public ErrType<ErrAppGSL, ErrClassDefault>{
public:
    enum : int { 
        SUCCESS_ = GSL_SUCCESS, EDOM_ = GSL_EDOM,     ERANGE_ = GSL_ERANGE, 
        ENOMEM_ = GSL_ENOMEM,   EINVAL_ = GSL_EINVAL
    };

    typedef ErrType<ErrAppGSL, ErrClassDefault> err_type_t;
    typedef gsl_error_handler_t err_handler_t;
    using err_type_t::ErrType;

    virtual const char * what() const noexcept override;
    virtual string whats() const override;
    
    /**
    The default error handler prints a message and throw ErrGSL(gsl_errno).
    */
    static void default_err_handler( 
        const char *reason, const char *file, int line, int gsl_errno);
    /**
    Set a specific error handler, or turn it off.
    */
    static err_handler_t * set_error_handler( 
        err_handler_t *new_handler = &default_err_handler ) noexcept;
    static err_handler_t * set_error_handler_off() noexcept;

    /** Query error string of predefined GSL error number. */
    const char * strerror(const int gsl_errno) const noexcept;

    /**
    throw_() - throw an error. If error_cntl_flag is ON, print args before 
        throwing.
    abort() - always print args and exit with provided error number.
    */
    template<typename ...Args>
    static void throw_( errno_t gsl_errno, Args &&... args);
    template<typename ...Args>
    static void abort( errno_t e, Args &&... args);

    /**
    Check for general errors and specific error. 
    check() - if not success, call throw_(gsl_errno, args...).
    check_alloc() - if ptr is NULL, call throw_(ENOMEM_, args...).
    */
    template<typename ...Args>
    static void check( errno_t gsl_errno, Args &&... args);
    template<typename ...Args>
    static void check_alloc( void *ptr, Args &&... args );
protected:
    static int _has_init_gsl_env;
    static err_handler_t * _lib_default_err_handler;
};

inline 
const char * ErrGSL::what() const noexcept { 
    return strerror(_errno);
}

/** Implementation */
inline 
string ErrGSL::whats() const {
    return err_type_t::whats() + " | Type: " + what(); }

inline 
void ErrGSL::default_err_handler( 
    const char *reason, const char *file, int line, int gsl_errno ){
    if( _err_cntl_flag )
        prints( "gsl: ", file, ":", line, ": ERROR: ", reason, '\n',
        "Default HIPP GSL error hadler invoked.\nErrGSL thrown" );
    throw ErrGSL( gsl_errno );   
}

inline 
auto ErrGSL::set_error_handler( err_handler_t *new_handler) noexcept 
-> err_handler_t * 
{
    return gsl_set_error_handler( new_handler );
}

inline 
auto ErrGSL::set_error_handler_off() noexcept -> err_handler_t * {
    return gsl_set_error_handler_off();
}

inline 
const char * ErrGSL::strerror(const int gsl_errno) const noexcept {
    return gsl_strerror(gsl_errno);
}

template<typename ...Args>
void ErrGSL::throw_( errno_t gsl_errno, Args &&... args ){
    if( _err_cntl_flag )
        prints( std::forward<Args>(args)... );
    throw ErrGSL( gsl_errno ); 
}

template<typename ...Args>
void ErrGSL::abort( errno_t e, Args &&... args ){
    prints( std::forward<Args>(args)... );
    exit( e );
}

template<typename ...Args>
void ErrGSL::check( errno_t gsl_errno, Args &&... args ) { 
    if( gsl_errno != SUCCESS_ ) 
        throw_(gsl_errno, std::forward<Args>(args)...); 
}

template<typename ...Args>
void ErrGSL::check_alloc( void *ptr, Args &&... args ){
    if( !ptr )
        throw_(ENOMEM_, std::forward<Args>(args)...);
}






} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERIC_GSL_UTIL_ERROR_H_