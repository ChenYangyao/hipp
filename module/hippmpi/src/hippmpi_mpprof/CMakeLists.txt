set(_submodid mpprof)
set(_src 
    mpprof_default_logger.cpp
    mpprof_logger.cpp
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
        "${_projectid}io"
        "${_projectid}algorithm"
        mpi-interface
)