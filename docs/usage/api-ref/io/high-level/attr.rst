Attribute and Property List Manipulation
================================================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::IO::H5``.

.. namespace:: HIPP::IO::H5

Attr 
-------------------------

.. class:: Attr: public Obj

    Attributes are "small datasets" attached on HDF5 named objects.

    Class ``Attr`` encapsulates the methods available on attributes. 

    **Memory management:** 
    The attribute class is copyable and movable (both in construction and 
    assignment). The copy, move and destruction are ``noexcept``. The copy operation 
    is shallow - the resulting object always refers to the same HDF5 resource
    as the source object. The move operation sets the move-from object an empty
    state.
    
    .. type:: Obj parent_t
        _Attr _obj_raw_t
        std::shared_ptr<_obj_raw_t> _obj_ptr_t

    .. type:: _obj_raw_t::info_t info_t

        Structured type that records attribute meta-info.

    **Constructors:** Class ``Attr`` "inherits" all constructors from its parent class.
        
    .. function:: \
        Dataspace dataspace() const
        Datatype datatype() const

        Return copy of the dataspace or datatype of the attribute.
        The datatype is read-only. 

    .. function:: \
        string get_name() const
        hsize_t get_storage_size() const
        void get_info(info_t &link_info) const
        info_t get_info() const

        Attribute information getter: retrieve the name, storage size allocated, 
        meta-info.

        The second overload of ``get_info()`` is the same except that the attribute
        meta-info is returned rather than the argument being filled.

    .. function:: \
        void write(const void *buff, const Datatype &memtype)    
        template<typename T> \
        void write(const T &buff)
        template<typename T> \
        void write_scalar(const T &x)
        void write_str(const string &s)
        void write_str(const char *s)
        void write_str(const vector<string> &ss)
        void write_str(const string ss[], size_t n_str)
        template<size_t N_STR> \
        void write_str(const string (&ss)[N_STR])
        void write_str(const char * const *ss, size_t n_str)
        template<size_t N> \
        void write_str(const char ss[][N], size_t n_str)
        template<size_t N_STR, size_t N> \
        void write_str(const char (&ss)[N_STR][N])

        Write data into the attribute.
        
        (1): The most general method that is the direct HDF5 C API counterpart. 
        Write a buffer starting at ``buff`` to the attribute with ``memtype``
        specifying is datatype.

        (2): Write an object. The ``buff`` argument can be
        
        - A raw pointer. The memory datatype is inferred from is point-to type 
          by :expr`Datatype::from_type`.
        - any object that is resolvable by :class:`ConstDatapacket`, including 
          numerical scalars (e.g., ``int``, ``float``), 
          general arrays or numerical types (e.g., ``std::array<int, 4>``, 
          ``std::vector<int>``), or general arrays of raw arrays 
          (e.g., ``std::vector< std::array<int, 4> >``).

        Other methods are defined for special cases.

        (3): Write a scalar. The datatype is inferred from T as if calling 
        :expr:`Datatype::from_type<T>()`.

        (4-11): write a string or a list of strings as fix-length ATOMIC string
        datatype. In the list-of-strings case, the size of the string is taken from 
        the maximum of the strings. 
        
        In all cases, the attribute must be created with consistent dataspace and
        datatype.

    .. function:: \
        void read(void *buff, const Datatype &memtype)
        template<typename T> \
        void read(T &&buff)
        template<typename T> \
        void read_scalar(T &x)
        void read_str(string &s)
        void read_str(char *s)
        void read_str(vector<string> &ss)
        void read_str(string ss[])
        template<size_t N> \
        void read_str(char ss[][N])

        Read data from the attribute.
        
        (1): The most general method that is the direct HDF5 C API counterpart. 
        Read into buffer starting at ``buff`` from the attribute with ``memtype``
        specifying its datatype.

        (2): Read into an object. The ``buff`` argument can be
        
        - A raw pointer. The memory datatype is inferred from is point-to type 
          by :expr:`Datatype::from_type`.
        - ``std::vector<T, Alloc>``. ``T`` must be valid argument to 
          :expr:`Datatype::from_type` (e.g., numerical scalar or raw-array).
          The vector is always resized to exactly hold all the elements. 
          If the resize operation cannot exactly fits the need, an ``ErrLogic``
          is thrown.
        - any object that is resolvable by :class:`ConstDatapacket`, including 
          numerical scalars (e.g., ``int``, ``float``), 
          general arrays or numerical types (e.g., ``std::array<int, 4>``).

        Other methods are defined for special cases.

        (3): Read a scalar. The datatype is inferred from T as if calling 
        :expr:`Datatype::from_type<T>()`.

        (4-8): Read a string or a list of strings as fix-length ATOMIC string
        datatype. In the list-of-strings case, the size of the string is taken from 
        the maximum of the strings. 
        The ``std::string`` or ``std::vector`` is auto-resized to fit the data. 
        Otherwise (e.g., ``char *``), the buffer must be large enough to hold
        the string that is read.
        
        In all cases, the attribute must has consistent dataspace and datatype.

    .. function:: \
        _obj_raw_t & obj_raw() noexcept
        const _obj_raw_t & obj_raw() const noexcept

        Return a reference to the intermediate-level HDF5 object.

