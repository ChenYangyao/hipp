Datatype, Dataspace and Dataset
==================================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::IO::H5``.

.. namespace:: HIPP::IO::H5

Datatype
-----------------

.. class:: Datatype : public NamedObj
    
    Datatype describe the bit-level memory layout, representation and conversion
    of data element in HDF5 dataset and attribute.

    Class ``Datatype`` encapsulates the methods available on datatypes. 

    The library predefines a set of native and standard datatypes, based on which, 
    or from scratch, customized datatype can be derived. 

    **Memory management:** 
    The datatype class is copyable and movable (both in construction and 
    assignment). The copy, move and destruction are ``noexcept``. The copy operation 
    is shallow - the resulting object always refers to the same HDF5 resource
    as the source object. The move operation sets the move-from objects an empty
    state.

    .. type:: NamedObj parent_t
        _Datatype _obj_raw_t
        std::shared_ptr<_obj_raw_t> _obj_ptr_t

    .. _api-io-h5-dtype-class-enum:

    .. type:: class_t

    .. member:: static constexpr class_t cCOMPOUND
                static constexpr class_t cOPAQUE
                static constexpr class_t cENUM
                static constexpr class_t cSTRING
                static constexpr class_t cARRAY
                static constexpr class_t cINTEGER
                static constexpr class_t cFLOAT
                static constexpr class_t cBITFIELD
                static constexpr class_t cREFERENCE
                static constexpr class_t cVLEN

        Datatype classes. The first four can be passed into method :func:`create` 
        to create new datatypes. All of them may be returned by :func:`get_class` 
        or :func:`member_class`.

    **Constructors:** class ``Datatype`` "inherits" all constructors from its 
    parent class.

    .. function:: Datatype copy() const

        Produce a deep copy of the datatype.

    .. function:: void commit(Group &group, const string &name, const Proplist &lcprop, \
            Proplist &tcprop, Proplist &taprop)

        Commite the datatype to file. The committed datatype (named datatype) 
        may be shared by many objects.

    .. function:: void lock()

        Lock the datatype so that it cannot be changed. All the predefined datatypes
        are already locked.
    
    .. function:: bool committed() const
        bool equal(const Datatype &dtype) const
        size_t size() const
        class_t get_class() const

        Retrieve the meta-info from the datatype instance, whether or not it is 
        committed, whether it is equal to another datatype instance, its size, 
        and its datatype class.

    .. function:: void resize(size_t size)
        Datatype resized(size_t size) const
    
        ``resize()``: sets the size of the current datatype instance.

        ``resized()``: produces a resized copy (i.e., similar to ``copy()`` and then
        ``resize()``).

    **Example:** get datatype for string object::

        string s = "hello, world";              // String to save into file.

    To write it to a HDF5 file, first define a STRING datatype for it by::

        auto str_dtype = H5::C_S1_T.copy();
        str_dtype.resize( s.size() + 1 );       // Reserve the space for `\0`

    Or, equivalently, use resized copy directly::
        
        str_dtype = H5::C_S1_T.resized( s.size() + 1 );

    Then, just create a datatype with this datatype, and write the string::

        H5::File file("str_dtype.h5");
        file.create_dataset("s", str_dtype, H5::Dataspace::vSCALAR).write_str(s);

    The above ``create_dataset()`` is equivalent to either of the following two 
    calls::

        file.create_dataset_str("s", s.size()+1).write_str(s);
        file.create_dataset_for_str("s", s).write_str(s);

    The content of the file shown by ``h5dump`` is :

    .. code-block:: text 

        HDF5 "str_dtype.h5" {
        GROUP "/" {
        DATASET "s" {
            DATATYPE  H5T_STRING {
                STRSIZE 13;
                STRPAD H5T_STR_NULLTERM;
                CSET H5T_CSET_ASCII;
                CTYPE H5T_C_S1;
            }
            DATASPACE  SCALAR
            DATA {
            (0): "hello, world"
            }
        }
        }
        }

    .. function:: void set_precision(size_t precision)

        For ATOMIC datatypes, set the numerical precision to ``precision``.

    .. function:: static Datatype create(class_t c, size_t size)

        General new datatype creators. 

        ``c``: datatype class. Can be ``cCOMPOUND`` | ``cOPAQUE`` | ``cENUM`` | 
        ``cSTRING``.

        ``size``: the size in bytes.

        More specific creators like ``create_compound()`` and ``create_array()`` 
        are easier in most cases.

    .. function:: static Datatype create_compound(size_t size)
        template<typename Record, typename Member, typename ...Args>\
        static Datatype create_compound(const string &mem_name, \
            Member Record::*mem_ptr, Args &&...args)

        Methods for the creation of COMPOUND datatypes. 

        ``create_compound(name1, ptr1, name2, ptr2, ...)``: 
        directly creates a new compound datatype (usually corresponding to a C++ 
        struct/class). Arguments are passed pair-wisely to define the name and
        and datatype detail of each member in the COMPOUND datatype. The detail
        is described by the member pointer from which the datatype and offset 
        are inferred by the library.
        
        The C++ type of the member can be any valid type to :expr:`Datatype::from_type` 
        (e.g., ``int``, ``float[3][4][5]``). 

        ``create_compound(size)``: create an empty COMPOUND datatype sized ``size`` 
        bytes. 
        
        Members can be inserted later by :func:`insert`.

        **Example:** create a compound datatype. Assume the following structured 
        C++ type is defined in a application::

            struct S {
                int a;
                double b[3];
                float c[2][3];
                array<array<long, 3>, 4> d;
            };

        To save data of such type into a HDF5 file, define a compound datatype 
        for it by::

            auto dtype = H5::Datatype::create( H5::Datatype::cCOMPOUND, sizeof(S) );
        
            // Or, equivalently
            dtype = H5::Datatype::create_compound(sizeof(S));

        Then, insert each member by either offset + datatype, or a member pointer::

            // Insert the members.
            dtype.insert("a", &S::a);
            dtype.insert("b", &S::b);

            // Or, insert by offset and datatype.
            dtype.insert("c", H5::Datatype::offset(&S::c), 
                H5::Datatype::from_type<float[2][3]>());
            dtype.insert("d", H5::Datatype::offset(&S::d), 
                H5::Datatype::from_type<array<array<long, 3>, 4> >());
        
        The above steps can be confused into one call::

            dtype = H5::Datatype::create_compound("a", &S::a, 
                "b", &S::b, 
                "c", &S::c, 
                "d", &S::d);

        Finally, perform I/O with this new datatype::
            
            S data[10];
            H5::File file("compound_creation.h5");
            file.create_dataset("s-data", dtype, {10}).write(data, dtype);

        The content in the file shown by ``h5dump`` is 

        .. code-block:: text 

            HDF5 "compound_creation.h5" {
            GROUP "/" {
            DATASET "s-data" {
                DATATYPE  H5T_COMPOUND {
                    H5T_STD_I32LE "a";
                    H5T_ARRAY { [3] H5T_IEEE_F64LE } "b";
                    H5T_ARRAY { [2][3] H5T_IEEE_F32LE } "c";
                    H5T_ARRAY { [4][3] H5T_STD_I64LE } "d";
                }
                DATASPACE  SIMPLE { ( 10 ) / ( 10 ) }
                DATA {
                (0): {
                        1411410160,
                        [ 9.88131e-324, 6.95258e-310, 6.93459e-310 ],
                        ...
                    }
                }
            }
            }
            }

    .. function:: Datatype & insert(const string &mem_name, size_t mem_offset, \
        const Datatype &mem_dtype)
        template<typename Record, typename Member> \
        Datatype & insert(const string &mem_name, Member Record::*mem_ptr)

        Inserts a new field into the current compound datatype.
    
        (1): ``insert(name, offset, dtype)`` inserts a member whose name is ``name``, 
        offset is ``offset`` and datatype is ``dtype``.

        (2): ``insert(name, field_ptr)`` inserts a member whose name is name and whose 
        datatype, offset are inferred from member-pointer ``field_ptr``. 
        The member can be any type that is mappable to a HDF5 datatype using 
        :expr:`Datatype::from_type`, (e.g., int, float[3][4][5]). 
    

    .. function:: template<typename Record, typename Member> \
        static constexpr ptrdiff_t offset(Member Record::*mem_ptr) noexcept

        Manually find the offset of a member typed ``Member`` in a structured 
        type ``Record``. 

    .. function:: Datatype & pack()

        Recursively removes the paddings in members to make the datatype 
        more memory efficient.

    .. function:: unsigned nmembers() const
        unsigned member_index(const string &name) const
        string member_name(unsigned idx) const
        class_t member_class(unsigned idx) const
        size_t member_offset(unsigned idx) const
        Datatype member_type(unsigned idx) const

        ``members()`` returns the number of members in the current compound 
        datatype instance.

        ``member_index(name)`` returns the index to the member named name for a 
        compound/enum datatype. 
        
        ``member_name(idx)`` converts the index back to the name. Index can be any 
        number in the range [0, N-1] where N is returned by ``nmembers()``.

        The datatype class, offset, and type can be retrieved by ``member_class()``,
        ``member_offset()`` and ``member_type()``.


    .. function:: Datatype create_array(const Dimensions &dims) const
        template<typename RawArray, DatatypeArch TA = DatatypeArch::NATIVE> \
        static Datatype create_array_for()

        ARRAY datatype creator and visitor.

        ``create_array()`` creates an array datatype with given dimensions ``dims``.

        ``create_array_for<Array>()`` creates an ARRAY datatype for any RawArray 
        protocol type, e.g., ``int a[2][3]``, ``std::array<std::array<float, 3>, 2>``.
    
    .. function:: unsigned array_ndims() const
        Dimensions array_dims() const

        Retrieve the array information.

    .. function:: template<typename NativeT, DatatypeArch TA = DatatypeArch::NATIVE> \
        static const Datatype & from_type();

        Map from a C++ type to its corresponding HDF5 datatype.
        
        ``NativeT``: a DatatypeTraits comformable type, i.e., 
        ``DatatypeTraits<NativeT>::has_h5_datatype == true``.
        
        ``TA``: the architecture specifier.

        User may extend the traits by adding specializations.

    .. function:: template<typename T> \
        static Datatype from_buff(const T &buff)

        Infer the datatype from an C++ object. For example:
        
        - ``buff = double``, the returned datatype is a scalar double type.
        - ``buff = vector<int>``, the returned datatype is a scalar integer type.

    .. function:: _obj_raw_t & obj_raw() noexcept
        const _obj_raw_t & obj_raw() const noexcept

        Return a reference to the intermediate-level HDF5 object.

    .. function::   bool equal( const H5Datatype &dtype ) const


