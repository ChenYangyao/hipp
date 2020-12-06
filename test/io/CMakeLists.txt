set(_modid io)
set(_src
    h5dataset.gtest.cpp
    h5datatype.gtest.cpp
    h5group.gtest.cpp
)

set(_exebase "${_projectid}${_modid}")
set(_exename "${_exebase}.out")
set(_objname "${_exebase}.obj")
message(STATUS "Test on module: ${_projectid}${_modid}")
message("   Sources: ${_src}")

add_executable( "${_exename}"
    "$<TARGET_OBJECTS:${_exebase}.obj>"
)
target_link_libraries( "${_exename}" 
    PRIVATE
        "${_exebase}"
        "${_projectid}cntl"
        gtest-interface-lib
)

add_test(NAME "${_exename}" COMMAND "${_exename}")
set_property(TEST "${_exename}" 
    APPEND PROPERTY
    ENVIRONMENT 
        LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:$ENV{LD_LIBRARY_PATH})
    
add_library("${_objname}" OBJECT "")
target_sources("${_objname}"
    PRIVATE
        ${_src}
)
target_link_libraries( "${_objname}" 
    PUBLIC
        "${_exebase}"
        "${_projectid}cntl"
)