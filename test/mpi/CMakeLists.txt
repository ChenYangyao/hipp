set(_modid mpi)

set(_exebase "${_projectid}${_modid}")

function(addmpitest src nproc)
    set(testname "mpitest-${src}")

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

addmpitest(p2p 4)
addmpitest(pack 4)