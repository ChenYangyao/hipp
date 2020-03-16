set(_submodid "function")
set(_src 
    integration.cpp
    interp.cpp
)
set(_libname "${_projectid}${_modid}${_submodid}")

message(STATUS "Sub module: ${_libname}")
message("   Sources: ${_src}")

add_library(${_libname} 
    OBJECT "")
target_sources(${_libname}
    PRIVATE
        ${_src}
)

set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)

set(_includes "${_moddir}/header" "${_projectdir}/module/cntl/header")
if(gsl_include_dir)
    list(APPEND _includes ${gsl_include_dir})
endif()
target_include_directories(${_libname}
    PRIVATE
        "${_moddir}/header/${_submodid}"
    PUBLIC  
        ${_includes}
)

target_compile_options(${_libname} 
    PRIVATE
        ${_cxx_flags}
)