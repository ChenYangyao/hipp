set(_bindir "${CMAKE_CURRENT_BINARY_DIR}") 
set(_srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
configure_file("${_srcdir}/hipp_config.h.in" "${_bindir}/hipp_config.h")

add_library(hipp-config INTERFACE)
target_include_directories(hipp-config INTERFACE "${_bindir}")

# Some old version of GCC for C++17 required link to the stdc++fs
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-lstdc++fs" STDCXXFS_LIBRARY_NEEDED)
    if(STDCXXFS_LIBRARY_NEEDED)
        target_link_libraries(hipp-config INTERFACE "stdc++fs")
        message(STATUS "stdc++fs detected and linked")
    else()
        message(STATUS "stdc++fs not required")
    endif()
endif()

install(FILES "${_bindir}/hipp_config.h" DESTINATION include)

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

