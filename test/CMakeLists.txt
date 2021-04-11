#if(NOT CMAKE_CXX_FLAGS MATCHES "-pthread")
#    set(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
#endif()

prtkeyproc("Test on cntl required")
add_subdirectory(cntl)

prtkeyproc("Test on algorithm required")
add_subdirectory(algorithm)

if(enable-hdf5)
    prtkeyproc("Test on IO enabled")
    add_subdirectory(io)
endif()

if(enable-gsl)
    prtkeyproc("Test on NUMERICAL enabled")
    add_subdirectory(numerical)
endif()

if(enable-mpi)
    prtkeyproc("Test on MPI enabled")
    add_subdirectory(mpi)
endif()
