#[==[

FindMPIMod
-----------

Dependencies
^^^^^^^^^^^^
`MPI_CXX_COMPILER`: the MPI compiler wrapper.

Resulted variables
^^^^^^^^^^^^^^^^^^
MPIMod_FOUND

Cache entry
^^^^^^^^^^^
MPIMod_INCLUDE_DIR
MPIMod_LIBRARY
MPIMod_FLAGS

Imported library
^^^^^^^^^^^^^^^^
MPIMod::MPI

#]==]

set(mpi_mod_find_flags)
if(MPIMod_FIND_REQUIRED)
    list(APPEND mpi_mod_find_flags "REQUIRED")
endif()
if(MPIMod_FIND_QUIETLY)
    list(APPEND mpi_mod_find_flags "QUIET")
endif()

find_package(MPI ${mpi_mod_find_flags})

set(MPIMod_FOUND "Whether or not MPI Module is found?" ${MPI_CXX_FOUND})

if(MPIMod_FOUND AND NOT TARGET MPIMod::MPI)
    message(STATUS "Imported library MPIMod::MPI")
    message("   Include dirs: ${MPI_CXX_INCLUDE_PATH}")
    message("   Libraries: ${MPI_CXX_LIBRARIES}")
    message("   Compile flags: ${MPI_CXX_COMPILE_FLAGS}")

    set(MPIMod_INCLUDE_DIR "${MPI_CXX_INCLUDE_PATH}" CACHE STRING "MPI include directories")
    set(MPIMod_LIBRARY "${MPI_CXX_LIBRARIES}" CACHE STRING "MPI libraries")
    set(MPIMod_FLAGS "${MPI_CXX_COMPILE_FLAGS}" CACHE STRING "MPI compile flags")

    add_library(MPIMod::MPI INTERFACE IMPORTED)
    target_link_libraries(MPIMod::MPI
        INTERFACE MPI::MPI_CXX
    )
endif()