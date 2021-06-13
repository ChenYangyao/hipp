set(_modid numerical)
set(_libname "${_projectid}${_modid}")

set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")
set(_headerdir "${_moddir}/header")
prtkeyproc("Module: ${_projectid}${_modid}")
message("   Source at: ${_moddir}")

add_subdirectory(src)
add_library(${_libname}
    SHARED 
        "$<TARGET_OBJECTS:${_libname}_gsl_util>"
        "$<TARGET_OBJECTS:${_libname}_function>"
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_link_libraries(${_libname}
    PUBLIC 
        "${_projectid}cntl"
        gsl-interface
)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
)
install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_headerdir}/${_libname}_gsl_util"
        "${_headerdir}/${_libname}_function"
        "${_headerdir}/${_libname}_random_number"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)