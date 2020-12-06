set(_modid io)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

set(_libname "${_projectid}${_modid}")
set(_headerdir "${_moddir}/header")
message(STATUS "Module: ${_libname}")
message("   Source at: ${_moddir}")

add_subdirectory(src)
add_library(${_libname}
    SHARED 
        "$<TARGET_OBJECTS:${_libname}hdf5>"
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_link_libraries(${_libname}
    PUBLIC 
        "-lhdf5"
        "${_projectid}cntl"
)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
)
install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_headerdir}/hdf5"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)
