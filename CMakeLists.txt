cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(hipp LANGUAGES C CXX VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform Detection
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    message(STATUS "Apple Darwin platform")
    set(MACOSX_RPATH ON)
    set(env_dyld_lib_path "$ENV{DYLD_LIBRARY_PATH}")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    message(STATUS "Linux platform")
    set(env_dyld_lib_path "$ENV{LD_LIBRARY_PATH}")
else()
    message(STATUS "Unsupported platform")
    endif()
set(env_rpath "$ENV{LD_RUN_PATH}")


# RPATH support
set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
	
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

set(HIPP_ADD_RPATH_FROM_ENV "Add environment variables to RPATH?" ON)
if(HIPP_ADD_RPATH_FROM_ENV)
    message(STATUS "Add RPATH from environment")
    if(env_rpath)
        message("    ${env_rpath}")
        list(APPEND CMAKE_INSTALL_RPATH "${env_rpath}")
        list(APPEND CMAKE_BUILD_RPATH "${env_rpath}")
    endif()
    if(env_dyld_lib_path)
        message("    ${env_dyld_lib_path}")
        list(APPEND CMAKE_INSTALL_RPATH "${env_dyld_lib_path}")
        list(APPEND CMAKE_BUILD_RPATH "${env_dyld_lib_path}")
    endif()
endif()

set(_projectid hipp)
set(_projectdir ${CMAKE_CURRENT_SOURCE_DIR})

include(cmake/colors.cmake)
include(cmake/printings.cmake)
define_colors()

# Options
if(NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "-O3 -Wall -pthread" CACHE STRING "cxx flags" FORCE)
endif()

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

set(HIPPCNTL_OFF OFF)
set(HIPPALGORITHM_OFF OFF)
set(HIPPNUMERICAL_OFF ON)
set(HIPPIO_OFF ON)
set(HIPPMPI_OFF ON)
set(HIPPSIMD_OFF ON)

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

    set(HIPPNUMERICAL_OFF OFF)

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

    set(HIPPIO_OFF OFF)
endif()

# Make MPI interface target, if enabled.
# mpi-interface target is defined.
if(enable-mpi)
    prtkeyproc("MPI feature detection ...")
    add_library(mpi-interface INTERFACE)
    find_package(MPI REQUIRED)
    target_link_libraries(mpi-interface
        INTERFACE MPI::MPI_CXX)

    set(HIPPMPI_OFF OFF)
endif(enable-mpi)

if(enable-simd)
    set(HIPPSIMD_OFF OFF)
endif()


add_subdirectory(module)

include(CTest)
if(BUILD_TESTING)
    # Compile googletest as part of the project
    prtkeyproc("Configure googletest ...")

    set(BUILD_GMOCK ON CACHE BOOL "")
    set(INSTALL_GTEST OFF CACHE BOOL "")
    
    add_subdirectory("third-party/googletest" "third-party/googletest" EXCLUDE_FROM_ALL)

    add_subdirectory(test)
endif()