.. _api-io-dtype-predefined:

Predefined Datatypes 
""""""""""""""""""""""""

.. var::    extern const H5Datatype NATIVE_FLOAT_T
            extern const H5Datatype NATIVE_DOUBLE_T
            extern const H5Datatype NATIVE_LDOUBLE_T
            extern const H5Datatype NATIVE_CHAR_T
            extern const H5Datatype NATIVE_SCHAR_T
            extern const H5Datatype NATIVE_SHORT_T
            extern const H5Datatype NATIVE_INT_T
            extern const H5Datatype NATIVE_LONG_T
            extern const H5Datatype NATIVE_LLONG_T
            extern const H5Datatype NATIVE_UCHAR_T
            extern const H5Datatype NATIVE_USHORT_T
            extern const H5Datatype NATIVE_UINT_T
            extern const H5Datatype NATIVE_ULONG_T
            extern const H5Datatype NATIVE_ULLONG_T
    
    The predefined datatypes that correspond to the native numeric types
    in this platform: ``float``, ``double``, ``long double``, ``char``, ``signed char``, 
    ``short``, ``int``, ``long``, ``long long``, ``unsigned char``, 
    ``unsigned short``, ``unsigned int``, ``unsigned long``, and ``unsigned long long``.

.. var::    extern const H5Datatype IEEE_F32LE_T
            extern const H5Datatype IEEE_F64LE_T
            extern const H5Datatype STD_I8LE_T
            extern const H5Datatype STD_I16LE_T
            extern const H5Datatype STD_I32LE_T
            extern const H5Datatype STD_I64LE_T
            extern const H5Datatype STD_U8LE_T
            extern const H5Datatype STD_U16LE_T
            extern const H5Datatype STD_U32LE_T
            extern const H5Datatype STD_U64LE_T
            extern const H5Datatype IEEE_F32BE_T
            extern const H5Datatype IEEE_F64BE_T
            extern const H5Datatype STD_I8BE_T
            extern const H5Datatype STD_I16BE_T
            extern const H5Datatype STD_I32BE_T
            extern const H5Datatype STD_I64BE_T
            extern const H5Datatype STD_U8BE_T
            extern const H5Datatype STD_U16BE_T
            extern const H5Datatype STD_U32BE_T
            extern const H5Datatype STD_U64BE_T   

    The predefined datatypes that correspond to the standard numeric types (i.e., 
    machine-independent). They are usually used as the "file-type" in the 
    construction of a dataset only when you want to get certain storage type 
    in the target file.

    The "LE" version is the little-endian type, and the "BE" version is 
    the big-endian type. "8", "16", "32", "64" are sizes of the datatypes
    in bytes.

