add_subdirectory(cntl)

option(enable-mpi "install mpi component?" OFF)
if(enable-mpi)
    message(STATUS "MPI enabled")
    add_subdirectory(mpi)
else()
    message(STATUS "MPI disabled. Module mpi will not be built")
endif()

option(enable-hdf5 "install HDF5 IO component?" OFF)
if(enable-hdf5)
    message(STATUS "HDF5 enabled")
    add_subdirectory(io)
else()
    message(STATUS "HDF5 disabled. Module IO will not be built")
endif()

