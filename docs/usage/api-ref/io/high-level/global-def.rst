Module Overview and Global Definitions
========================================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::IO::H5``.

.. namespace:: HIPP::IO::H5

.. _api-io-h5-objects:

HDF5 Objects 
-----------------

The basic HDF5 objects are :class:`Dataspace`, :class:`Proplist`, :class:`Attr`,
:class:`Datatype`, :class:`Dataset`, :class:`Group`, and :class:`File`. 

Among them, :class:`Datatype`, :class:`Dataset`, :class:`Group`, and :class:`File` 
are "named objects" that can be stored in files and connected by links.
Addionally, a :class:`File` object can be used where a  :class:`Group` object 
is needed (the "root" group is implicitly used by the library).

To reflect the logic relations of those objects, HIPP adopts the following inheritance
graphs of them. 

.. graphviz::

    digraph {
        node [ fontsize="9" height="0.1" 
            shape="polygon" style="filled" fillcolor="#ff7f0e88"]
        edge [minlen="0.2" arrowsize="0.5" dir="back" 
            fillcolor="#a6cee3" color="#1f78b4"]
        rankdir="LR"
        nodesep="0.1"

        nodeObj [label="Obj" shape="ellipse" style="filled" fillcolor="#1f77b488"]
        nodeNamedObj [label="NamedObj" shape="ellipse" style="filled" fillcolor="#1f77b488"]

        nodeDataspace [label="Dataspace"]
        nodeProplist [label="Proplist"]
        nodeAttr [label="Attr"]


        nodeDatatype [label="Datatype"]
        nodeDataset [label="Dataset"]
        nodeGroup [label="Group"]
        nodeFile [label="File"]

        nodeGroup -> nodeFile
        nodeNamedObj -> nodeDatatype
        nodeNamedObj -> nodeDataset
        nodeNamedObj -> nodeGroup
        
        
        nodeObj -> nodeNamedObj
        nodeObj -> nodeDataspace
        nodeObj -> nodeProplist
        nodeObj -> nodeAttr
        
        subgraph G0 {
            rank = "same"
            edge [style="invisible" dir="none"]
            nodeObj -> nodeDataspace -> nodeProplist -> nodeAttr
        }

        subgraph G1 {
            rank = "same"
            edge [style="invisible" dir="none"]
            nodeNamedObj -> nodeDatatype -> nodeDataset -> nodeGroup -> nodeFile
        }

        subgraph G01 {
            rank = "same"
            edge [style="invisible" dir="none"]
        }
    }

The inheritance has strong impact on how the APIs of those objects are used.
For example:

- :class:`File` inherits all methods from :class:`Group`. Hence, you can create links, 
  extract meta-info of them, visit/iterate links and sub objects, create/open datasets, 
  from a :class:`File` instance.
- :class:`Datatype`, :class:`Dataset`, :class:`Group`, and :class:`File` inherit 
  all methods from :class:`NamedObj`. Therefore, they can create/open attributes 
  and iterate over them, by using the API defined in :class:`NamedObj`.
- All of these types are sub class of :class:`Obj`. Hence, the memory management API
  defined in :class:`Obj`, such as :expr:`Obj::raw()`, :expr:`Obj::free()`, and
  :expr:`Obj::has_referenced()` can be used on instance of any HDF5 type.

However, some different methods that have the name do appear (static bind) both 
in the sub class and the parent class. For example:

- Each class defines its own version of ``obj_raw()`` which returns a referene 
  to intermediate-level HDF5 object counterpart. Therefore, ``file.obj_raw()``
  returns :expr:`_File &`, while ``file.NamedObj::obj_raw()`` returns 
  :expr:`_NamedObj &`.
- :class:`Group` and :class:`NamedObj` both has its own version of ``get_info()``,
  which returns the group meta-info and object-meta info, respectively.
  Hence, ``group.get_info()`` returns the group meta-info of the instance,
  while ``group.NamedObj::get_info()`` returns its meta-info as a named object.
  To simplify the usage,
  we do provide a method :expr:`Group::get_object_info()` which has the same 
  effect of the later call.


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
