***************************************
The Numerical Library
***************************************

.. include:: /global.rst

.. namespace:: HIPP::NUMERICAL

.. _tutor-numerical-using-the-lib:

Using the Numerical Library
==============================

To use HIPP's NUMERICAL module, include the header ``<hippnumerical.h>``. 
A minimal cpp source file is like :download:`numerical-minimal-example.cpp </../example/tutorial/numerical/numerical-minimal-example.cpp>`::

    // numerical-minimal-example.cpp

    #include <hippnumerical.h>                  // [1]

    using namespace HIPP;
    using namespace std;
    namespace nu = HIPP::NUMERICAL;             // [2]

    int main(int argc, char const *argv[])
    {
        // use the library
        return 0;
    }

Debrief:

- ``[1]``: all the NUMERICAL components are declared within the header file ``<hippnumerical.h>``.
- ``[2]``: all the NUMERICAL objects are defined within the namespace ``HIPP::NUMERICAL``. 
  We alias it as ``nu`` for clarity throughout this tutorial.

To compile and execute, run:

.. code-block:: bash

    g++ -std=c++17 numerical-minimal-example.cpp -lhippnumerical -lhippcntl -lgsl -lgslcblas
    ./a.out

Each componenet in this library is detailed in one of the following sections:

.. toctree::
    :maxdepth: 2

    array
    rannum
    kdsearch
    
