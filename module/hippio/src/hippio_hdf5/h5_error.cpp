#include <h5_error.h>
namespace HIPP{
namespace IO{
    
thread_local ErrH5::flag_t ErrH5::_err_cntl_flag = 1;

} // namespace IO
} // namespace HIPP
