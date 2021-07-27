*************************************
CNTL Module
*************************************


.. _api-cntl-usage:

CNTL Module Overview
======================

The CNTL module of HIPP defines a bunch of extensions for the C++ standard library to make 
the development easier.

All the definitions in the CNTL module are in the top-level namespace ``HIPP``. They 
can be accessed by including the header **<hippcntl.h>**,
and linking the library ``libhippcntl.so``. 

.. _api-cntl-convention:

To avoid confusion, we list the conventions used in this API reference:

* The documentation text of one definition would probably refer to other definitions. If they are in the 
  same namespace (``HIPP`` here), we drop the namespace specifiers for clarity 
  (e.g., class :class:`PStream <HIPP::PStream>` means class :class:`HIPP::PStream`). 
* The example code that uses C++ standard library may ignore the ``std::`` namespace specifier (namely, assume 
  a global ``using namespace std`` is used). This is not good in practice, but makes the example code more compact. 

API References
================

.. toctree::
    :maxdepth: 2

    stream
    error
    time

