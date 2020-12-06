set(_modid simd)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

set(_libname "${_projectid}${_modid}")
set(_headerdir "${_moddir}/header")
message(STATUS "Module: ${_libname}")
message("   Source at: ${_moddir}")

add_library(${_libname} INTERFACE)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
)
install(DIRECTORY 
    "${_headerdir}/simdopcode"
    "${_headerdir}/simdpacked"
    "${_headerdir}/simdvec"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)