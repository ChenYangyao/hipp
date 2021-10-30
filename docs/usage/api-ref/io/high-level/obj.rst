Object and Named Objects 
===========================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::IO::H5``.

.. namespace:: HIPP::IO::H5

Obj
-------

.. class:: Obj

    The base class of all HDF5 objects. 

    ``Obj`` is copyable and movable (both in construction and assignment). 
    The copy, move and destruction are ``noexcept``. The copy operation 
    is shallow - the resulting object always refers to the same HDF5 resource
    as the source object. The move operation sets the move-from object an empty
    state.

    .. type:: _Obj _obj_raw_t
                std::shared_ptr<_Obj> _obj_ptr_t

    .. function:: explicit Obj() noexcept
        explicit Obj(_obj_ptr_t ptr) noexcept
        explicit Obj(_obj_raw_t *ptr) noexcept

        Constructors.
        
        ``ptr``: HDF5 resource (represented by an intermediate-level object) that is to
        be taken over by the instance. If nullptr, the instance is initialized as 
        an empty state that refers to no HDF5 resource 
        (i.e., ``has_referenced()->false``).
        
        (1): default constructor - get an empty-state object.
        
        (2,3): use a shared pointer or a raw pointer.

    .. function:: virtual ~Obj() noexcept
    
    .. function:: \
        Obj(const Obj &obj) noexcept
        Obj(Obj &&obj) noexcept
        Obj & operator=(const Obj &obj) noexcept
        Obj & operator=(Obj &&obj) noexcept

    .. function:: void free() noexcept

        Detach the instance from the underlying HDF5 resource. If no other object
        refers to this resource, the resource gets destroyed.
    
        On return, the instance becomes empty.

    .. function:: \
        hid_t raw() const noexcept

        Return the original HDF5 resource/object handler (type ``hid_t``).
    
    .. function:: \
        _obj_raw_t & obj_raw() noexcept
        const _obj_raw_t & obj_raw() const noexcept

        Return the intermediate-level HDF5 object.

    .. function:: \
        bool has_referenced() const noexcept
        explicit operator bool () const noexcept

        Find whether or not the object refers to an HDF5 resource.
        If not, the object is in an empty state.

    .. function:: \
        bool operator==(const Obj &o) const
        bool operator!=(const Obj &o) const

        Equality test - return true if either
    
        - lhs and rhs are both empty;
        - lhs and rhs refers to the same HDF5 resource (i.e., represented by the 
          same value of HDF5 identifier typed ``hid_t``).

        In any other case, two objects are unequal. For example, if one dataset
        in a file is opened by two calls of ``Group::open_dataset()``, the two
        resulting dataset objects are unequal.


NamedObj
------------

