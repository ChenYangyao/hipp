add_subdirectory("${_libname}_mpi")
add_subdirectory("${_libname}_mpe")

if(enable-hdf5)
    add_subdirectory("${_libname}_mpprof")
endif()