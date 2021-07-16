*********************************************
SIMD Module
*********************************************

.. _api-simd-usage:

SIMD Module Overview
======================

The ``HIPP::SIMD`` module includes the templated version of SIMD instrunctions.
Currently it only supports the Intel x86 platform where AVX vector instructions are implemented 
(see `Intel's Guide <https://software.intel.com/sites/landingpage/IntrinsicsGuide/>`_ 
and check the `Wiki <https://en.wikipedia.org/wiki/Advanced_Vector_Extensions>`_ page to see 
the processors that supports SIMD). Some of the operations need ``AVX2``, ``AVX512F``, ``AVX512VL`` 
supports.

All the definitions in this SIMD module can be accessed by including the header file **<hippsimd.h>**.
All SIMD classes are defined within the namespace ``HIPP::SIMD``. 
This is a pure-header module, no library is needed to link.
Refer to the manual of the compiler you use to see how to enable SIMD extensions. 
For example, ``-mavx2``, ``-mavx512f``, ``-mavx512vl`` enables the AVX2, AVX512F and AVX512VL 
extension in GCC, respectively. To achieve the best performance, optimization flags need 
to be enabled in the compiling process,
e.g.:

.. code-block:: bash 

    $ g++ -std=c++17 -O3 -mavx2 src.cpp

.. _api-simd-convention:

To avoid confusion, we list the conventions used in this API reference:

* The example code that uses C++ standard library may ignore the ``std::`` namespace specifier (namely, assume 
  a global ``using namespace std`` is used). This is not good in practice, but makes the example code more compact.



API References
================

.. toctree::
    :maxdepth: 2

    common-def
    avx