AttrManager
--------------

.. class:: AttrManager

    Tha manager provides commonly used shortcuts for attribute creation and 
    access.

    .. type:: NamedObj refobj_t

        
    .. function:: \
        AttrManager() noexcept
        explicit AttrManager(refobj_t obj) noexcept

        Constructors.
    
        (1): refers to an empty attribute object. I/O may not be performed in this 
        case.
        
        (2): refers to given dataset.

    .. function:: \
        ~AttrManager() noexcept = default
        AttrManager(const AttrManager &) noexcept = default
        AttrManager(AttrManager &&) noexcept = default
        AttrManager & operator=(const AttrManager &) noexcept = default
        AttrManager & operator=(AttrManager &&) noexcept = default

        The attribute manager is copyable and movable. The copy is shallow with 
        resulting instance referring to the same attribute.
        The move is destroyable, leaving the move-from instance an empty state.

    .. function:: \
        refobj_t refobj() const

        Retrieve the referred object, i.e., the attribute object.
        
    .. function:: \
        void reset(refobj_t obj) noexcept
        void reset() noexcept

        Reset the referred object.

        (1): to a given dataset object.
        
        (2): to an empty state.
        
    .. function:: \
        template<typename T> \
        void put(const string &name, const T &x)
        template<typename T> \
        void put_str(const string &name, const T &s);

        Put operations - write data into attribute of given ``name``.
        
        (1): Create a attribute with given ``name`` whose datatype are dataspace are 
        determined from the data ``x`` and write ``x`` into it.
        If the attribute already exists, open it and update its contents to ``x``
        where ``x`` must have consistent datatype and size.
        ``x`` may be anything that can be used in :expr`NamedObj::create_attr_for`
        except the string overloads.

        (2): the same as (1), but using string data ``x`` acceptable by 
        :expr:`NamedObj::create_attribute_for_str`. Examples include single strings
        (``std::string`` or C-style string ``const char *`` ), vector of 
        strings (``std::vector<std::string>``) or array of strings 
        (``std::string[N]``), 2-D character array (``const char [N_STR][N]``,
        resolved as ``N_STR`` strings with fixed length ``N``).
        The string attribute is created as scalar or 1-D dataspace, with 
        fixed-length ATOMIC STRING datatype.

        
    .. function:: \
        template<typename T> \
        void get(const string &name, T &x)
        template<typename T> \
        void get_str(const string &name, T &s)
        template<typename T> \
        T get(const string &name)
        template<typename T> \
        T get_str(const string &name)

        Get operations: read data from existing attribute of given ``name``.
        
        In all case, the attribute must have consistent datatype and dataspace with
        the argument ``x``.

        (1-2): are similar to ``put`` but perform reading here.
    
        (3-4): are similar to (1-2), respectively, but return the data which are
        read (i.e., the object typed ``T`` gets default-initialized, be read into, 
        and returned).

    .. function:: \
        bool exists(const string &name)

        Find whether or not an attribute of given ``name`` exists in the parent 
        object.

    .. function:: \
        Attr open(const string &name)

        Open an existing attribute named ``name``.





