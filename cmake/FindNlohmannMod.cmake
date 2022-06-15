#[==[

FindNlohmannMod
---------------

Dependencies
^^^^^^^^^^^^
`Nlohmann_ROOT`: the variable containing the path of the library root directory,
which must have
    include/nlogmann/json.hpp

Resulted variables
^^^^^^^^^^^^^^^^^^
NlohmannMod_FOUND

Cache entry
^^^^^^^^^^^
NlohmannMod_INCLUDE_DIR

Imported library
^^^^^^^^^^^^^^^^
NlohmannMod::JSON

#]==]

find_path(NlohmannMod_INCLUDE_DIR "nlohmann/json.hpp")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NlohmannMod
    DEFAULT_MSG
    NlohmannMod_INCLUDE_DIR)

if(NlohmannMod_FOUND AND NOT TARGET NlohmannMod::JSON)
    message(STATUS "Imported library NlohmannMod::JSON")
    message("   Include dir: ${NlohmannMod_INCLUDE_DIR}")
    
    add_library(NlohmannMod::JSON INTERFACE IMPORTED)
    target_include_directories(NlohmannMod::JSON
        INTERFACE "${NlohmannMod_INCLUDE_DIR}"
    )
endif()