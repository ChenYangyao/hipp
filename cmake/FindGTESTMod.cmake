#[==[

FindGTESTMod
------------

Dependencies
^^^^^^^^^^^^
`GTEST_ROOT`: the variable containing the path of the library root directory,
which must have
    include/gtest/gtest.h
    lib/<gtest library>
    lib/<gtest_main library>
Instead, if component `GMOCK` enabled, that must have
    include/gmock/gmock.h
    lib/<gmock library>
    lib/<gmock_main library>

Resulted variables
^^^^^^^^^^^^^^^^^^
GTESTMod_FOUND

Cache entry
^^^^^^^^^^^
GTESTMod_INCLUDE_DIR
GTESTMod_{GTEST|GTEST_MAIN|GMOCK|GMOCK_MAIN}_LIBRARY

Imported library
^^^^^^^^^^^^^^^^
GTESTMod::{GTEST|GTEST_MAIN|GMOCK|GMOCK_MAIN}

#]==]


find_path(GTESTMod_INCLUDE_DIR "gtest/gtest.h")

find_library(GTESTMod_GTEST_LIBRARY "gtest")
find_library(GTESTMod_GTEST_MAIN_LIBRARY "gtest_main")

set(gtest_lib_ids GTESTMod_GTEST_LIBRARY GTESTMod_GTEST_MAIN_LIBRARY)

if(GTESTMod_FIND_REQUIRED_GMOCK)
    find_library(GTESTMod_GMOCK_LIBRARY "gmock")
    find_library(GTESTMod_GMOCK_MAIN_LIBRARY "gmock_main")
    list(APPEND gtest_lib_ids GTESTMod_GMOCK_LIBRARY GTESTMod_GMOCK_MAIN_LIBRARY)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GTEST DEFAULT_MSG 
    GTESTMod_INCLUDE_DIR ${gtest_lib_ids})

if(GTESTMod_FOUND)
    message(STATUS "Imported library GTESTMod::GTEST")
    message("   Library: ${GTESTMod_GTEST_LIBRARY}")
    message("   Include dir: ${GTESTMod_INCLUDE_DIR}")

    if(NOT TARGET GTESTMod::GTEST)
        add_library(GTESTMod::GTEST UNKNOWN IMPORTED)
        set_target_properties(GTESTMod::GTEST PROPERTIES
            IMPORTED_LOCATION "${GTESTMod_GTEST_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${GTESTMod_INCLUDE_DIR}")
        target_compile_options(GTESTMod::GTEST INTERFACE "-pthread")
        target_link_options(GTESTMod::GTEST INTERFACE "-pthread")


    message(STATUS "Imported library GTESTMod::GTEST_MAIN")
    message("   Library: ${GTESTMod_GTEST_MAIN_LIBRARY}")

    if(NOT TARGET GTESTMod::GTEST_MAIN)    
        add_library(GTESTMod::GTEST_MAIN UNKNOWN IMPORTED)
        set_target_properties(GTESTMod::GTEST_MAIN PROPERTIES
            IMPORTED_LOCATION "${GTESTMod_GTEST_MAIN_LIBRARY}")
        target_link_libraries(GTESTMod::GTEST_MAIN INTERFACE GTESTMod::GTEST)
    endif()

    if(GTESTMod_FIND_REQUIRED_GMOCK AND NOT TARGET GTESTMod::GMOCK)
        message(STATUS "Imported library GTESTMod::GMOCK")
        message("   Library: ${GTESTMod_GMOCK_LIBRARY}")
        message("   Include dir: ${GTESTMod_INCLUDE_DIR}")
        
        add_library(GTESTMod::GMOCK UNKNOWN IMPORTED)
        set_target_properties(GTESTMod::GMOCK PROPERTIES
            IMPORTED_LOCATION "${GTESTMod_GMOCK_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${GTESTMod_INCLUDE_DIR}")
        target_compile_options(GTESTMod::GMOCK INTERFACE "-pthread")
        target_link_options(GTESTMod::GMOCK INTERFACE "-pthread")
    endif()

    if(GTESTMod_FIND_REQUIRED_GMOCK AND NOT TARGET GTESTMod::GMOCK_MAIN)
        message(STATUS "Imported library GTESTMod::GMOCK_MAIN")
        message("   Library: ${GTESTMod_GMOCK_MAIN_LIBRARY}")
        
        add_library(GTESTMod::GMOCK_MAIN UNKNOWN IMPORTED)
        set_target_properties(GTESTMod::GMOCK_MAIN PROPERTIES
            IMPORTED_LOCATION "${GTESTMod_GMOCK_MAIN_LIBRARY}")
        target_link_libraries(GTESTMod::GMOCK_MAIN INTERFACE GTESTMod::GMOCK)
    endif()
endif()