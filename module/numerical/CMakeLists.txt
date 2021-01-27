set(_modid numerical)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

set(_libname "${_projectid}${_modid}")
set(_headerdir "${_moddir}/header")
message(STATUS "Module: ${_projectid}${_modid}")
message("   Source at: ${_moddir}")

add_subdirectory(src)
add_library(${_libname}
    SHARED 
        "$<TARGET_OBJECTS:${_libname}gsl_util>"
        "$<TARGET_OBJECTS:${_libname}function>"
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
set(_link_libs "-lgsl" "-lgslcblas")
if(gsl_lib_dir)
    list(APPEND _link_libs "-L${gsl_lib_dir}")
endif()
target_link_libraries(${_libname}
    PUBLIC 
        "${_projectid}cntl"
        ${_link_libs} 
)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
)
install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_headerdir}/gsl_util"
        "${_headerdir}/function"
        "${_headerdir}/random_number"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)