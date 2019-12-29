#include "error/error_class.h"

namespace HIPP{
const char * ErrClass::_errmsgs[] = {
    "",
    "default", "unknown", "runtime error", "logic error", "memory error", 
    "cast error", "IO error", "invalid error no"
};    
} // namespace HIPP

