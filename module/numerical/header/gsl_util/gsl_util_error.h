#ifndef _HIPPNUMERIC_GSL_UTIL_ERROR_H_
#define _HIPPNUMERIC_GSL_UTIL_ERROR_H_
#include "gsl_util_base.h"
namespace HIPP{
namespace NUMERICAL{

class ErrGSL: public ErrType<ErrAppGSL, ErrClassDefault>{
public:
    typedef ErrType<ErrAppGSL, ErrClassDefault> err_type_t;
    typedef gsl_error_handler_t err_handler_t;
    using err_type_t::ErrType;

    virtual const char *what() const noexcept override { 
        return gsl_strerror( _errno ); 
    }
    virtual string whats() const override{
        return err_type_t::whats() + " | Type: " + what(); }
    
    static void default_err_handler( 
        const char *reason, const char *file, int line, int gsl_errno ){
        if( _err_cntl_flag )
            prints( "gsl: ", file, ":", line, ": ERROR: ", reason, '\n',
            "Default HIPP GSL error hanlder invoked.\nErrGSL thrown" );
        throw ErrGSL( gsl_errno );   
    }
    const char *strerror(const int gsl_errno) const noexcept {
        return gsl_strerror(gsl_errno);
    }
    static err_handler_t * set_error_handler( 
        err_handler_t *new_hanlder = &default_err_handler ) noexcept {
        return gsl_set_error_handler( new_hanlder );
    }
    static err_handler_t * set_error_handler_off() noexcept{
        return gsl_set_error_handler_off();
    }

    template<typename ...Args>
    static void throw_( errno_t gsl_errno, Args &&... args ){
        if( _err_cntl_flag )
            prints( std::forward<Args>(args)... );
        throw ErrGSL( gsl_errno ); 
    }
    template<typename ...Args>
    static void abort( errno_t e, Args &&... args ){
        prints( std::forward<Args>(args)... );
        exit( e );
    }
    template<typename ...Args>
    static void check( errno_t gsl_errno, Args &&... args ) { 
        if( gsl_errno ) 
            throw_(gsl_errno, std::forward<Args>(args)...); 
    }

    template<typename ...Args>
    static void check_alloc( void *ptr, Args &&... args ){
        if( !ptr )
            throw_(GSL_ENOMEM, std::forward<Args>(args)...);
    }

    static int _has_init_gsl_env;
    static err_handler_t * _lib_default_err_handler;
};






} // namespace NUMERICAL
} // namespace HIPP
#endif	//_HIPPNUMERIC_GSL_UTIL_ERROR_H_