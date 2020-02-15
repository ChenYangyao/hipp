set(_modid simd)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "Module: ${_projectid}${_modid}")
message("   Pure header module")

install(DIRECTORY 
    "${_moddir}/header/simdopcode"
    "${_moddir}/header/simdpacked"
    "${_moddir}/header/simdvec"
    DESTINATION include
)
install(FILES
        "${_moddir}/header/${_projectid}${_modid}.h"
    DESTINATION include
)