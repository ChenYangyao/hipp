cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(hipp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_projectid hipp)
set(_projectdir ${CMAKE_CURRENT_SOURCE_DIR})

set(gsl_include_dir "" CACHE STRING "GSL header dir")
set(gsl_lib_dir "" CACHE STRING "GSL library dir")

if(NOT CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "-O3 -Wall")
endif()

include(cmake/colors.cmake)
include(cmake/gtest.cmake)
define_colors()

add_subdirectory(module)

option(BUILD_TESTING "Build testing" OFF)
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
