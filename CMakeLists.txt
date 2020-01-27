cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(hipp LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(_cxx_flags "-O3" "-Wall" CACHE STRING "c++ flags")

set(_projectid hipp)
set(_projectdir ${CMAKE_CURRENT_SOURCE_DIR})
add_subdirectory(module)