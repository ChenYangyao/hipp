set(_modid numerical)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "Module: ${_projectid}${_modid}")
message("   Source at: ${_moddir}")

add_subdirectory(src)
set(_libname "${_projectid}${_modid}")
add_library(${_libname}
    SHARED 
        "$<TARGET_OBJECTS:${_libname}gsl_util>"
        "$<TARGET_OBJECTS:${_libname}function>"
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_compile_options(${_libname} 
    PRIVATE
        ${_cxx_flags}
)
set(_link_libs "${_projectid}cntl")
if( gsl_lib_dir )
    list(APPEND _link_libs "-L${gsl_lib_dir}")
endif()
list(APPEND _link_libs "-lgsl" "-lgslcblas" )
target_link_libraries(${_libname}
    PUBLIC 
        ${_link_libs} 
)

install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_moddir}/header/gsl_util"
        "${_moddir}/header/function"
    DESTINATION include
)
install(FILES
        "${_moddir}/header/${_projectid}${_modid}.h"
    DESTINATION include
)