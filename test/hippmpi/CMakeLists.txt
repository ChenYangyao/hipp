set(_modid mpi)

set(_exebase "${_projectid}${_modid}")

function(addmpitest src nproc)
    set(testname "${_exebase}_${src}.out")

    add_executable("${testname}" "${src}.cpp")
    target_link_libraries("${testname}" 
        PRIVATE "${_exebase}")
    target_include_directories("${testname}" 
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

    add_test(NAME "${testname}" 
        COMMAND mpirun -np ${nproc} "./${testname}" ${ARGN})
    set_property(TEST "${testname}" 
        APPEND PROPERTY
        ENVIRONMENT 
            LD_LIBRARY_PATH=${CMAKE_INSTALL_PREFIX}/lib:$ENV{LD_LIBRARY_PATH})
endfunction(addmpitest)

addmpitest(env.test 4)
addmpitest(comm.test 4)
addmpitest(p2p.test 4)
addmpitest(collective.test 4)
addmpitest(pack 4)
addmpitest(datatype.test 4)
addmpitest(virtual_topology.test 6)

if(NOT HIPPIO_OFF)
    prtkeyproc("Test on hippmpi_mpprof enabled")
    addmpitest(mpprof 4)
endif()