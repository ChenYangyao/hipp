set(_modid cntl)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "Module: ${_projectid}${_modid}")
message("   Source at: ${_moddir}")

add_subdirectory(src)
set(_libname "${_projectid}${_modid}")
add_library(${_libname} 
    SHARED 
        "$<TARGET_OBJECTS:${_libname}error>"
        "$<TARGET_OBJECTS:${_libname}stream>"
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_compile_options(${_libname} 
    PRIVATE
        ${_cxx_flags}
)
install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_moddir}/header/incl"
        "${_moddir}/header/error"
        "${_moddir}/header/memory"
        "${_moddir}/header/stream"
    DESTINATION include
)
install(FILES
        "${_moddir}/header/${_projectid}${_modid}.h"
    DESTINATION include
)