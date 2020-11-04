Common Defitions
=========================

The following classes are all defined within namespace ``HIPP::SIMD``.

.. namespace:: HIPP::SIMD

.. class:: template<typename ScaleT, size_t NPack> Vec

    A vector of NPack scalar values typed ScaleT. This is the common base class for all SIMD vector types. 

    See the specializations for the detailed definitions of each vector type, e.g., :class:`Vec\<double, 4>` and :class:`Vec\<float, 8>`.