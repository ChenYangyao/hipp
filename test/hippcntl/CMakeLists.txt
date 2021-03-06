set(_modid cntl)
set(_src
    stream.gtest.cpp
    generic_concept.gtest.cpp
)

set(_exebase "${_projectid}${_modid}")
set(_exename "${_exebase}.out")
set(_objname "${_exebase}.obj")
prtkeyproc("Test on module: ${_exebase}")
message("   Sources: ${_src}")

add_executable( "${_exename}"
    "$<TARGET_OBJECTS:${_objname}>"
)
target_link_libraries( "${_exename}" 
    PRIVATE
        "${_exebase}"
        gtest_main
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
    PRIVATE
        "${_exebase}"
        gtest_main
)
