set(_submodid stream)
set(_src 
    stream_pretty.cpp
    stream_pretty_log.cpp)

set(_libname "${_projectid}${_modid}${_submodid}")
set(_headerdir "${_moddir}/header")
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
        "${_headerdir}/${_submodid}"
        "${_headerdir}"
)