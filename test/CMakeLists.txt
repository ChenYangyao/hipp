if(NOT CMAKE_CXX_FLAGS MATCHES "-pthread")
    set(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")
endif()

add_subdirectory(cntl)
add_subdirectory(algorithm)

if(enable-hdf5)
    message(STATUS "Test on ${BoldGreen}IO enabled${ColourReset}")
    add_subdirectory(io)
endif()

if(enable-gsl)
    message(STATUS "Test on ${BoldGreen}NUMERICAL enabled${ColourReset}")
    add_subdirectory(numerical)
endif()