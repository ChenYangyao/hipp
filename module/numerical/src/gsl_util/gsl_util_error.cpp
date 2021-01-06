#include "gsl_util_error.h"
namespace HIPP{
namespace NUMERICAL{

ErrGSL::err_handler_t * ErrGSL::_lib_default_err_handler 
    = ErrGSL::set_error_handler();
int ErrGSL::_has_init_gsl_env = 1;

} // namespace NUMERICAL
} // namespace HIPP