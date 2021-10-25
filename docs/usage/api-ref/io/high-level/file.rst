File and Group
===============

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::IO::H5``.

.. namespace:: HIPP::IO::H5

File
------

.. class:: File : public Group

    ``File`` provides interfaces to manipulate (create, open, or retrieve metainfo from) HDF5 files.

    **Memory management:** 
    The file class is copyable and movable (both in construction and 
    assignment). The copy, move and destruction are ``noexcept``. The copy operation 
    is shallow - the resulting object always refers to the same HDF5 resource
    as the source object. The move operation sets the move-from objects an empty
    state.

    .. type:: Group parent_t
        _File _obj_raw_t
        std::shared_ptr<_obj_raw_t> _obj_ptr_t

    **Constructors:**
    Class ``File`` "inherits" all constructors from its parent class.


    .. function:: File(const string &name, const string &flag = "ac", \
                    const Proplist &cporp = Proplist::vDFLT, \
                    const Proplist &aprop = Proplist::vDFLT)

        Create a new file or open an existing file.

        ``name``:  file name

        ``flag``:  file access, must be one of the following

        .. table:: 
            :class: tight-table

            =================== ================================================================
            ``flag``            Description
            =================== ================================================================
            ``"w"``             create and truncate, then open as R/W mode.
            ``"x"``             exclusively create (failed if existing), then open as R/W mode.
            ``"ac"`` | ``"ca"`` open existing file as R/W mode. Create it if not existing.
            ``"r"``             open existing file as R mode.
            ``"a"``             open existing file as R/W mode.
            =================== ================================================================
        
        ``cprop, aprop``: creation and access property lists. If to open existing one, cprop
        is ignored.    

    .. function::static File open(const string &name, const string &flag = "a", \
            const Proplist &aprop = Proplist::vDFLT)

        Open an existing file named ``name`` in file system.
        
        ``flag`` specifies the access mode, must be "r" (Read-only) | "a" (W/R).

    .. function:: static File create(const string &name, const string &flag = "w", \
        const Proplist &cprop = Proplist::vDFLT, \
        const Proplist &aprop = Proplist::vDFLT)

        Create a file.
        
        ``flag`` specifies the access mode, must be "w" (truncate if existing) | "x" (fail if existing).

    .. function:: static Proplist create_proplist(const string &cls = "c")

        Create a property list for file operation.
        
        ``cls``: The property class, must be be one from

        .. table:: 
            :class: tight-table
        
            ========================= ===================================================
            ``cls``                   Description
            ========================= ===================================================
            ``"c"`` | ``"create"``    file creation property list.
            ``"a"`` | ``"access"``    file access property list.
            ``"m"`` | ``"mount"``     file mount property list.
            ========================= ===================================================

    .. function:: _obj_raw_t & obj_raw() noexcept
        const _obj_raw_t & obj_raw() const noexcept

        Return a reference to the intermediate-level HDF5 object.
    
Group 
-------------------


