Module Overview and Global Definitions
========================================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::IO::H5``.

.. namespace:: HIPP::IO::H5

Global Definitions
--------------------

.. type::   hid_t hid_t
            hsize_t hsize_t
            hssize_t hssize_t
            herr_t herr_t
            htri_t htri_t

.. type::   H5_index_t          index_t
            H5_iter_order_t     iter_order_t

.. var::    static constexpr index_t idxNAME = H5_INDEX_NAME
            static constexpr index_t idxCRT_ORDER = H5_INDEX_CRT_ORDER

    Index types used in HDF5 index table.

.. var::    static constexpr iter_order_t iterINC = H5_ITER_INC
            static constexpr iter_order_t iterDEC = H5_ITER_DEC
            static constexpr iter_order_t iterNATIVE = H5_ITER_NATIVE

    Index access orders when visiting sub-objects in a group.


For backward compatibility, we define the following types in the namespace ``HIPP::IO``.
They are aliased to the corresponding types in ``HIPP::IO::H5`` by stripping the prefix ``H5``.

.. namespace:: HIPP::IO
        
.. type:: ErrH5 = H5::ErrH5

.. type::  H5File = H5::File
        H5Group = H5::Group
        H5Dataset = H5::Dataset
        H5Datatype = H5::Datatype
        H5Attr = H5::Attr
        H5Dataspace = H5::Dataspace
        H5Proplist = H5::Proplist

.. type:: template<typename R> H5XTable = H5::XTable<R>
