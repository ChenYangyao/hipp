add_subdirectory("${_libname}_mpi")
add_subdirectory("${_libname}_mpe")

if(NOT HIPPIO_OFF)
    add_subdirectory("${_libname}_mpprof")
endif()