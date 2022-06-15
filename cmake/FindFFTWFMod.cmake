#[==[

FindFFTWFMod
------------

Dependencies
^^^^^^^^^^^^
`FFTWF_ROOT`: the variable containing the path of the library root directory,
which must have
    
    include/fftw3f/fftw3.h

    lib/<fftw3f library> (e.g., libfftw3f.so, and libfftw3f_omp.so or
    libfftw3f_threads.so if omp or threads is required)

Optional Requirements
^^^^^^^^^^^^^^^^^^^^^
OMP
THREADS

Resulted variables
^^^^^^^^^^^^^^^^^^
FFTWFMod_FOUND

Cache entry
^^^^^^^^^^^
FFTWFMod_INCLUDE_DIR, FFTWFMod_OMP_INCLUDE_DIR, FFTWFMod_THREADS_INCLUDE_DIR
FFTWFMod_LIBRARY, FFTWFMod_OMP_LIBRARY, FFTWFMod_THREADS_LIBRARY

Imported library
^^^^^^^^^^^^^^^^
FFTWFMod::FFTWF
FFTWFMod::FFTWF_OMP
FFTWFMod::FFTWF_THREADS

#]==]

find_path(FFTWFMod_INCLUDE_DIR "fftw3f/fftw3.h")

find_library(FFTWFMod_LIBRARY "fftw3f")

if(FFTWFMod_FIND_REQUIRED_OMP)
    find_library(FFTWFMod_OMP_LIBRARY "fftw3f_omp")
endif()

if(FFTWFMod_FIND_REQUIRED_THREADS)
    find_library(FFTWFMod_THREADS_LIBRARY "fftw3f_threads")
endif()

set(fftwf_lib_ids FFTWFMod_LIBRARY)
foreach(fftwf_lib_id IN LISTS FFTWF_FIND_COMPONENTS)
    list(APPEND fftwf_lib_ids "FFTWFMod_${fftwf_lib_id}_LIBRARY")
endforeach()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FFTWFMod
    DEFAULT_MSG 
    FFTWFMod_INCLUDE_DIR 
    ${fftwf_lib_ids})

if(FFTWFMod_FOUND) 
    if(NOT TARGET FFTWFMod::FFTWF)
        message(STATUS "Imported library FFTWFMod::FFTWF")
        message("   Library: ${FFTWFMod_LIBRARY}")
        message("   Include dir: ${FFTWFMod_INCLUDE_DIR}")

        add_library(FFTWFMod::FFTWF UNKNOWN IMPORTED)
        set_target_properties(FFTWFMod::FFTWF PROPERTIES
            IMPORTED_LOCATION "${FFTWFMod_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${FFTWFMod_INCLUDE_DIR}")
    endif()
    
    if(FFTWFMod_FIND_REQUIRED_OMP AND NOT TARGET FFTWFMod::FFTWF_OMP)
        message(STATUS "Imported library FFTWFMod::FFTWF_OMP")
        message("   Library: ${FFTWFMod_OMP_LIBRARY}")

        add_library(FFTWFMod::FFTWF_OMP UNKNOWN IMPORTED)
        set_target_properties(FFTWFMod::FFTWF_OMP PROPERTIES
            IMPORTED_LOCATION "${FFTWFMod_OMP_LIBRARY}")
        target_link_libraries(FFTWFMod::FFTWF_OMP
            INTERFACE FFTWFMod::FFTWF)
        target_compile_options(FFTWFMod::FFTWF_OMP 
            INTERFACE "-fopenmp")
    endif()

    if(FFTWFMod_FIND_REQUIRED_THREADS AND NOT TARGET FFTWFMod::FFTWF_THREADS)
        message(STATUS "Imported library FFTWFMod::FFTWF_THREADS")
        message("   Library: ${FFTWFMod_THREADS_LIBRARY}")

        add_library(FFTWFMod::FFTWF_THREADS UNKNOWN IMPORTED)
        set_target_properties(FFTWFMod::FFTWF_THREADS PROPERTIES
            IMPORTED_LOCATION "${FFTWFMod_THREADS_LIBRARY}")
        target_link_libraries(FFTWFMod::FFTWF_THREADS
            INTERFACE FFTWFMod::FFTWF)
        target_compile_options(FFTWFMod::FFTWF_THREADS 
            INTERFACE "-pthread")
    endif()
endif()


