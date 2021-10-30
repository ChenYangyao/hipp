**********************************************
General-purpose C++ Utilities
**********************************************

.. include:: /global.rst

.. namespace:: HIPP

Using the Library
===================

To use HIPP's CNTL module, include the header ``<hippcntl.h>``. A minimal cpp source
file is like::

    // cntl-minimal-example.cpp
    #include <hippcntl.h>

    using namespace HIPP;
    using namespace std;

    int main(int argc, char const *argv[]) {
        // use the library
        return 0;
    }

Note that we adopt the above namespace declarations for clarity throughout this 
tutorial.

To generate executables, just compile and link the libraries. Then run it

.. code-block:: bash

    g++ -std=c++17 cntl-minimal-example.cpp -lhippcntl
    ./a.out

The details of the module components are covered by the following sub-sections:

.. toctree:: 
    :maxdepth: 2
    
    stream_manipulation
    exception_handling
    logging
    timing
    concept
