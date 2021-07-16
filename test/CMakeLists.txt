#if(NOT CMAKE_CXX_FLAGS MATCHES "-pthread")
#    set(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
#endif()

prtkeyproc("Test on ${_projectid}cntl required")
add_subdirectory("${_projectid}cntl")

prtkeyproc("Test on ${_projectid}algorithm required")
add_subdirectory("${_projectid}algorithm")

if(enable-hdf5)
    prtkeyproc("Test on ${_projectid}io enabled")
    add_subdirectory("${_projectid}io")
endif()

if(enable-gsl)
    prtkeyproc("Test on ${_projectid}numerical enabled")
    add_subdirectory("${_projectid}numerical")
endif()

if(enable-mpi)
    prtkeyproc("Test on ${_projectid}mpi enabled")
    add_subdirectory("${_projectid}mpi")
endif()
