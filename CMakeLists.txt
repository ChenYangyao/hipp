cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(hipp LANGUAGES C CXX VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform Detection
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    message(STATUS "Apple Darwin platform")
    set(MACOSX_RPATH ON)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    message(STATUS "Linux platform")
else()
    message(STATUS "Unsupported platform")
endif()

#[====[ 
RPATH support
#]====]
set(HIPP_ADD_RPATH "" CACHE STRING "Additional RPATH for build and install")
set(HIPP_ADD_RPATH_ENV "" CACHE STRING "Additional RPATH for build and install interpreted as colon-separated list")

set(CMAKE_SKIP_BUILD_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
	
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif()

if(HIPP_ADD_RPATH)
    message(STATUS "Add RPATH " "${HIPP_ADD_RPATH}")
    list(APPEND CMAKE_INSTALL_RPATH "${HIPP_ADD_RPATH}")
    list(APPEND CMAKE_BUILD_RPATH "${HIPP_ADD_RPATH}")
endif()
if(HIPP_ADD_RPATH_ENV)
    string(REPLACE ":" ";" HIPP_ADD_RPATH_ENV "${HIPP_ADD_RPATH_ENV}")
    message(STATUS "Add RPATH " "${HIPP_ADD_RPATH_ENV}")
    list(APPEND CMAKE_INSTALL_RPATH "${HIPP_ADD_RPATH_ENV}")
    list(APPEND CMAKE_BUILD_RPATH "${HIPP_ADD_RPATH_ENV}")
endif()

message(STATUS "Finally adopted PRATH --")
message("   Build: ${CMAKE_BUILD_RPATH}")
message("   Install: ${CMAKE_INSTALL_RPATH}")

set(_projectid hipp)
set(_projectdir ${CMAKE_CURRENT_SOURCE_DIR})


include(cmake/colors.cmake)
define_colors()

include(cmake/printings.cmake)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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
    find_package(GSLMod REQUIRED)

    add_library(gsl-interface INTERFACE)
    target_link_libraries(gsl-interface INTERFACE GSLMod::GSL)
    
    set(HIPPNUMERICAL_OFF OFF)
endif()

# Make HDF5 interface target, if enabled.
# hdf5-interface target is defined.
if(enable-hdf5)
    find_package(HDF5Mod REQUIRED)

    add_library(hdf5-interface INTERFACE)
    target_link_libraries(hdf5-interface
        INTERFACE HDF5Mod::HDF5)
    
    set(HIPPIO_OFF OFF)
endif()

# Make MPI interface target, if enabled.
# mpi-interface target is defined.
if(enable-mpi)
    find_package(MPIMod REQUIRED)

    add_library(mpi-interface INTERFACE)
    target_link_libraries(mpi-interface
        INTERFACE MPIMod::MPI)

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
