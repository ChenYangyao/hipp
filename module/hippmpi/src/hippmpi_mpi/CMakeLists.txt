set(_submodid mpi)
set(_src 
    mpi_comm.cpp 
    mpi_datapacket.cpp 
    mpi_datatype.cpp 
    mpi_env.cpp 
    mpi_file.cpp
    mpi_group.cpp
    mpi_info.cpp
    mpi_op.cpp 
    mpi_win.cpp
    mpi_raw_comm.cpp 
    mpi_raw_file.cpp
    mpi_requests.cpp
    mpi_pack.cpp
)
set(_libname "${_projectid}${_modid}_${_submodid}")
prtkeyproc("Sub module: ${_libname}")
message("   Sources: ${_src}")

add_library(${_libname} OBJECT "")
target_sources(${_libname} PRIVATE ${_src})
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_include_directories(${_libname}
    PRIVATE
        "${_headerdir}/${_libname}"
        "${_headerdir}"
)
target_link_libraries(${_libname}
    PRIVATE
        "${_projectid}cntl"
        mpi-interface
)