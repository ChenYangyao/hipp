set(_modid algorithm)
set(_moddir "${CMAKE_CURRENT_SOURCE_DIR}")

message(STATUS "Module: ${_projectid}${_modid}")
message("   Source at: ${_moddir}")

install(DIRECTORY 
        "${_moddir}/header/kdsearch"
    DESTINATION include
)
install(FILES
        "${_moddir}/header/${_projectid}${_modid}.h"
    DESTINATION include
)