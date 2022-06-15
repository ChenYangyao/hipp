#[==[

FindGSLMod
-----------

Dependencies
^^^^^^^^^^^^
`GSL_ROOT`: the variable containing the path of the library root directory,
which must have
    bin/<gsl CLIs>
    include/gsl/<gsl headers>
    lib/<gsl libraries> (e.g., libgsl.so, libgslcblas.so)
For compatibility with built-in find module, `GSL_ROOT_DIR` (either CMake 
variable or environment variable) is also acceptable.

Resulted variables
^^^^^^^^^^^^^^^^^^
GSLMod_FOUND

Cache entry
^^^^^^^^^^^
GSLMod_INCLUDE_DIR
GSLMod_LIBRARY

Imported library
^^^^^^^^^^^^^^^^
GSLMod::GSL

#]==]

set(gsl_mod_find_flags)
if(GSLMod_FIND_REQUIRED)
    list(APPEND gsl_mod_find_flags "REQUIRED")
endif()
if(GSLMod_FIND_QUIETLY)
    list(APPEND gsl_mod_find_flags "QUIET")
endif()

if(NOT DEFINED GSL_ROOT_DIR AND NOT DEFINED ENV{GSL_ROOT_DIR})
    if(DEFINED GSL_ROOT)
        set(GSL_ROOT_DIR "${GSL_ROOT}")
    elseif(DEFINED ENV{GSL_ROOT})
        set(GSL_ROOT_DIR "$ENV{GSL_ROOT}")
    endif()
endif()

find_package(GSL ${gsl_mod_find_flags})

set(GSLMod_FOUND "Whether or not GSL Module is found?" ${GSL_FOUND})

if(GSLMod_FOUND AND NOT TARGET GSLMod::GSL)
    message(STATUS "Imported library GSLMod::GSL")
    message("   Include dirs: ${GSL_INCLUDE_DIRS}")
    message("   Libraries: ${GSL_LIBRARIES}")

    set(GSLMod_INCLUDE_DIR "${GSL_INCLUDE_DIRS}" CACHE STRING "GSL include directories")
    set(GSLMod_LIBRARY "${GSL_LIBRARIES}" CACHE STRING "GSL libraries")

    add_library(GSLMod::GSL INTERFACE IMPORTED)
    target_link_libraries(GSLMod::GSL
        INTERFACE GSL::gsl GSL::gslcblas
    )
endif()