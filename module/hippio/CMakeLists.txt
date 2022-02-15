set(_modid io)
set(_libname "${_projectid}${_modid}")

set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")
set(_headerdir "${_moddir}/header")
prtkeyproc("Module: ${_libname}")
message("   Source at: ${_moddir}")

add_subdirectory(src)
add_library(${_libname}
    SHARED 
        "$<TARGET_OBJECTS:${_libname}_hdf5>"
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_link_libraries(${_libname}
    PUBLIC 
        hipp-config
        "${_projectid}cntl"
        hdf5-interface
)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
)

install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_headerdir}/${_libname}_hdf5"
        "${_headerdir}/${_libname}_hdf5_raw"
        "${_headerdir}/${_libname}_hdf5_impl"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)
