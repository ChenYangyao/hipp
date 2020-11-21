set(_submodid hdf5)
set(_src 
    h5_error.cpp
    h5_type_raw.cpp    
    h5_obj_dataspace.cpp
    h5_obj_group.cpp
    h5_obj_file.cpp
    h5_obj_dataset.cpp
    h5_obj_proplist.cpp
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
        "${_moddir}/header"
        "${_projectdir}/module/cntl/header"
)
target_compile_options(${_libname} 
    PRIVATE
        ${_cxx_flags}
)