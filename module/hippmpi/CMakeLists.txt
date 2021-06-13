set(_modid mpi)
set(_libname "${_projectid}${_modid}")

set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")
set(_headerdir "${_moddir}/header")
set(_binmoddir "${CMAKE_CURRENT_BINARY_DIR}")
set(_binheaderdir "${_binmoddir}/header")
prtkeyproc("Module: ${_libname}")
message("   Source at: ${_moddir}")

if(NOT enable-hdf5)
    set(_HIPPMPI_CONFIG_NO_MPPROF ON)
endif()

configure_file("${_headerdir}/${_libname}_config.h.in" "${_binheaderdir}/${_libname}_config.h" )

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
        "${_projectid}cntl"
        mpi-interface
)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
        "${_binheaderdir}"
)

if(enable-hdf5)
    target_sources(${_libname}
        PRIVATE
            "$<TARGET_OBJECTS:${_libname}_mpprof>"
    )
    target_link_libraries(${_libname}
        PUBLIC
            "${_projectid}io"
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
        "${_headerdir}/hippmpi_mpi"
        "${_headerdir}/hippmpi_mpe"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
        "${_binheaderdir}/${_libname}_config.h"
    DESTINATION include
)