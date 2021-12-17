Quick Start
=====================

.. include:: /global.rst

.. _quickstart-about-the-lib:

About the Library
--------------------

.. _quickstart-modules:

Modules
""""""""""

The HIPP library is organized as several modules:

- The ``CNTL`` module consists of global definitions and general utilities of HIPP. 
  Examples include the IO streams :var:`~HIPP::pout`, :var:`~HIPP::perr`, the log stream :var:`~HIPP::plog`, 
  the exceptions layers, error macros, and timing utilities. 
  All are in the global ``namespace HIPP``.
- Other modules, including ``MPI``, ``IO``, ``NUMERICAL``, ``ALGORITHM``, and ``SIMD``, 
  are designed for specific tasks and defined in the corresponding sub-namespaces.

To use a module, 

- include the corresponding *header file* into your source code;
- link the corresponding *library file* (except the header-only modules).

Note that the library file of one module may depend on those of other modules or 
third-party libraries. Refer to the tutorial of each module for the details.

The following figure illustrates the layout of the modules:

.. _fig-quickstart-overall-structure:
.. figure:: imgs/overall-structure.svg
    :width: 750px
    :align: center

    The header files, library files, namespaces and utilities in the modules of HIPP.

.. _quickstart-naming-convention:

Naming Convention
""""""""""""""""""""

We adopt the following naming convention for library components:

- Namespaces are UpperCased. For example, the global namespace ``HIPP`` and 
  sub-namespace ``HIPP::MPI``.
- Classes are CamelCased. For example, the MPI Group class :class:`~HIPP::MPI::Group` 
  and the logic error exception class :class:`~HIPP::ErrLogic`.
- Free-functions and class methods are LowerCased. For example, the printing function
  :func:`~HIPP::prt` and the message sending method of MPI communicator :func:`~HIPP::MPI::send`.
- Non-constant static variables are LowerCased. 
  Constant static variables, enumerators, and macros are not named with any 
  defining convention, due to their diversity of types.

In rare cases, we do have names that break the above convention. Refer to the 
API-Ref for their exact meaning if they are not clear.


Get Started
---------------

Your First MPI Program
""""""""""""""""""""""""

The MPI module of HIPP provides high-level wrapper to the standard C-API of MPI.
To use, include ``<hippmpi.h>`` and put suitable namespace declarations like 
the following::

    #include <hippmpi.h>

    using namespace HIPP;
    using namespace std;
    namespace mpi = HIPP::MPI;

At the beginning of any MPI application, initialize the environment by defining 
a named variable typed :class:`~HIPP::MPI::Env`. The WORLD communicator is 
returned from the environment. The process rank and number are returned from the 
communicator::

    mpi::Env env(argc, argv);
    auto comm = env.world();
    int rank = comm.rank(), n_procs = comm.size();

To make a point-to-point communication, call :func:`~HIPP::MPI::Comm::send` / :func:`~HIPP::MPI::Comm::recv`
on a communicator by passing the rank of the target process, a communication tag, and the data object.
For example, we send 10 values from process 0 to 1::

    vector<double> buff(10);

    if( rank == 0 ) 
        comm.send(1, 0, buff);
    else if( rank == 1 ) 
        comm.recv(0, 0, buff);

To make a collective communication, such as a broadcast, call :func:`~HIPP::MPI::Comm::bcast`
on a communicator by passing the data object and the root rank. For example, we broadcast 
the above buffer to all processes::

    comm.bcast(buff, 0);

The compilation and execution are described in the Tutorial :ref:`tutor-mpi-using-the-lib`.

HDF5 IO 
""""""""""

The IO module of HIPP defines high-level wrapper to the standard HDF5 API. 
To use, include ``<hippio.h>`` and put suitable namespace declarations like the 
following::

    #include <hippio.h>

    using namespace HIPP;
    using namespace std;
    namespace H5 = HIPP::IO::H5;

To create a new file in HDF5 format, define a :class:`~HIPP::IO::H5::File` instance
by passing a file name and the access mode ``"w"``::

    H5::File fout("data.h5", "w");

Datasets under (the root group of) this file are managed by 
a :class:`~HIPP::IO::H5::DatasetManager` which can be obtained by::

    auto dsets = fout.datasets();

Suppose you have the following data objects to be saved into the file::

    double d34[3][4] = {};
    vector<int> i10(10);
    string s = "Hello HIPP!";

The method :func:`~HIPP::IO::H5::DatasetManager::put` of the manager creates
new dataset for numerical array and write data object to it. The method 
:func:`~HIPP::IO::H5::DatasetManager::put_str`, instead, works with string 
data::

    dsets.put("d34", d34);
    dsets.put("i10", i10);
    dsets.put_str("s", s);

To load back the data from file, call :func:`~HIPP::IO::H5::DatasetManager::get`
and :func:`~HIPP::IO::H5::DatasetManager::get_str`::

    dsets.get("d34", d34);
    dsets.get("i10", i10);
    dsets.get_str("s", s);

    pout << "d34 = {", pout(d34[0], d34[0]+12), "}\n",
            "i10 = {", i10, "}\n",
            "s = ", s, "\n";

The compilation and execution are described in the Tutorial :ref:`tutor-io-h5-using-the-lib`.

The content of ``"data.h5"`` shown by ``h5dump`` is:

.. code-block:: text

    HDF5 "data.h5" {
    GROUP "/" {
       DATASET "d34" {
          DATATYPE  H5T_IEEE_F64LE
          DATASPACE  SIMPLE { ( 3, 4 ) / ( 3, 4 ) }
          DATA {
          (0,0): 0, 0, 0, 0,
          (1,0): 0, 0, 0, 0,
          (2,0): 0, 0, 0, 0
          }
       }
       DATASET "i10" {
          DATATYPE  H5T_STD_I32LE
          DATASPACE  SIMPLE { ( 10 ) / ( 10 ) }
          DATA {
          (0): 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
          }
       }
       DATASET "s" {
          DATATYPE  H5T_STRING {
             STRSIZE 12;
             STRPAD H5T_STR_NULLTERM;
             CSET H5T_CSET_ASCII;
             CTYPE H5T_C_S1;
          }
          DATASPACE  SCALAR
          DATA {
          (0): "Hello HIPP!"
          }
       }
    }
    }
