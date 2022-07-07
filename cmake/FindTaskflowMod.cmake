#[==[

FindTaskflowMod
---------------

Dependencies
^^^^^^^^^^^^
`Taskflow_ROOT`: the variable containing the path of the library root directory,
which must have
    include/taskflow/taskflow.hpp

Resulted variables
^^^^^^^^^^^^^^^^^^
TaskflowMod_FOUND

Cache entry
^^^^^^^^^^^
TaskflowMod_INCLUDE_DIR

Imported library
^^^^^^^^^^^^^^^^
TaskflowMod::Taskflow

#]==]

find_path(TaskflowMod_INCLUDE_DIR "taskflow/taskflow.hpp")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TaskflowMod
    DEFAULT_MSG
    TaskflowMod_INCLUDE_DIR)

if(TaskflowMod_FOUND AND NOT TARGET TaskflowMod::Taskflow)
    message(STATUS "Imported library TaskflowMod::Taskflow")
    message("   Include dir: ${TaskflowMod_INCLUDE_DIR}")
    
    add_library(TaskflowMod::Taskflow INTERFACE IMPORTED)
    target_include_directories(TaskflowMod::Taskflow
        INTERFACE "${TaskflowMod_INCLUDE_DIR}"
    )
    target_compile_options(TaskflowMod::Taskflow INTERFACE "-pthread")
    target_link_options(TaskflowMod::Taskflow INTERFACE "-pthread")
endif()