.. var::    extern const H5Datatype C_S1_T 

    The elementary type of a C string (i.e., ``const char *``). A fixed-length 
    C string can be obtained by copying this instance and calling 
    :func:`resize <H5Datatype::resize>` to change it to desired length.

Datatype Traits and Converter
""""""""""""""""""""""""""""""""""

.. type:: _DatatypeArch DatatypeArch


``TypeCvt``: interface of the type mapping. Do not add specialization to this 
class template. Instead, define specialization to DatatypeTraits.

.. class:: template<typename NativeT, DatatypeArch TA = DatatypeArch::NATIVE, typename V=void> \
        TypeCvt 

    .. member:: static constexpr bool has_h5_datatype = false

.. class:: template<typename NativeT, DatatypeArch TA> \
    TypeCvt<NativeT, TA, std::enable_if_t<DatatypeTraits<NativeT, TA>::has_h5_datatype> >
    
    .. member:: static constexpr bool has_h5_datatype = true
    
    .. type::   DatatypeTraits<NativeT, TA> traits_t
                NativeT native_t

    .. function:: \
        static string h5_name()
        static string native_name()
        static const Datatype & datatype() noexcept


Dataspace
--------------------

.. class:: Dataspace: public Obj

    Dataspace describe the layout of elements in a dataset or attribute.
 
    Class ``Dataspace`` encapsulates the methods available on dataspaces. 

    **Memory management:** 
    The dataspace class is copyable and movable (both in construction and 
    assignment). The copy, move and destruction are ``noexcept``. The copy operation 
    is shallow - the resulting object always refers to the same HDF5 resource
    as the source object. 
    The move operation sets the move-from object an empty state.

    .. type:: Obj parent_t
        _Dataspace _obj_raw_t
        std::shared_ptr<_obj_raw_t> _obj_ptr_t

    .. type:: _obj_raw_t::class_t class_t
    
    .. member:: \ 
            static constexpr class_t cNULL   =  _obj_raw_t::cNULL
            static constexpr class_t cSIMPLE =  _obj_raw_t::cSIMPLE
            static constexpr class_t cSCALAR =  _obj_raw_t::cSCALAR

        Dataspace type.
        
        
    .. type:: _obj_raw_t::seloper_t seloper_t
    
    .. member:: \
            static constexpr seloper_t selSET     =   _obj_raw_t::selSET
            static constexpr seloper_t selOR      =   _obj_raw_t::selOR
            static constexpr seloper_t selAND     =   _obj_raw_t::selAND
            static constexpr seloper_t selXOR     =   _obj_raw_t::selXOR
            static constexpr seloper_t selNOTB    =   _obj_raw_t::selNOTB
            static constexpr seloper_t selNOTA    =   _obj_raw_t::selNOTA
            static constexpr seloper_t selAPPEND  =   _obj_raw_t::selAPPEND
            static constexpr seloper_t selPREPEND =   _obj_raw_t::selPREPEND

            Dataspace selection operator.

    .. type:: _obj_raw_t::seltype_t seltype_t
    
    .. member:: \
        static constexpr seltype_t selNONE_T       = _obj_raw_t::selNONE_T
        static constexpr seltype_t selALL_T        = _obj_raw_t::selALL_T
        static constexpr seltype_t selPOINTS_T     = _obj_raw_t::selPOINTS_T
        static constexpr seltype_t selHYPERSLABS_T = _obj_raw_t::selHYPERSLABS_T

        Dataspace selection type.

    .. member:: \
        static const Dataspace vALL
        static const Dataspace vNULL
        static const Dataspace vSCALAR

        Predefined dataspace values.

        ``allval`` represents "all" data in a dataset or a memory buffer whose exact meaning depends on the context. 
        
        ``nullval`` represents an empty dataspace.
        
        ``scalarval`` represents the dataspace for a single element, although its datatype 
        may be composite such as ARRAY or COMPOUND.

        
    **Constructors:** Class ``Dataspace`` "inherits" all constructors from its parent class.


    .. function:: \
        Dataspace(std::initializer_list<hsize_t> dims)
        Dataspace(vector<hsize_t> & dims)
        Dataspace(const Dimensions &dims)
        Dataspace(const Dimensions &dims, const Dimensions &maxdims)

        Create a new, SIMPLE class dataspace (i.e., regular array, classed 
        ``cSIMPLE``).

        ``dims``: current dimensions of the array, i.e., number of elements along 
        axes.
        
        ``maxdims```: maximum limit of dimensions. If provided larger than ``dims``,
        the size of the dataset created with such dataspace 
        may be extended later.
        
        The maximal available rank is limited by the HDF5 implementation.

        (1-3): ``maxdims = dims``.
        
        (4): set ``dims`` and ``max_dims`` separately.

    .. function:: \
        ostream & info(ostream &os=cout, int fmt_cntl=0) const
        friend ostream & operator<< (ostream &os, const Dataspace &dspace)

        Print information of the instance to stream ``os``.
        
        ``fmt_cntl``: format controller. ``0`` for a inline, short message and ``1``
        for a verbose one.

        ``operator <<`` is equivalent to ``info()`` with default format controller.

    .. function:: \
        static Dataspace create(class_t type)
        static Dataspace create_null()
        static Dataspace create_scalar()
        static Dataspace create_simple()

        (1): create data space from scratch.
        
        ``type``: cSIMPLE | cNULL | cSCALAR. The dataspace class.

        (2-4): shortcuts of (1).

    .. function:: \
        Dataspace copy() const
        void extent_copy(Dataspace &dst_dspace) const

        ``copy()``: return an exact copy of the dataspace.
        
        ``extent_copy()``: copy the current extent to ``dst_dspace``. May change its 
        dataspace class.

    .. function:: \
        class_t get_type() const

        Get dataspace class.
        

    .. function:: \
        int ndims() const
        Dimensions dims() const
        Dimensions maxdims() const
        hsize_t size() const
        hsize_t maxsize() const

        For SIMPLE dataspace, retrieve its information by the following methods:
        
        ``ndims()``, ``dims()``, ``maxdims()``: return its extents, i.e., number of dimensions 
        (rank), dimensions and maximal dimensions.
        
        ``size()``: number of elements, i.e., product of ``dims()``.
        
        ``maxsize()``: maximal number of elements, i.e., product of ``maxdims()``.

    .. function:: \
        void select_none()
        void select_all()

        Selections that can be applied to SIMPLE dataspace - select none or all.

    .. function:: \
        void select_hyperslab(const Hyperslab &hs)
        void select_hyperslab(const string &op, const Hyperslab &hs)
        void select_hyperslab(seloper_t op, const Hyperslab &hs)
        void select_hyperslab(const string &op,\
            const hsize_t *start, const hsize_t *count, \
            const hsize_t *stride = NULL, const hsize_t *block = NULL)
        void select_hyperslab(seloper_t op, \
            const hsize_t *start, const hsize_t *count, \
            const hsize_t *stride = NULL, const hsize_t *block = NULL)

        Hyperslab selections.

        ``select_hyperslab()`` allows selects a hyperslab of elements starting at the 
        offset ``start`` with ``count`` blocks at each dimension.
        Each block has ``block`` elements and ``stride`` to the next block.
        
        ``op(string)``: specifies how to operator with the current selection. Must be 
        "set" ("=") | "or" ("|") | "and" ("&") | "xor" (^) | "notb" | "nota", 
        
        ``op(seloper_t)``: see the predefined members of selection operations.
        
        (1): "set" selection by a ``Hyperslab`` instance.
        
        (2,3): same with (1), but allow a ``op``.
        
        (4,5): same with (2,3), but using raw buffers for the hyperslab 
        specification. 

        In (1-3), the Hyperslab may have empty stride or block, means ``1`` at all 
        dimensions. None-empty members must have the same length as the rank 
        of the dataspace.
            
        In (4,5), ``stride`` or ``block`` could be `NULL``, means ``1`` at all 
        dimensions.

        In all cases, the sub-setting cannot extend over the dataspace boundary.

    .. function:: \
        Dataspace combine_hyperslab(const string &op, const Hyperslab &hs) const
        Dataspace combine_hyperslab(seloper_t op, const Hyperslab &hs) const

        Combine the current hyperslab selection with ``hs`` using operation ``op``.
        Return the result.

        The ``hs`` argument has the same requirement as in ``select_hyperslab``.
        If the current instance is not a hyperslab, it is freed as ``sel_all()``
        and then combined with ``hs``.

    .. function:: \
        Dataspace combine_select(const string &op, const Dataspace &dspace2) const
        Dataspace combine_select(seloper_t op, const Dataspace &dspace2) const

        Combine the selection of current instance with ``dspace2`` using ``op``.
        Return the result. The extent is taken from the current instance.

    .. function:: \
        Dataspace hyperslabed(const Hyperslab &hs) const

        Return a dataspace whose current extent is copy from the current instance
        with a hyperslab selection specified by ``hs``.

    .. function:: \
        void select_elements(const Points &points)
        void select_elements(const string &op, const Points &points)
        void select_elements(seloper_t op, const Points &points)
        void select_elements(const string &op, size_t n_elems,  \
            const hsize_t coords[])
        void select_elements(seloper_t op, size_t n_elems, const hsize_t coords[])

        Element-base selection - select a list of coordinates in the dataspace.
        
        (1): use a Points instance for selection with ``op == "set"``.

        (2,3): allow an operation with existing selection.
        
        (4,5): the same as (2,3), but using a raw buffer for coordinates.
        
        ``op (string)``: "set" ("=") | "append" ("+") | "prepend". 
        
        ``op (seloper_t)``: using member constants.
        
        ``points``: a 1-D row-major flatten array shaped (n_elems, rank). Rank is 
        not used and is inferred from the dataspace instance.
        
        ``coords``: use raw buffer with same effect as ``points``. The ``rank`` is 
        inferred from the dataspace instance.

    .. function:: \
        Dataspace elemented(const Points &points) const

        Return a dataspace whose current extent is copy from the current instance
        with a element selection specified by ``hs``.
        
    .. function:: \
        hssize_t get_select_npoints() const
        seltype_t get_select_type() const
        bool select_valid() const

        Retrieve selection information from the dataspace.
        
        ``get_select_npoints()``: return the number of elements selected. 

        ``get_select_type()``: return the selection type.
        
        ``select_valid()``: whether or not the selection is valid, i.e., within the
        current dataspace extent.

    .. function:: \
        _obj_raw_t & obj_raw() noexcept
        const _obj_raw_t & obj_raw() const noexcept

        Return the intermediate-level wrapper objects.

Dataset 
---------------------

.. class:: Dataset: public NamedObj

    Datasets are named objects stored in HDF5 files.

    Class ``Dataset`` encapsulates the methods available on datasets. 

    **Memory management:** 
    The dataset class is copyable and movable (both in construction and 
    assignment). The copy, move and destruction are ``noexcept``. The copy operation 
    is shallow - the resulting object always refers to the same HDF5 resource
    as the source object. The move operation sets the move-from object an empty
    state.
    
    .. type:: NamedObj parent_t
        _Dataset _obj_raw_t
        std::shared_ptr<_obj_raw_t> _obj_ptr_t

    
    **Constructors:** Class ``Dataset`` "inherits" all constructors from its parent class.

    .. function:: \
        Dataspace dataspace() const
        Datatype datatype() const

        Return copy of the dataspace or datatype of the dataset.
        
        The datatype is read-only. 
        If the datatype is named, it is opened and returned.

    .. function:: \
        static Proplist create_proplist(const string &cls)
        Proplist proplist(const string &cls) const

        ``create_proplist()``: create a property list for dataset operation.
        
        ``cls``: creation property list of dataset. Can be

        .. table:: 
            :class: tight-table

            ===================================== ==================================
            ``cls``                               Description
            ===================================== ==================================
            ``"c"`` | ``"create"``                dataset creation.
            ``"a"`` | ``"access"``                dataset access.
            ``"x"`` | ``"xfer"`` | ``"transfer"`` data transfer.
            ===================================== ==================================

        ``proplist()``: retrieve the property list of the current dataset. Only 
        dataset creation and access property list can be retrieved. 
        
    .. function:: \
        void write(const void *buff,  \
            const Datatype &memtype,  \
            const Dataspace &memspace  = Dataspace::vALL, \
            const Dataspace &filespace = Dataspace::vALL, \
            const Proplist &xprop      = Proplist::vDFLT)
        template<typename T> \
        void write(const T &buff,  \
            const Dataspace &memspace  = Dataspace::vALL, \
            const Dataspace &filespace = Dataspace::vALL, \
            const Proplist &xprop      = Proplist::vDFLT)
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

        Write data into the dataset.
        
        (1): The most general method that is the direct HDF5 C API counterpart. 
        Write a buffer starting at ``buff`` to the dataset. ``memtype``, 
        ``memspace`` and ``filespace`` specify the memory datatype, memory 
        dataspace and dataset dataspace in the file.

        (2): Write an object. The ``buff`` argument can be
        
        - A raw pointer. The memory datatype is inferred from is point-to type 
          by :expr:`Datatype::from_type`. If ``memspace`` is all-space, the file
          space is used for it.
        - any object that is resolvable by :class:`ConstDatapacket`, including 
          numerical scalars (e.g., ``int``, ``float``), 
          general arrays or numerical types (e.g., ``std::array<int, 4>``, 
          ``std::vector<int>``), or general arrays of raw arrays 
          (e.g., ``std::vector< std::array<int, 4> >``).
          If ``memspace`` is all-space, it is inferred from the object itself.

        Other methods are defined for special cases. All of them use the default 
        property list :expr:`Proplist::vDFLT`.

        (3): Write a scalar. The datatype is inferred from T as if calling 
        :expr:`Datatype::from_type<T>()`.

        (4-11): write a string or a list of strings as fix-length ATOMIC string
        datatype. In the list-of-strings case, the size of the string is taken from 
        the maximum of the strings. 
        
        In all cases, the dataset must be created with consistent dataspace and
        datatype.

    .. function:: \
        template<typename T, \
            std::enable_if_t<!std::is_pointer_v<T>, int > =0> \
            void write_hyperslab(const T &buff, const Hyperslab &hs)
        template<typename T, \
            std::enable_if_t<!std::is_pointer_v<T>, int > =0> \
            void write_elements(const T &buff, const Points &points)
        template<typename T, \
            std::enable_if_t<!std::is_pointer_v<T>, int > =0> \
            void write_element(const T &x, const Dimensions &coord)

        Write data into the selected part of the dataset.
        
        ``write_hyperslab()`` : the filespace is selected by a hyperslab.
        
        ``write_elements()``: the filespace is selected by a list of elements of given
        coordinates.
        
        ``write_element()``: a single element in the filespace is selected.

        These methods are equivalenent to first get the full filespace, 
        call ``select_hyperslab()`` or ``select_elements()`` on it, and then 
        call ``write(buff, Datapace::vALL, filespace)`` (for hyperslab and elements)
        or ``write(buff, Datapace::vSCALAR, filespace)`` (for a single element).
        
    .. function:: \
        void read(void *buff,  \
            const Datatype &memtype, \
            const Dataspace &memspace  = Dataspace::vALL,  \
            const Dataspace &filespace = Dataspace::vALL, \
            const Proplist &xprop      = Proplist::vDFLT) const
        template<typename T> \
        void read(T &&buff, const Dataspace &memspace = Dataspace::vALL,  \
            const Dataspace &filespace = Dataspace::vALL, \
            const Proplist &xprop = Proplist::vDFLT) const
        template<typename T> \
        void read_scalar(T &x)
        void read_str(string &s)
        void read_str(char *s)
        void read_str(vector<string> &ss)
        void read_str(string ss[])
        template<size_t N> \
        void read_str(char ss[][N])

        Read data from the dataset.
        
        (1): The most general method that is the direct HDF5 C API counterpart. 
        Read into buffer starting at ``buff`` from the dataset. ``memtype``, 
        ``memspace`` and ``filespace`` specify the memory datatype, memory 
        dataspace and dataset dataspace in the file.

        (2): Read into an object. The ``buff`` argument can be
        
        - A raw pointer. The memory datatype is inferred from is point-to type 
          by :expr:`Datatype::from_type`. If ``memspace`` is all-space, the file
          space is used for it.
        - ``std::vector<T, Alloc>``. ``T`` must be valid argument to 
          :expr:`Datatype::from_type` (e.g., numerical scalar or raw-array).
          If ``memspace`` is all-space, it is taken from filespace. 
          The vector is always resized to exactly hold the selected elements. 
          If the resize operation cannot exactly fits the need, an ``ErrLogic``
          is thrown.
        - any object that is resolvable by :class:`ConstDatapacket`, including 
          numerical scalars (e.g., ``int``, ``float``), 
          general arrays or numerical types (e.g., ``std::array<int, 4>``).
          If ``memspace`` is all-space, it is inferred from the object itself.

        Other methods are defined for special cases. All of them use the default 
        property list :expr:`Proplist::vDFLT`.

        (3): Read a scalar. The datatype is inferred from ``T`` as if calling 
        :expr:`Datatype::from_type<T>()`.

        (4-8): Read a string or a list of strings as fix-length ATOMIC string
        datatype. In the list-of-strings case, the size of the string is taken from 
        the maximum of the strings. 
        The ``std::string`` or ``std::vector`` is auto-resized to fit the data. 
        Otherwise (e.g., ``char *``), the buffer must be large enough to hold
        the string that is read.
        
        In all cases, the dataset must has consistent dataspace and datatype.

    .. function:: \
        template<typename T, typename A>  \
        void read_hyperslab(vector<T, A> &v, const Hyperslab &hs) const
        template<typename T, typename A> \
        void read_elements(vector<T, A> &v, const Points &points) const
        template<typename T> \
        void read_element(T &x, const Dimensions &coord) const

        Read data from the selected part of the dataset.
        
        ``read_hyperslab()``: the filespace is selected by a hyperslab.
        
        ``read_elements()``: the filespace is selected by a list of elements of given
        coordinates.

        ``read_element()``: a single element in the filespace is selected.

        These methods are equivalenent to first get the full filespace, 
        call ``select_hyperslab()`` or ``select_elements()`` on it, and then 
        call ``read(v, Datapace::vALL, filespace)`` (for hyperslab and elements)
        or ``read(x, Datapace::vSCALAR, filespace)`` (for a single element).

    .. function:: \
        _obj_raw_t & obj_raw() noexcept
        const _obj_raw_t & obj_raw() const noexcept

        Return a reference to the intermediate-level HDF5 object.

DatasetManager
----------------

.. class:: DatasetManager

    Tha manager provides commonly used shortcuts for dataset creation and 
    access.

    .. type:: Group refobj_t

    .. function:: \
        DatasetManager() noexcept
        explicit DatasetManager(refobj_t obj) noexcept

        Constructors.
        
        (1): refers to an empty dataset object. I/O may not be performed in this 
        case.
        
        (2): refers to given dataset.

    .. function:: \
        ~DatasetManager() noexcept = default
        DatasetManager(const DatasetManager &) noexcept = default
        DatasetManager(DatasetManager &&) noexcept = default
        DatasetManager & operator=(const DatasetManager &) noexcept = default
        DatasetManager & operator=(DatasetManager &&) noexcept = default

        The dataset manager is copyable and movable. The copy is shallow with 
        resulting instance referring to the same dataset.
        The move is destroyable, leaving the move-from instance an empty state.

    .. function:: \
        refobj_t refobj() const

        Retrieve the referred object, i.e., the dataset object.

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
        void put_str(const string &name, const T &x)
        template<typename T> \
        void put_slab(const string &name, const T &x, const Hyperslab &filespace)
        template<typename T> \
        void put_ats(const string &name, const T &x, const Points &coords)
        template<typename T> \
        void put_at(const string &name, const T &x, const Dimensions &coord)

        Put operations - write data into dataset of given ``name``.
        
        (1): Create a dataset with given ``name`` whose datatype are dataspace are 
        determined from the data ``x`` and write ``x`` into it.
        If the dataset already exists, open it and update its contents to ``x``
        where ``x`` must have consistent datatype and size.
        ``x`` may be anything that can be used in :expr:`Group::create_dataset_for`
        except the string overloads.

        (2): the same as (1), but using string data ``x`` acceptable by 
        :expr`Group::create_dataset_for_str`. Examples include single strings
        (``std::string`` or C-style string ``const char *`` ), vector of 
        strings (``std::vector<std::string>``) or array of strings 
        (``std::string[N]``), 2-D character array (``const char [N_STR][N]``,
        resolved as ``N_STR`` strings with fixed length ``N``).
        The string dataset is created as scalar or 1-D dataspace, with fixed-length 
        ATOMIC STRING datatype.

        (3): Put the data ``x`` into an existsing dataset of given ``name``. The
        filespace of the dataset is selected by a hyperslab ``filespace``.

        (4): Put a list of data elements ``x`` at the given coordinates in an 
        existing dataset of given ``name``.

        (5): Put a single data element ``x`` at the given coordinate in an 
        existing dataset of given ``name``.

    .. function:: \
        template<typename T> \
        void get(const string &name, T &x)
        template<typename T> \
        void get_str(const string &name, T &x)
        template<typename T> \
        void slab(const string &name, T &x, const Hyperslab &filespace)
        template<typename T> \
        void ats(const string &name, T &x, const Points &coords)
        template<typename T> \
        void at(const string &name, T &x, const Dimensions &coord)
        template<typename T> \
        T get(const string &name)
        template<typename T> \
        T get_str(const string &name)
        template<typename T> \
        T slab(const string &name, const Hyperslab &filespace)
        template<typename T> \
        T ats(const string &name, const Points &coords)
        template<typename T> \
        T at(const string &name, const Dimensions &coord)

        Get operations: read data from existing dataset of given ``name``.
        
        In all case, the dataset must have consistent datatype and dataspace with
        the argument ``x``.

        (1-5): are similar to ``put`` but perform reading here.
        
        (6-10): are similar to (1-5), respectively, but return the data which are
        read (i.e., the object typed ``T`` gets default-initialized, be read into, 
        and returned).

    .. function:: \
        bool exists(const string &name)

        Find whether or not a dataset of given ``name`` exists in the parent 
        object.

    .. function:: \
        Dataset open(const string &name)

        Open an existing dataset named ``name``.



        


    


