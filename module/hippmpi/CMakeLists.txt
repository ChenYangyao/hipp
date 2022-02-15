set(_modid mpi)
set(_libname "${_projectid}${_modid}")

set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")
set(_headerdir "${_moddir}/header")
prtkeyproc("Module: ${_libname}")
message("   Source at: ${_moddir}")

add_subdirectory(src)
add_library(${_libname}
    SHARED 
        "$<TARGET_OBJECTS:${_libname}_mpi>"
        "$<TARGET_OBJECTS:${_libname}_mpe>"
)

set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_link_libraries(${_libname}
    PUBLIC 
        hipp-config
        "${_projectid}cntl"
        mpi-interface
)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
)

if(NOT HIPPIO_OFF)
    target_sources(${_libname}
        PRIVATE
            "$<TARGET_OBJECTS:${_libname}_mpprof>"
    )
    target_link_libraries(${_libname}
        PUBLIC
            "${_projectid}io"
            "${_projectid}algorithm"
    )
    install(DIRECTORY 
            "${_headerdir}/${_libname}_mpprof"
        DESTINATION include
    )
endif()

install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_headerdir}/hippmpi_mpi_raw"
        "${_headerdir}/hippmpi_mpi"
        "${_headerdir}/hippmpi_mpe"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)