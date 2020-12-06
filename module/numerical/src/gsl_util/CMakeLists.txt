set(_submodid gsl_util)
set(_src 
    gsl_util_error.cpp
)
set(_libname "${_projectid}${_modid}${_submodid}")
set(_headerdir "${_moddir}/header")
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
set(_includes 
    "${_headerdir}/${_submodid}"
    "${_headerdir}" 
)
if(gsl_include_dir)
    list(APPEND _includes "${gsl_include_dir}")
endif()
target_include_directories(${_libname}
    PRIVATE
        ${_includes}
)
target_link_libraries(${_libname}
    PRIVATE
        "${_projectid}cntl"
)