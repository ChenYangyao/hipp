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
    mpi_raw_comm.cpp 
    mpi_raw_file.cpp
    mpi_requests.cpp
)
set(_libname "${_projectid}${_modid}${_submodid}")

message(STATUS "Sub module: ${_libname}")
message("   Sources: ${_src}")

add_library(${_libname} 
    OBJECT "")
target_sources(${_libname}
    PRIVATE
        ${_src}
)
set_target_properties(${_libname}
    PROPERTIES
        POSITION_INDEPENDENT_CODE 1
)
target_include_directories(${_libname}
    PRIVATE
        "${_moddir}/header/${_submodid}"
    PUBLIC 
        ${MPI_CXX_INCLUDE_PATH}
        "${_moddir}/header"
        "${_projectdir}/module/cntl/header"
)
target_compile_options(${_libname} 
    PUBLIC 
        ${MPI_CXX_COMPILE_FLAGS}
    PRIVATE
        ${_cxx_flags}
)