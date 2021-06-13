add_subdirectory("${_projectid}cntl")

add_subdirectory("${_projectid}algorithm")

if(enable-mpi)
    message(STATUS "${BoldGreen}MPI enabled${ColourReset}")
    add_subdirectory("${_projectid}mpi")
else()
    message(STATUS "${BoldRed}MPI disabled${ColourReset}. Module ${_projectid}mpi will not be built")
endif()


if(enable-hdf5)
    message(STATUS "${BoldGreen}HDF5 enabled${ColourReset}")
    add_subdirectory("${_projectid}io")
else()
    message(STATUS "${BoldRed}HDF5 disabled${ColourReset}. Module ${_projectid}io will not be built")
endif()


if(enable-simd)
    message(STATUS "${BoldGreen}SIMD enabled${ColourReset}")
    add_subdirectory("${_projectid}simd")
else()
    message(STATUS "${BoldRed}SIMD disabled${ColourReset}. Module ${_projectid}simd will not be built")
endif()


if(enable-gsl)
    message(STATUS "${BoldGreen}GSL enabled${ColourReset}")
    if(gsl_include_dir)
        message(STATUS "GSL include dir is specified: ${gsl_include_dir}")
    endif()
    if(gsl_lib_dir)
        message(STATUS "GSL library dir is specified: ${gsl_lib_dir}")
    endif()
    add_subdirectory("${_projectid}numerical")
else()
    message(STATUS "${BoldRed}GSL disabled${ColourReset}. Module ${_projectid}numerical will not be built")
endif()

