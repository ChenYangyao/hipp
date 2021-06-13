set(_modid cntl)
set(_libname "${_projectid}${_modid}")

set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")
set(_headerdir "${_moddir}/header")
prtkeyproc("Module: ${_libname}")
message("   Source at: ${_moddir}")

add_subdirectory(src)
add_library(${_libname} 
    SHARED 
        "$<TARGET_OBJECTS:${_libname}_error>"
        "$<TARGET_OBJECTS:${_libname}_stream>"
        "$<TARGET_OBJECTS:${_libname}_time>"
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
)
install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_headerdir}/${_libname}_incl"
        "${_headerdir}/${_libname}_error"
        "${_headerdir}/${_libname}_memory"
        "${_headerdir}/${_libname}_stream"
        "${_headerdir}/${_libname}_time"
    DESTINATION include
)
install(FILES "${_headerdir}/${_libname}.h"
    DESTINATION include
)