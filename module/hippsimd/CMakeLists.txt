set(_modid simd)
set(_libname "${_projectid}${_modid}")

set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")
set(_headerdir "${_moddir}/header")
prtkeyproc("Module: ${_libname}")
message("   Source at: ${_moddir}")

add_library(${_libname} INTERFACE)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
)
install(DIRECTORY 
    "${_headerdir}/${_libname}_simdopcode"
    "${_headerdir}/${_libname}_simdpacked"
    "${_headerdir}/${_libname}_simdvec"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)