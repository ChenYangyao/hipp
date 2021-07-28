*************************************
NUMERICAL Module
*************************************

.. _api-numerical-usage:

NUMERICAL Module Overview
==========================

HIPP's NUMERICAL module defines APIs for numerical computation. These include

- Numerical arrays.
- Random number generators.
- Special functions.
- Interpolation.
- One-dimension and multi-dimension Optimization.
- Numerical differentiation and integration.

All the definitions in this module are in the namespace ``HIPP::NUMERICAL``. They 
can be accessed by including the header ``#include <hippnumerical.h>`` and
linking the library ``libhippnumerical.so``. 

Additional library dependency includes the GSL library, i.e., ``-lgsl`` and 
``-lgslcblas`` can be used in the linking stage.

.. _api-numerical-convention:

To avoid confusion, we list the conventions used in this API reference:

- All codes assume a namespace declaration ``using namespace HIPP::NUMERICAL``.
- Definitions in the standard namespace ``std`` are used without the namespace qualifier. E.g., ``cout`` means ``std::cout``, 
  and ``vector`` means ``std::vector``.

API References
================

.. toctree::
    :maxdepth: 2

    sarray

