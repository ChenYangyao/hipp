set(_submodid h5)
set(_src
    "dataset"
    "datatype"
    "group"
    "xtable"
    "file"
    "dataset_manager"
    "attr_manager"
    "util_datapacket"
    "named_obj"
    "attr"
)

set(_exebase "${_projectid}${_modid}_${_submodid}")
prtkeyproc("Test on module: ${_exebase}")

function(addhippio_h5_test src)
    message("   ${src}.gtest.cpp")
    set(_exename "${_exebase}_${src}.gtest.out")
    
    add_executable( "${_exename}" "${src}.gtest.cpp")
    target_link_libraries( "${_exename}" 
        PRIVATE "${_modlib}" gmock_main
    )
    target_include_directories("${_exename}" 
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )    
    add_test(NAME "${_exename}" COMMAND "${_exename}")
    set_property(TEST "${_exename}" 
        APPEND PROPERTY
        ENVIRONMENT 
            LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:$ENV{LD_LIBRARY_PATH})
endfunction(addhippio_h5_test)


foreach(s IN LISTS _src)
    addhippio_h5_test("${s}")
endforeach()