Proplist
--------------

.. class:: Proplist: public Obj 

    Property list that controls the detail behaviors of other HDF5 calls.

    **Memory management:** 
    ``Proplist`` is copyable and movable (both in construction and assignment). 
    The copy, move and destruction are ``noexcept``. The copy operation 
    is shallow - the resulting object always refers to the same HDF5 resource
    as the source object. The move operation sets the move-from object an empty
    state.

    .. type::   Obj parent_t
                _Proplist _obj_raw_t
                std::shared_ptr<_obj_raw_t> _obj_ptr_t

    .. type:: _obj_raw_t::d_layout_t d_layout_t
        
    .. member:: \
            static constexpr d_layout_t laCONTIGUOUS  = _obj_raw_t::laCONTIGUOUS
            static constexpr d_layout_t laCHUNKED     = _obj_raw_t::laCHUNKED
            static constexpr d_layout_t laCOMPACT     = _obj_raw_t::laCOMPACT
            static constexpr d_layout_t laVIRTUAL     = _obj_raw_t::laVIRTUAL

        Dataset layout types.

        
    .. member:: \
            static const hid_t dACCESS = _obj_raw_t::dACCESS
            static const hid_t dCREATE = _obj_raw_t::dCREATE
            static const hid_t dXFER   = _obj_raw_t::dXFER
            static const hid_t fCREATE = _obj_raw_t::fCREATE
            static const hid_t fMOUNT  = _obj_raw_t::fMOUNT
            static const hid_t fACCESS = _obj_raw_t::fACCESS

        Property list class.

    .. member:: \
        static const Proplist vDFLT

        Predefined property list object.

        ``vDFLT``: the default property list.

    
    **Constructors:** class ``Proplist`` "inherits" all constructors from its parent class.

    .. function:: \
        static Proplist create(hid_t cls)

        Create a new property list classed ``cls`` which could be predefined 
        member variables of ``Proplist``.
        Instead of using this call, it is safer to use the methods in specific
        types for property list creation, e.g., ``Dataset::create_property_list()``,
        ``File::create_proplist``.

    .. function:: \
        Proplist & set_layout(d_layout_t layout)
        Proplist & set_layout(const string &layout)
        d_layout_t layout() const
        void layout(string &lo) const

        Set and retrive the layout of dataset on the dataset creation property list. 
        Layout string can be either "contiguous", "chunked", "compact" or "virtual".
        Layout value can be the predefined member values.

    .. function:: \
        Proplist & set_chunk(const Dimensions &dims)
        int chunk_ndims() const
        Dimensions chunk() const

        Set and retrive the chunk size for a chunk-layout dataset.

        ``dims`` must have the same rank (i.e., number of dimensions) as the 
        dataset. ``dims`` cannot be larger than the dims of a fixed-size dataset, 
        and cannot be larger than “2^32-1”. The size of each chunk cannot be larger 
        than 4GB.

        If the layout of the dataset is not "chunk", ``set_chunk()`` change the 
        layout to "chunk".

    .. function:: \
        Proplist & set_chunk_cache(size_t nslot, size_t nbytes, double w0)
        void chunk_cache( size_t &nslot, size_t &nbytes, double &w0 ) const
        std::tuple<size_t, size_t, double> chunk_cache() const

        Set and retrive the cache method of the dataset on the dataset access 
        property list. If set by user, override the cache method of the file.
        

    .. function:: \
        Proplist & set_cache( size_t nslot, size_t nbytes, double w0 )
        void cache(size_t &nslot, size_t &nbytes, double &w0) const
        std::tuple<size_t, size_t, double> cache() const

        Set and retrive the cache method of all datasets of a file on the dataset 
        access property list.

    .. function:: \
        Proplist & preserve(hbool_t status)
        hbool_t preserve() const

        Dataset transfer.

    .. function:: \    
        _obj_raw_t & obj_raw() noexcept
        const _obj_raw_t & obj_raw() const noexcept

        Return a reference to the intermediate-level HDF5 object.



    
