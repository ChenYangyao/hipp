#[==[

FindFFTWMod
-----------

Dependencies
^^^^^^^^^^^^
`FFTW_ROOT`: the variable containing the path of the library root directory,
which must have
    
    include/fftw3.h

    lib/<fftw3 library> (e.g., libfftw3.so, and libfftw3_omp.so or
    libfftw3_threads.so if omp or threads is required)

Optional Requirements
^^^^^^^^^^^^^^^^^^^^^
OMP
THREADS

Resulted variables
^^^^^^^^^^^^^^^^^^
FFTWMod_FOUND

Cache entry
^^^^^^^^^^^^
FFTWMod_INCLUDE_DIR, FFTWMod_OMP_INCLUDE_DIR, FFTWMod_THREADS_INCLUDE_DIR
FFTWMod_LIBRARY, FFTWMod_OMP_LIBRARY, FFTWMod_THREADS_LIBRARY

Imported library
^^^^^^^^^^^^^^^^
FFTWMod::FFTW
FFTWMod::FFTW_OMP
FFTWMod::FFTW_THREADS

#]==]

find_path(FFTWMod_INCLUDE_DIR "fftw3.h")

find_library(FFTWMod_LIBRARY "fftw3")

if(FFTWMod_FIND_REQUIRED_OMP)
    find_library(FFTWMod_OMP_LIBRARY "fftw3_omp")
endif()

if(FFTWMod_FIND_REQUIRED_THREADS)
    find_library(FFTWMod_THREADS_LIBRARY "fftw3_threads")
endif()

set(fftw_lib_ids FFTWMod_LIBRARY)
foreach(fftw_lib_id IN LISTS FFTW_FIND_COMPONENTS)
    list(APPEND fftw_lib_ids "FFTWMod_${fftw_lib_id}_LIBRARY")
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTWMod
    DEFAULT_MSG 
    FFTWMod_INCLUDE_DIR 
    ${fftw_lib_ids})

if(FFTWMod_FOUND) 
    message(STATUS "Imported library FFTWMod::FFTW")
    message("   Library: ${FFTWMod_LIBRARY}")
    message("   Include dir: ${FFTWMod_INCLUDE_DIR}")

    if(NOT TARGET FFTWMod::FFTW)
        add_library(FFTWMod::FFTW UNKNOWN IMPORTED)
        set_target_properties(FFTWMod::FFTW PROPERTIES
            IMPORTED_LOCATION "${FFTWMod_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${FFTWMod_INCLUDE_DIR}")
    endif()
    

    if(FFTWMod_FIND_REQUIRED_OMP AND NOT TARGET FFTWMod::FFTW_OMP)
        message(STATUS "Imported library FFTWMod::FFTW_OMP")
        message("   Library: ${FFTWMod_OMP_LIBRARY}")

        add_library(FFTWMod::FFTW_OMP UNKNOWN IMPORTED)
        set_target_properties(FFTWMod::FFTW_OMP PROPERTIES
            IMPORTED_LOCATION "${FFTWMod_OMP_LIBRARY}")
        target_link_libraries(FFTWMod::FFTW_OMP
            INTERFACE FFTWMod::FFTW)
        target_compile_options(FFTWMod::FFTW_OMP 
            INTERFACE "-fopenmp")
    endif()

    if(FFTWMod_FIND_REQUIRED_THREADS AND NOT TARGET FFTWMod::FFTW_THREADS)
        message(STATUS "Imported library FFTWMod::FFTW_THREADS")
        message("   Library: ${FFTWMod_THREADS_LIBRARY}")

        add_library(FFTWMod::FFTW_THREADS UNKNOWN IMPORTED)
        set_target_properties(FFTWMod::FFTW_THREADS PROPERTIES
            IMPORTED_LOCATION "${FFTWMod_THREADS_LIBRARY}")
        target_link_libraries(FFTWMod::FFTW_THREADS
            INTERFACE FFTWMod::FFTW)
        target_compile_options(FFTWMod::FFTW_THREADS 
            INTERFACE "-pthread")
    endif()
endif()


