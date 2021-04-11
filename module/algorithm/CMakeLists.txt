set(_modid algorithm)
set(_libname "${_projectid}${_modid}")

set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")
set(_headerdir "${_moddir}/header")

prtkeyproc("Module: ${_libname}")
message("  Source at: ${_moddir}")

# Pure header interface library
add_library(${_libname} INTERFACE)
target_link_libraries(${_libname}
    INTERFACE
        "${_projectid}cntl"
)
target_include_directories(${_libname}
    INTERFACE 
        "${_headerdir}"
)


install(DIRECTORY 
        "${_headerdir}/kdsearch"
    DESTINATION include
)
install(DIRECTORY 
        "${_headerdir}/mathops"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)