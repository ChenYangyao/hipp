*************************************
IO Module
*************************************

.. include:: /global.rst

.. _api-io-usage:

IO Module Overview
====================

The IO module of HIPP provides 

- High-level, pure-OOP wrappers for the HDF5 library.
- Extentions to simplify common I/O tasks.

All the HDF5 wrappers and extensions are defined in the namespace ``HIPP::IO::H5``. 
To use them, 

- include the header ``<hippio.h>``;
- link the library ``libhippio.so``, ``libhippcntl.so``
- link ``libhdf5.so``, if the HDF5 library is built as dynamic.

The following namespace declarations are adopted for clarity in all example
codes in this API-ref::

  namespace H5 = HIPP::IO::H5;
  using namespace HIPP;
  using namespace std; 

List of Module Components 
==========================

.. namespace:: HIPP::IO::H5

.. table::
    :class: tight-table

    ==================================================================== ====================================================== ====================================================
    Class                                                                Corresponding HDF5 Standard C API                      Details
    ==================================================================== ====================================================== ====================================================
    :class:`File`                                                        H5F                                                    File manipulation.
    :class:`Group`                                                       H5G, H5L                                               Data group operations.
    :class:`Dataset`                                                     H5D                                                    Dataset operations.
    :class:`Datatype`                                                    H5T                                                    Type system.
    :class:`Dataspace`                                                   H5S                                                    Dataspace operations.
    :class:`Proplist`                                                    H5P                                                    Property list.
    :class:`Attr`                                                        H5A                                                    Attributes.
    :class:`Obj`
    :class:`NamedObj`                                                    H5O                                                    Base class of all HDF5 objects.
    :class:`XTable`                                                                                                             Base class of all named HDF5 objects.
    :class:`Dimensions`         
    :class:`Points`
    :class:`Chunk`
    :class:`Hyperslab`
    :class:`Datapacket`, :class:`ConstDatapacket`                                                                               Convert C++ object to H5 datatype and dataspace.
    :class:`DatapacketScalar`, :class:`ConstDatapacketScalar`                                                                   Scalar versions of ``Datapacket``.
    :class:`DatapacketStr`, :class:`ConstDatapacketStr`                                                                         String versions of ``Datapacket``.
    ==================================================================== ====================================================== ====================================================


API References
================

High-level API 
---------------

.. toctree::
    :maxdepth: 2

    high-level/global-def
    high-level/obj
    high-level/file 
    high-level/dataset 
    high-level/attr
    high-level/error
    high-level/util

Extended API 
--------------

.. toctree::
    :maxdepth: 2

    ext/tabular


Intermediate-level API
------------------------

.. toctree::
    :maxdepth: 2

    intermediate-level/datatype


