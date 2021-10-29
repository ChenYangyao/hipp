set(_modid cntl)
set(_src
    "stream"
    "generic_concept"
    "time_ticker"
)

set(_exebase "${_projectid}${_modid}")
prtkeyproc("Test on module: ${_exebase}")

function(addhippcntl_test src)
    message("   ${src}.gtest.cpp")
    set(_exename "${_exebase}_${src}.gtest.out")
    
    add_executable( "${_exename}" "${src}.gtest.cpp")
    target_link_libraries( "${_exename}" 
        PRIVATE "${_exebase}" gmock_main
    )
    add_test(NAME "${_exename}" COMMAND "${_exename}")
    set_property(TEST "${_exename}" 
        APPEND PROPERTY
        ENVIRONMENT 
            LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:$ENV{LD_LIBRARY_PATH})
endfunction()

foreach(s IN LISTS _src)
    addhippcntl_test("${s}")
endforeach()

