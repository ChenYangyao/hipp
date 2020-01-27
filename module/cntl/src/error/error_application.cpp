#include "error_application.h"

namespace HIPP{
const char * ErrApp::_errmsgs[] = {
    "",
    "default", "unknown", "system", "MPI", "OpenMP", 
    "invalid error no"
};    
} // namespace HIPP

