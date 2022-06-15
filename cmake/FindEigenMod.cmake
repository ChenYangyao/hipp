#[==[

FindEigenMod
-----------

Dependencies
^^^^^^^^^^^^
`Eigen_ROOT`: the variable containing the path of the library root directory,
which must have
    include/Eigen/Dense

Resulted variables
^^^^^^^^^^^^^^^^^^
EigenMod_FOUND

Cache entry
^^^^^^^^^^^^
EigenMod_INCLUDE_DIR

Imported library
^^^^^^^^^^^^^^^^
EigenMod::Eigen

#]==]


find_path(EigenMod_INCLUDE_DIR "Eigen/Dense")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EigenMod
    DEFAULT_MSG
    EigenMod_INCLUDE_DIR)

if(EigenMod_FOUND AND NOT TARGET EigenMod::Eigen)
    message(STATUS "Imported library EigenMod::Eigen")
    message("   Include dir: ${EigenMod_INCLUDE_DIR}")
    
    add_library(EigenMod::Eigen INTERFACE IMPORTED)
    target_include_directories(EigenMod::Eigen
        INTERFACE "${EigenMod_INCLUDE_DIR}"
    )
endif()