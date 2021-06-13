#include "error_application.h"

namespace HIPP{
const char * ErrApp::_errmsgs[] = {
    "",
    "default", "unknown", "system", "MPI", "OpenMP", "HDF5", "GSL", "Python",
    "invalid error no"
};    
} // namespace HIPP

