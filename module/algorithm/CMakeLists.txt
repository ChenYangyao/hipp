set(_modid algorithm)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

set(_libname "${_projectid}${_modid}")
set(_headerdir "${_moddir}/header")
message(STATUS "Module: ${_projectid}${_modid}")
message("   Source at: ${_moddir}")


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
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)