.. class:: Group: public NamedObj

    Groups are named objects used to organized other objects in a HDF5 file.
    Class ``Group`` encapsulates the methods available on groups. 

    **Memory management:** 
    The group class is copyable and movable (both in construction and 
    assignment). The copy, move and destruction are ``noexcept``. The copy operation 
    is shallow - the resulting object always refers to the same HDF5 resource
    as the source object. The move operation sets the move-from objects an empty
    state.

    .. type:: NamedObj parent_t
        _Group _obj_raw_t
        std::shared_ptr<_obj_raw_t> _obj_ptr_t

        
    .. type:: _obj_raw_t::info_t          info_t
        
        Structured type that records group meta-info.

    .. type:: _obj_raw_t::storage_type_t  storage_type_t

    .. member:: \
            static constexpr storage_type_t storeCOMPACT_T          = _obj_raw_t::storeCOMPACT_T
            static constexpr storage_type_t storeDENSE_T            = _obj_raw_t::storeDENSE_T
            static constexpr storage_type_t storeSYMBOL_TABLE_T     = _obj_raw_t::storeSYMBOL_TABLE_T

        Storage types of group.

    .. type:: _Link::info_t link_info_t

        Structured type that records link meta-info.
        
    .. type:: _Link::type_t link_type_t;

    .. member:: 
            static constexpr link_type_t tERROR     = _Link::tERROR
            static constexpr link_type_t tHARD      = _Link::tHARD
            static constexpr link_type_t tSOFT      = _Link::tSOFT
            static constexpr link_type_t tEXTERNAL  = _Link::tEXTERNAL
            static constexpr link_type_t tMAX       = _Link::tMAX;

        Type of links.
        

    .. type::   NamedObj::info_t        obj_info_t

        Structured type that records object meta-info.

    .. type::   NamedObj::info_field_t  obj_info_field_t
                NamedObj::type_t        obj_type_t

    **Constructors:** Class ``Group`` "inherits" all constructors from its parent class.

    .. function:: \
        void get_info(info_t &group_info) const
        void get_info(const string &name, info_t &group_info,  \
            const Proplist &laprop = Proplist::vDFLT) const
        void get_info(const string &group_name, hsize_t idx, info_t &group_info,  \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, \
            const Proplist &laprop = Proplist::vDFLT) const
        info_t get_info() const
        info_t get_info(const string &name,  \
            const Proplist &laprop = Proplist::vDFLT) const
        info_t get_info(const string &group_name, hsize_t idx, \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,  \
            const Proplist &laprop = Proplist::vDFLT) const

        Retrieve group meta info of the current object, of a sub object by ``name``,
        or of the sub sub object indexed ``idx`` in a sub group ``group_name`` (
        can be "." to denote the group itself).

        ``group_info``: object into which the meta info is put.

        ``idx_type, order``: which type of index is used and in which order the object 
        is visited in the index list.

        The three overloads that return ``info_t`` are the same except the the group
        information is returned rather than filled into the argument.
        
    .. function:: \        
        void get_object_info(obj_info_t &info, \
            info_field_t fields = NamedObj::infoALL) const
        void get_object_info(const string &name, obj_info_t &info,  \
            info_field_t fields = NamedObj::infoALL, \
            const Proplist &laprop = Proplist::vDFLT) const
        void get_object_info(const string &group_name,  \ 
            hsize_t idx, obj_info_t &info, \
            info_field_t fields = NamedObj::infoALL, \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, \
            const Proplist &laprop = Proplist::vDFLT) const
        obj_info_t get_object_info( \
            info_field_t fields = NamedObj::infoALL) const
        obj_info_t get_object_info(const string &name,  \
            info_field_t fields = NamedObj::infoALL, \
            const Proplist &laprop = Proplist::vDFLT) const
        obj_info_t get_object_info(const string &group_name, hsize_t idx,  \
            info_field_t fields = NamedObj::infoALL, \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,  \
            const Proplist &laprop = Proplist::vDFLT) const

        Retrive the object meta info of the current object, of a sub object by 
        ``name``, or of a sub sub object indexed ``idx`` in the sub group 
        ``group_name`` (can be "." to denote the group itself). 

        ``info``: object into which the meta info is put.

        ``idx_type, order``: which type of index is used and in which order the object 
        is visited in the index list.

        The three overloads that return ``obj_info_t`` are the same except the the 
        object information is returned rather than filled into the argument.

    .. function:: \
        void get_link_info(const string &name, link_info_t &info, \
            const Proplist &laprop = Proplist::vDFLT) const
        void get_link_info(const string &group_name, hsize_t idx, link_info_t &info,    \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,    \
            const Proplist &laprop = Proplist::vDFLT) const
        link_info_t get_link_info(const string &name, \
            const Proplist &laprop = Proplist::vDFLT) const
        link_info_t get_link_info(const string &group_name, hsize_t idx,    \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, \
            const Proplist &laprop = Proplist::vDFLT) const
        void get_link_val(const string &name, vector<char> &buff,  \
            const Proplist &laprop = Proplist::vDFLT) const
        void get_link_val(const string &group_name, hsize_t idx, vector<char> &buff, \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, \
            const Proplist &laprop = Proplist::vDFLT) const
        string get_link_name(const string &group_name, hsize_t idx,  \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, \
            const Proplist &laprop = Proplist::vDFLT) const

        Retrive the link meta info of a sub object by ``name``, or of a sub sub 
        object indexed ``idx`` in the sub group ``group_name`` 
        (can be "." to denote the group itself). 

        ``info``: object into which the meta info is put.

        ``idx_type, order``: which type of index is used and in which order the object 
        is visited in the index list.

        The two overloads that return ``link_info_t`` are the same except the the 
        link information is returned rather than filled into the argument.

        ``get_link_val()`` and ``get_link_name()`` return the link value and name,
        respectively. 

        ``buff``: on return, link value is filled into it and its size is adjusted
        to fit.

    .. function:: DatasetManager datasets() noexcept

        Returns a dataset manager of this group.
        Tha manager provides commonly used shortcuts for dataset creation and 
        access.


    .. function:: \
        bool link_exists(const string &name, \
            const Proplist &laprop = Proplist::vDFLT) const
        bool object_exists(const string &name,  \
            const Proplist &laprop = Proplist::vDFLT) const
        bool object_exists_by_type(const string &name,  \
            obj_type_t obj_type, const Proplist &laprop = Proplist::vDFLT) const
        bool group_exists(const string &name) const
        bool dataset_exists(const string &name) const

        Find whether or not certain item exists in the current group. If any of the
        intermediate steps in the name does not exists, or does not resolve to
        an object, or the final target link does not exits, return false.
        
        (1): for link of any type. 

        (2): the same as (1) but also check that the target link resolves to an 
        object of any type.
        
        (3): the same as (2), but further require that the target match the object
        type given by ``obj_type``.
        
        (4,5): special cases of (3), and with default link access property list.

    
    .. function:: \
        Group create_group(const string &name, const string &flag="ac", \
            const Proplist &lcprop = Proplist::vDFLT, \
            const Proplist &gcprop = Proplist::vDFLT, \
            const Proplist &gaprop  = Proplist::vDFLT)

        Create a group with link name ``name`` under the current group.
        
        ``flag``: determine what to do if the group named ``name`` already exists. Can be

        .. table::
            :class: tight-table
        
            =========================== =====================================================================
            ``flag``                    Description
            =========================== =====================================================================
            ``"x"``                     failed and throw an ``ErrH5`` exception.
            ``"ac"`` | ``"ca"``         open it anyway. In this case ``lcprop`` and ``gcprop`` are ignored.
            ``"trunc"`` | ``"w"``       (deprecated) the same as "ac".
            ``"excl"``                  (deprecated) the same as "x".
            =========================== =====================================================================

    .. function:: \
        Group open_group(const string &name, \
            const Proplist &aprop = Proplist::vDFLT) const

        Open an existing group with path name ``name``.

    .. function:: \
        Dataset create_dataset(const string &name, \
            const Datatype &dtype, const Dataspace &dspace, \
            const string &flag = "ac",\
            const Proplist &lcprop = Proplist::vDFLT, \
            const Proplist &dcprop  = Proplist::vDFLT, \
            const Proplist &daprop  = Proplist::vDFLT)
        template<typename T> \
        Dataset create_dataset(const string &name, const Dataspace &dspace, \
            const string &flag = "ac", \
            const Proplist &lcprop = Proplist::vDFLT,  \
            const Proplist &dcprop  = Proplist::vDFLT,  \
            const Proplist &daprop  = Proplist::vDFLT)
        template<typename T> \
        Dataset create_dataset_scalar(const string &name,  \
            const string &flag = "ac")
        Dataset create_dataset_str(const string &name, size_t n, \
            const string &flag = "ac")
        Dataset create_dataset_str(const string &name, size_t n_str, size_t n, \
            const string &flag = "ac")

        Create a dataset named ``name`` under the current group.
        
        (1): The most general method that is the direct HDF5 C API counterpart.
        
        (2): The datatype is inferred from ``T`` as if calling 
        :expr:`Datatype::from_type<T>()`.

        Other methods are defined for special cases. All of them use the default 
        property list :expr:`Proplist::vDFLT`.

        (3): Create a scalar dataset whose datatype is inferred from T as if calling 
        :expr:`Datatype::from_type<T>()`.
        
        (4-5): Create a dataset for fix-length ATOMIC STRING sized ``n``, or a list 
        of ``n_str`` such strings. ``n`` must include the space for the null-term.

        ``flag``: determine what to do if the dataset already exists. Can be

        .. table::
            :class: tight-table
        
            =========================== =====================================================================
            ``flag``                    Description
            =========================== =====================================================================
            ``"x"``                     failed and throw an ``ErrH5`` exception.
            ``"ac"`` | ``"ca"``         open it anyway. In this case ``lcprop`` and ``dcprop`` are ignored.
            ``"trunc"`` | ``"w"``       (deprecated) the same as "ac".
            ``"excl"``                  (deprecated) the same as "x".
            =========================== =====================================================================

    .. function:: \
        template<typename T> \
        Dataset create_dataset_for(const string &name, const T &buff, \
            const string &flag = "ac")
        template<typename T> \
        Dataset create_dataset_for_scalar(const string &name,  \
            const T &x, const string &flag = "ac")
        Dataset create_dataset_for_str(const string &name,  \
            const string &s, const string &flag = "ac")
        Dataset create_dataset_for_str(const string &name,  \
            const char *s, const string &flag = "ac")
        Dataset create_dataset_for_str(const string &name,  \
            const vector<string> &ss, const string &flag = "ac")
        template<size_t N_STR> \
        Dataset create_dataset_for_str(const string &name,  \
            const string (&ss)[N_STR], const string &flag = "ac")
        template<size_t N_STR, size_t N> \
        Dataset create_dataset_for_str(const string &name, \
            const char (&ss)[N_STR][N], const string &flag = "ac")

        ``create_dataset_for()``: create a dataset from the object to be written. The 
        library infers the datatype and dataspace from the object.
        
        (1): any non-string object, which can be any single object that is 
        resolvable by :class:`ConstDatapacket`. The returned dataset can be
        called with ``write(buff)`` to write the object into it. Examples of 
        ``buff`` include

        - single scalar value, e.g., ``int x``;
        - array-like object, e.g., ``std::array<int, 3> x``, ``int x[3][4]``;
        - vector of scalar values, e.g., ``std::vector<double> x``;
        - vector of array-like objects, e.g., 
          ``std::vector< std::array<int, 3> > x``.
        
        (2): any object that can be resolved as a ATOMIC type, i.e., any valid
        type to :func:`Datatype::from_cvt`. 
        The difference from (1) is that, array-like objects are treated as 
        ATOMIC ARRAY datatype here. The returned dataset can be applied with
        ``write_scalar(x)``.

        (3-7): string of list of strings. The returned attribute can therefore
        be written by ``write_str(s)`` or ``write_str(ss)``. The created dataset
        has fix-length ATOMIC STRING datatype.

    .. function:: \
        Dataset open_dataset(const string &name, \
            const Proplist &aprop = Proplist::vDFLT) const

        Open an existing dataset of given path name ``name``.

    .. function:: \
        _obj_raw_t & obj_raw() noexcept
        const _obj_raw_t & obj_raw() const noexcept

        Return a reference to the intermediate-level HDF5 object.

