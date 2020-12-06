set(_modid mpi)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

set(_libname "${_projectid}${_modid}")
set(_headerdir "${_moddir}/header")
message(STATUS "Module: ${_libname}")
message("   Source at: ${_moddir}")

find_package(MPI REQUIRED)
add_subdirectory(src)
add_library(${_libname}
    SHARED 
        "$<TARGET_OBJECTS:${_libname}mpi>"
        "$<TARGET_OBJECTS:${_libname}mpe>"
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_link_libraries(${_libname}
    PUBLIC 
        "${_projectid}cntl"
        MPI::MPI_CXX
)
target_include_directories(${_libname} 
    INTERFACE 
        "${_headerdir}"
)
install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_headerdir}/mpi"
        "${_headerdir}/mpe"
    DESTINATION include
)
install(FILES
        "${_headerdir}/${_libname}.h"
    DESTINATION include
)