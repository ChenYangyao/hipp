*************************************
CNTL Module
*************************************


.. _api-cntl-usage:

CNTL Module Overview
======================

The CNTL module of HIPP defines a bunch of extensions for the C++ standard library to 
simply the C++ application development.

All the definitions in the CNTL module are in the top-level namespace ``HIPP``. 
To use them 

- include the header ``<hippcntl.h>``;
- link the library ``libhippcntl.so``. 

.. _api-cntl-convention:

The following namespace declarations are adopted for clarity in all example 
codes in this API-ref::

  using namespace HIPP;
  using namespace std;

They expose all definitions in the CNTL model and C++ standard namespace.

API References
================

.. toctree::
    :maxdepth: 2

    stream
    error
    time
    concept

