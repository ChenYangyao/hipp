set(_modid mpi)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "Module: ${_projectid}${_modid}")
message("   Source at: ${_moddir}")

find_package(MPI REQUIRED)
add_subdirectory(src)
set(_libname "${_projectid}${_modid}")
add_library(${_libname}
    SHARED 
        "$<TARGET_OBJECTS:${_libname}mpi>"
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
        MPI::MPI_CXX
        "${_projectid}cntl"
)

install(TARGETS ${_libname}
    DESTINATION lib
)
install(DIRECTORY 
        "${_moddir}/header/mpi"
    DESTINATION include
)
install(FILES
        "${_moddir}/header/${_projectid}${_modid}.h"
    DESTINATION include
)