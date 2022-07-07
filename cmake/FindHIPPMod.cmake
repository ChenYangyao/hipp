#[==[

FindHIPPMod
-----------

Dependencies
^^^^^^^^^^^^
`HIPP_ROOT`: the variable containing the path of the library root directory,
which must have
    
    include/hippcntl.h (and other headers if required)
    lib/<hipp libraries> (e.g., libhippcntl.so, and others if required)

Optional Requirements
^^^^^^^^^^^^^^^^^^^^^
IO
NUMERICAL
MPI
SIMD
ALGORITHM

Resulted variables
^^^^^^^^^^^^^^^^^^
HIPPMod_FOUND

Cache entry
^^^^^^^^^^^^
HIPPMod_INCLUDE_DIR
HIPPMOD_{CNTL|IO|NUMERICAL|MPI}_LIBRARY

Imported library
^^^^^^^^^^^^^^^^
HIPPMod::{CNTL|IO|NUMERICAL|MPI|SIMD|ALGORITHM}

#]==]

find_path(HIPPMod_INCLUDE_DIR "hippcntl.h")

find_library(HIPPMod_CNTL_LIBRARY "hippcntl")

set(hipp_lib_ids HIPPMod_CNTL_LIBRARY)

if(HIPPMod_FIND_REQUIRED_IO)
    find_library(HIPPMod_IO_LIBRARY "hippio")
    find_package(HDF5Mod REQUIRED)
    list(APPEND hipp_lib_ids HIPPMod_IO_LIBRARY)
endif()

if(HIPPMod_FIND_REQUIRED_NUMERICAL)
    find_library(HIPPMod_NUMERICAL_LIBRARY "hippnumerical")
    find_package(GSLMod REQUIRED)
    list(APPEND hipp_lib_ids HIPPMod_NUMERICAL_LIBRARY)
endif()

if(HIPPMod_FIND_REQUIRED_MPI)
    find_library(HIPPMod_MPI_LIBRARY "hippmpi")
    find_package(MPIMod REQUIRED)
    list(APPEND hipp_lib_ids HIPPMod_MPI_LIBRARY)
endif()

if(HIPPMod_FIND_REQUIRED_SIMD)
    find_path(HIPPMod_SIMD_INCLUDE_DIR "hippsimd.h")
    list(APPEND hipp_lib_ids HIPPMod_SIMD_INCLUDE_DIR)
endif()

if(HIPPMod_FIND_REQUIRED_ALGORITHM)
    find_path(HIPPMod_ALGORITHM_INCLUDE_DIR "hippalgorithm.h")
    list(APPEND hipp_lib_ids HIPPMod_ALGORITHM_INCLUDE_DIR)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HIPPMod DEFAULT_MSG 
    HIPPMod_INCLUDE_DIR ${hipp_lib_ids})

if(HIPPMod_FOUND)
    
    message(STATUS "Imported library HIPPMod::CNTL")
    message("   Library: ${HIPPMod_CNTL_LIBRARY}")
    message("   Include dir: ${HIPPMod_INCLUDE_DIR}")

    if(NOT TARGET HIPPMod::CNTL)
        add_library(HIPPMod::CNTL UNKNOWN IMPORTED)
        set_target_properties(HIPPMod::CNTL PROPERTIES
            IMPORTED_LOCATION "${HIPPMod_CNTL_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${HIPPMod_INCLUDE_DIR}"
        )
    endif()

    if(HIPPMod_FIND_REQUIRED_IO AND NOT TARGET HIPPMod::IO)
        message(STATUS "Imported library HIPPMod::IO")
        message("   Library: ${HIPPMod_IO_LIBRARY}")

        add_library(HIPPMod::IO UNKNOWN IMPORTED)
        set_target_properties(HIPPMod::IO PROPERTIES
            IMPORTED_LOCATION "${HIPPMod_IO_LIBRARY}")
        target_link_libraries(HIPPMod::IO 
            INTERFACE HIPPMod::CNTL HDF5Mod::HDF5)
    endif()

    if(HIPPMod_FIND_REQUIRED_NUMERICAL AND NOT TARGET HIPPMod::NUMERICAL)

        message(STATUS "Imported library HIPPMod::NUMERICAL")
        message("   Library: ${HIPPMod_NUMERICAL_LIBRARY}")

        add_library(HIPPMod::NUMERICAL UNKNOWN IMPORTED)
        set_target_properties(HIPPMod::NUMERICAL PROPERTIES
            IMPORTED_LOCATION "${HIPPMod_NUMERICAL_LIBRARY}")
        target_link_libraries(HIPPMod::NUMERICAL 
            INTERFACE HIPPMod::CNTL GSLMod::GSL)
    endif()

    if(HIPPMod_FIND_REQUIRED_MPI AND NOT TARGET HIPPMod::MPI)

        message(STATUS "Imported library HIPPMod::MPI")
        message("   Library: ${HIPPMod_MPI_LIBRARY}")

        add_library(HIPPMod::MPI UNKNOWN IMPORTED)
        set_target_properties(HIPPMod::MPI PROPERTIES
            IMPORTED_LOCATION "${HIPPMod_MPI_LIBRARY}")
        target_link_libraries(HIPPMod::MPI 
            INTERFACE HIPPMod::CNTL MPIMod::MPI)
    endif()

    if(HIPPMod_FIND_REQUIRED_SIMD AND NOT TARGET HIPPMod::SIMD)
        message(STATUS "Imported library HIPPMod::SIMD")
        add_library(HIPPMod::SIMD INTERFACE IMPORTED)
        target_link_libraries(HIPPMod::SIMD 
            INTERFACE HIPPMod::CNTL)
    endif()

    if(HIPPMod_FIND_REQUIRED_ALGORITHM AND NOT TARGET HIPPMod::ALGORITHM)
        message(STATUS "Imported library HIPPMod::ALGORITHM")
        add_library(HIPPMod::ALGORITHM INTERFACE IMPORTED)
        target_link_libraries(HIPPMod::ALGORITHM 
            INTERFACE HIPPMod::CNTL)
    endif()

endif()
