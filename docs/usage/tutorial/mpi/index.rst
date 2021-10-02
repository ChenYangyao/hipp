*************************************
Process-level Parallelism with MPI
*************************************

Basic Usage 
--------------

.. toctree:: 
    :maxdepth: 2

    basic_usage/simple_communication
    basic_usage/datatype
    basic_usage/comm
    basic_usage/one_side
    basic_usage/file_io

Advanced Topics 
----------------

.. toctree::
    :maxdepth: 2

    advanced/comm_mode
    advanced/inter

Example Codes Gallery
-----------------------

.. table::
    :class:     fix-width-table tight-table
    :widths:    30 30 40

    =========================================================================== ========================================================================================================================= ==============================================================================================
    Topic                                                                       Codes                                                                                                                     Description
    =========================================================================== ========================================================================================================================= ==============================================================================================
    :ref:`tutor-mpi-env`                                                        :download:`get-started.cpp </../example/tutorial/mpi/get-started.cpp>`                                                    Use :class:`~HIPP::MPI::Env` to start the MPI environment.
    :ref:`tutor-mpi-p2p`                                                        :download:`basic-p2p.cpp </../example/tutorial/mpi/basic-p2p.cpp>`                                                        Use :class:`~HIPP::MPI::Comm` to send/recv data.
    :ref:`tutor-mpi-collective`                                                 :download:`basic-collective.cpp </../example/tutorial/mpi/basic-collective.cpp>`                                          Use :class:`~HIPP::MPI::Comm` for collective data movement and computation.
    :ref:`tutor-app-distributed-sum`                                            :download:`app-distributed-sum.cpp </../example/tutorial/mpi/app-distributed-sum.cpp>`                                    Example: use :func:`~HIPP::MPI::Comm::scatter` and :func:`~HIPP::MPI::Comm::reduce` for 
                                                                                                                                                                                                          task decomposition and result reduction.
    :ref:`tutor-app-pi-computation`                                             :download:`app-pi-computation.cpp </../example/tutorial/mpi/app-pi-computation.cpp>`                                      Example: approximate :math:`\pi` with numerical integration.
    :ref:`tutor-app-matrix-vec-dot`                                             :download:`app-matrix-vec-dot.cpp </../example/tutorial/mpi/app-matrix-vec-dot.cpp>`                                      Example: matrix-vector multiplication :math:`c=A\times b` with self-scheduling.
    =========================================================================== ========================================================================================================================= ==============================================================================================

