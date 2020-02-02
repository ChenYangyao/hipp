set(_modid io)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "Module: ${_projectid}${_modid}")
message("   Source at: ${_moddir}")

add_subdirectory(src)


set(_libname "${_projectid}${_modid}")
add_library(${_libname}
    SHARED 
        "$<TARGET_OBJECTS:${_libname}hdf5>"
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_compile_options(${_libname} 
    PRIVATE
        ${_cxx_flags}
)
target_link_libraries(${_libname}
    PUBLIC 
        "-lhdf5"
        "${_projectid}cntl"
)

install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_moddir}/header/hdf5"
    DESTINATION include
)
install(FILES
        "${_moddir}/header/${_projectid}${_modid}.h"
    DESTINATION include
)
