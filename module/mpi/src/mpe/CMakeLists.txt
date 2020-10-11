set(_submodid mpe)
set(_src 
    mpe_sync_seqblock.cpp
    mpe_sync_lock.cpp
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