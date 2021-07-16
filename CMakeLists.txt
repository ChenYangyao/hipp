cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(hipp LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_projectid hipp)
set(_projectdir ${CMAKE_CURRENT_SOURCE_DIR})

include(cmake/colors.cmake)
include(cmake/printings.cmake)
define_colors()

# Options
set(CMAKE_CXX_FLAGS "-O3 -Wall" CACHE STRING "c++ flags")

option(enable-mpi "install mpi component?" OFF)

option(enable-hdf5 "install HDF5 IO component?" OFF)

option(enable-simd "install SIMD extension?" OFF)

option(enable-gsl "install GSL numerical component?" OFF)
set(GSL_ROOT_DIR "" CACHE STRING "gsl root directory")

option(enable-all-module "install all module components?" OFF)

if(enable-all-module)
    set(enable-mpi ON)
    set(enable-hdf5 ON)
    set(enable-simd ON)
    set(enable-gsl ON)
endif()

# Make GSL interface target, if enabled.
# gsl-interface target is defined.
if(enable-gsl)
    add_library(gsl-interface INTERFACE)
    prtkeyproc("GSL feature detection ...")
    if(GSL_ROOT_DIR)
        find_package(GSL REQUIRED)
        target_link_libraries(gsl-interface 
            INTERFACE GSL::gsl GSL::gslcblas)
        prtkeyproc("GSL found.")
    else()
        target_link_libraries(gsl-interface
            INTERFACE "-lgsl" "-lgslcblas")
        prtkeyproc("GSL root not set. Environmental settings will be used.")
    endif()
endif()

# Make HDF5 interface target, if enabled.
# hdf5-interface target is defined.
if(enable-hdf5)
    prtkeyproc("HDF5 feature detection ...")
    add_library(hdf5-interface INTERFACE)
    find_package(HDF5 REQUIRED COMPONENTS C)
    target_link_libraries(hdf5-interface
        INTERFACE ${HDF5_LIBRARIES})
    target_include_directories(hdf5-interface 
        INTERFACE ${HDF5_INCLUDE_DIRS})
endif()

# Make MPI interface target, if enabled.
# mpi-interface target is defined.
if(enable-mpi)
    prtkeyproc("MPI feature detection ...")
    add_library(mpi-interface INTERFACE)
    find_package(MPI REQUIRED)
    target_link_libraries(mpi-interface
        INTERFACE MPI::MPI_CXX)
endif(enable-mpi)

add_subdirectory(module)

include(CTest)
if(BUILD_TESTING)
    # Compile googletest as part of the project
    prtkeyproc("Configure googletest ...")
    set(BUILD_GMOCK OFF CACHE BOOL "")
    set(INSTALL_GTEST OFF CACHE BOOL "")
    add_subdirectory("third-party/googletest" "third-party/googletest" EXCLUDE_FROM_ALL)

    add_subdirectory(test)
endif()