.. class:: NamedObj : public Obj

    NameObj - the base class of all named HDF5 object (``Dataset``, ``Datatype``, 
    ``Group`` and ``File``).

    **Memory management:** 
    The named object class is copyable and movable (both in construction and 
    assignment). The copy, move and destruction are ``noexcept``. The copy operation 
    is shallow - the resulting object always refers to the same HDF5 resource
    as the source object. The move operation sets the move-from object an empty
    state.

    .. type:: Obj parent_t
        _NamedObj _obj_raw_t
        std::shared_ptr<_obj_raw_t> _obj_ptr_t

    .. type:: _obj_raw_t::info_t info_t

        Structured type that records object meta-info.

    .. type:: _obj_raw_t::info_field_t info_field_t
    
    .. member:: \
            static constexpr info_field_t infoBASIC      = _obj_raw_t::infoBASIC
            static constexpr info_field_t infoTIME       = _obj_raw_t::infoTIME
            static constexpr info_field_t infoNUM_ATTRS  = _obj_raw_t::infoNUM_ATTRS
            static constexpr info_field_t infoHDR        = _obj_raw_t::infoHDR
            static constexpr info_field_t infoMETA_SIZE  = _obj_raw_t::infoMETA_SIZE
            static constexpr info_field_t infoALL        = _obj_raw_t::infoALL

            Fields of named object meta-info.

    .. type:: _obj_raw_t::type_t type_t
        
    .. member:: \
            static constexpr type_t tUNKNOWN        = _obj_raw_t::tUNKNOWN
            static constexpr type_t tGROUP          = _obj_raw_t::tGROUP
            static constexpr type_t tDATASET        = _obj_raw_t::tDATASET
            static constexpr type_t tNAMED_DATATYPE = _obj_raw_t::tNAMED_DATATYPE
            static constexpr type_t tMAP            = _obj_raw_t::tMAP
            static constexpr type_t tNTYPES         = _obj_raw_t::tNTYPES

        Object types.

    .. type:: \
            _Attr::info_t attr_info_t
            _named_obj_attr_helper::iter_arg_t attr_iter_arg_t
            _named_obj_attr_helper::iter_op_t attr_iter_op_t



    **Constructors:** Class ``NamedObj`` "inherits" all constructors from its parent class.

    .. function:: \
        void get_info(info_t &obj_info, info_field_t  fields = infoALL) const;

        Retrive the object meta info of the current object.

        ``info``: object into which the meta info is put.
        
        The second overload returns the info structure instead of fills the 
        argument.

    .. function:: \
        void get_attr_info(const string &obj_name, const string &attr_name,  \
            attr_info_t &info, const Proplist &laprop = Proplist::vDFLT) const
        void get_attr_info(const string &obj_name, hsize_t idx, attr_info_t &info, \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,  \
            const Proplist &laprop = Proplist::vDFLT) const
        attr_info_t get_attr_info(const string &obj_name, const string &attr_name, \
            const Proplist &laprop = Proplist::vDFLT) const
        attr_info_t get_attr_info(const string &obj_name, hsize_t idx,         \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,       \
            const Proplist &laprop = Proplist::vDFLT) const

        Get meta-info of an attribute named ``attr_name`` under object named 
        ``obj_name``, or indexed ``idx`` under that object.

        ``idx_type``, ``order``: which type of index is used and in which order the object 
        is visited in the index list.

        The two overloads that return ``attr_info_t`` are the same except that the
        attribute information is returned rather than the argument being filled.


    .. function:: \
        string get_attr_name(const string &obj_name, hsize_t idx, \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE,  \
            const Proplist &laprop = Proplist::vDFLT) const
        
        Get the name of the attribute indexed ``idx`` under the object named 
        ``obj_name``.

        ``idx_type``, ``order``: which type of index is used and in which order the object 
        is visited in the index list.


    .. function:: \
        bool attr_exists(const string &name) const

        Find whether or not the attribute of given ``name`` exists.


    .. function:: \
        void rename_attr(const string &old_name, const string &new_name)
        void delete_attr(const string &name)
        void delete_attr(const string &obj_name, const string &attr_name,  \
            const Proplist &laprop = Proplist::vDFLT)
        void delete_attr(const string &obj_name, hsize_t idx,         \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, \
            const Proplist &laprop = Proplist::vDFLT)
        
        Attribute modification methods.

        ``rename_attr()``: change the name of an attribute.
        
        ``delete_attr()``: delete the attribute named ``name`` under the current object,
        or under the object named ``obj_name``, or indexed ``idx`` under the object
        named ``obj_name``.

        ``idx_type``, ``order``: which type of index is used and in which order the object 
        is visited in the index list.

    .. function:: \
        herr_t attr_iterate(hsize_t &idx, attr_iter_op_t op, void *op_data=nullptr, \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE) const
        herr_t attr_iterate(const string &obj_name, hsize_t &idx,  \
            attr_iter_op_t op, void *op_data=nullptr, \
            index_t idx_type = idxNAME, iter_order_t order = iterNATIVE, \
            const Proplist &laprop = Proplist::vDFLT) const

        Iteration methods: iterate over the attributs in the current object, 
        or in a sub object named ``obj_name``, starting at index ``idx``.
        On exit, ``idx`` indicates the next position to iterate. 

        The user-provided callback ``op`` may return

        - 0: then the iteration continues, until success. 
        - positive: shortcut success, causing the method returns immediately with
          that returned value.
        - negative: short failure, causing the iteration stops and throws an 
          :class:`ErrH5` with that returned value as error number.
        
        ``op_data``: passed to ``op``.
        ``idx_type``, ``order``: which type of index is used and in which order the object 
        is visited in the index list.


    .. function:: \
        AttrManager attrs() noexcept

        Returns an attribute manager of this named object.
        Tha manager provides frequently-used shortcuts for attribute creation and 
        access.

    .. function:: \
        Attr create_attr(const string &name,  \
            const Datatype &dtype, const Dataspace &dspace,  \
            const string &flag = "ac", \
            const Proplist &cprop = Proplist::vDFLT, \ 
            const Proplist &aprop = Proplist::vDFLT)
        template<typename T> \
        Attr create_attr(const string &name, const Dataspace &dspace, \
            const string &flag = "ac",  \
            const Proplist &cprop = Proplist::vDFLT,  \
            const Proplist &aprop = Proplist::vDFLT)
        template<typename T> \
        Attr create_attr_scalar(const string &name, const string &flag = "ac")
        Attr create_attr_str(const string &name, size_t n, \
            const string &flag = "ac")
        Attr create_attr_str(const string &name, size_t n_str, size_t n, \
            const string &flag = "ac")

        Create an attributed named ``name`` under the current object.
        
        (1): The most general method that is the direct HDF5 C API counterpart.
        
        (2): The datatype is inferred from ``T`` as if calling 
        :expr:`Datatype::from_type<T>()`.

        Other methods are defined for special cases. All of them use the default 
        property list :expr:`Proplist::vDFLT`.

        (3): Create a scalar dataset whose datatype is inferred from T as if calling 
        :expr:`Datatype::from_type<T>()`.
        
        (4-5): Create a dataset for fix-length ATOMIC STRING sized ``n``, or a list 
        of ``n_str`` such strings. ``n`` must include the space for the null-term.

        ``flag``: determine what to do if the attribute already exists. Can be

        .. table::
            :class: tight-table
        
            =========================== =====================================================================
            ``flag``                    Description
            =========================== =====================================================================
            ``"x"``                     failed and throw an ``ErrH5`` exception.
            ``"ac"`` | ``"ca"``         open it anyway. In this case ``cprop`` are ignored.
            ``"trunc"`` | ``"w"``       (deprecated) the same as "ac".
            ``"excl"``                  (deprecated) the same as "x".
            =========================== =====================================================================

    .. function:: \
        template<typename T> \
        Attr create_attr_for(const string &name, const T &buff, \
            const string &flag = "ac")
        template<typename T> \
        Attr create_attr_for_scalar(const string &name, const T &x, \
            const string &flag = "ac")
        Attr create_attr_for_str(const string &name, const string &s, \
            const string &flag = "ac")
        Attr create_attr_for_str(const string &name, const char *s, \
            const string &flag = "ac")
        Attr create_attr_for_str(const string &name, const vector<string> &ss, \
            const string &flag = "ac")
        template<size_t N_STR> \
        Attr create_attr_for_str(const string &name, const string (&ss)[N_STR], \
            const string &flag = "ac")
        template<size_t N_STR, size_t N> \
        Attr create_attr_for_str(const string &name, const char (&ss)[N_STR][N], \
            const string &flag = "ac")

        ``create_attr_for()``: create an attribute from the object to be written. The 
        library infers the datatype and dataspace from the object.
        
        (1): any non-string object, which can be any single object that is 
        resolvable by :class:`ConstDatapacket`. The returned attribute can be
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
        ATOMIC ARRAY datatype here. The returned attribute can be applied with
        ``write_scalar(x)``.

        (3-7): string of list of strings. The returned attribute can therefore
        be written by ``write_str(s)`` or ``write_str(ss)``. The created attribute
        has fix-length ATOMIC STRING datatype.
    
    .. function:: \
        Attr open_attr(const string &name, \
            const Proplist &aprop = Proplist::vDFLT) const

        Open an existing attribute of given path name ``name``.

    .. function:: \        
        _obj_raw_t & obj_raw() noexcept
        const _obj_raw_t & obj_raw() const noexcept

        Return a reference to the intermediate-level HDF5 object.

.. class:: _named_obj_attr_helper::iter_arg_t

    .. function:: \
        NamedObj & object() noexcept
        const string & name() const noexcept
        const info_t & info() const noexcept
        void * op_data() const noexcept
        
        Retrieve the root object, current attribute name, current attribute info and 
        user-provided data in the iteration.


.. type:: std::function< herr_t(_named_obj_attr_helper::iter_arg_t &) > _named_obj_attr_helper::iter_op_t