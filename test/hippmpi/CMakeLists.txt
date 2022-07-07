set(_modid mpi)

set(_exebase "${_projectid}${_modid}")

function(addmpitest src nproc)
    message("   ${src}.test.cpp")
    set(_exename "${_exebase}_${src}.test.out")

    add_executable("${_exename}" "${src}.test.cpp")
    target_link_libraries("${_exename}" 
        PRIVATE "${_exebase}")
    target_include_directories("${_exename}" 
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

    add_test(NAME "${_exename}" 
        COMMAND mpirun -np ${nproc} "./${_exename}" ${ARGN})
    #set_property(TEST "${_exename}" 
    #    APPEND PROPERTY
    #    ENVIRONMENT 
    #        LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:$ENV{LD_LIBRARY_PATH})
endfunction(addmpitest)

addmpitest(env 4)
addmpitest(comm 4)
addmpitest(p2p 4)
addmpitest(collective 4)
addmpitest(pack 4)
addmpitest(datatype 4)
addmpitest(virtual_topology 6)

if(NOT HIPPIO_OFF)
    prtkeyproc("Test on hippmpi_mpprof enabled")
    addmpitest(mpprof 4)
endif()