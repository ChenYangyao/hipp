#[==[

FindYAMLMod
-----------

Dependencies
^^^^^^^^^^^^
`YAML_ROOT`: the variable containing the path of the library root directory,
which must have
    include/yaml-cpp/yaml.h
    lib/<yaml-cpp library> (e.g., libyaml-cpp.so)

Resulted variables
^^^^^^^^^^^^^^^^^^
YAMLMod_FOUND

Cache entry
^^^^^^^^^^^
YAMLMod_INCLUDE_DIR
YAMLMod_LIBRARY

Imported library
^^^^^^^^^^^^^^^^
YAMLMod::YAML

#]==]

find_path(YAMLMod_INCLUDE_DIR "yaml-cpp/yaml.h")

find_library(YAMLMod_LIBRARY "yaml-cpp")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YAMLMod
    DEFAULT_MSG 
    YAMLMod_INCLUDE_DIR YAMLMod_LIBRARY)

if( YAMLMod_FOUND AND NOT TARGET YAMLMod::YAML )
    message(STATUS "Imported library YAMLMod::YAML")
    message("   Library: ${YAMLMod_LIBRARY}")
    message("   Include dir: ${YAMLMod_INCLUDE_DIR}")
    
    add_library(YAMLMod::YAML UNKNOWN IMPORTED)
    set_target_properties(YAMLMod::YAML PROPERTIES
        IMPORTED_LOCATION "${YAMLMod_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${YAMLMod_INCLUDE_DIR}"
    )
endif()