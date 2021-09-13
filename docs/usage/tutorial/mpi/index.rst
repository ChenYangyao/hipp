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
    :ref:`tutor-mpi-env`                                                        :download:`get-started.cpp </../example/tutorial/mpi/get-started.cpp>`                                                    Use :class:`~HIPP::MPI::Env` to start the MPI environment
    :ref:`tutor-mpi-p2p`                                                        :download:`basic-p2p.cpp </../example/tutorial/mpi/basic-p2p.cpp>`                                                        Use :class:`~HIPP::MPI::Comm` to send/recv data
    =========================================================================== ========================================================================================================================= ==============================================================================================

