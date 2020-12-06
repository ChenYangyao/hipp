Datatype, Dataspace and Dataset
==================================

The following classes are all defined within namespace ``HIPP::IO``.

.. namespace:: HIPP::IO


Class H5Datatype
-----------------

.. class:: H5Datatype
    
    In HDF5 format, every dataset has a datatype which describes the type (i.e., interpretation of each byte) 
    of the element in the dataset. 
    
    HDF5 pre-defines several sets of datatypes (standard or platform-dependent). The predefined datatypes 
    are listed below in :ref:`Predefined Datatypes <api-io-dtype-predefined>`.

    User may define new datatypes, derived from predefined datatypes or other 
    user-defined datatypes.

    ``H5Datatype`` instance can be copy-constructed, copy-assigned, move-constructed and move-assigned.
    The copy, move operations and destructor are all noexcept. The copy operations are shallow-copy, i.e., 
    instances of source and target refer to the same underlying datatype object (like a ``shared_ptr``).

    .. _api-io-dtype-class-enum:

    .. enum:: class_t 

    .. enumerator:: class_t::COMPOUND_C
                    class_t::OPAQUE_C
                    class_t::ENUM_C
                    class_t::STRING_C

        Enumerators of datatype classes. They can be passed into method :func:`create` 
        (to create new user-defined datatypes) or returned by :func:`member_class` (to detect classes 
        of existing datatypes).

    .. function::   bool equal( const H5Datatype &dtype ) const
                    size_t size() const
        
        Datatype information query functions.

        ``equal(dtype)`` tests whether the current instance is the same datatype 
        as dtype.

        ``size()`` returns the size in bytes of the current instance.

    .. H5Datatype copy() const

        Copy the current datatype.

    .. void resize( size_t size )

        Reset the size of the current instance. 
        
        For example, a fixed-length C-string type can be constructed by copy :var:`C_S1_T`
        and resize it to desired length. 

    .. function::   static H5Datatype create(class_t c, size_t size)
                    template<typename record_t, typename field_t, typename ...Args>\
                    static H5Datatype create_compound(\
                        const string &field_name, field_t record_t::*field_ptr,\
                        Args &&...args)
                    H5Datatype & insert(const string &name, size_t offset, const H5Datatype &dtype)
                    template<typename record_t, typename field_t>\
                    H5Datatype & insert(const string &name, field_t record_t::*field_ptr)
                    H5Datatype & pack()
                    unsigned nmembers() const
                    unsigned member_index(const string &name) const
                    class_t member_class(unsigned idx) const
                    size_t member_offset(unsigned idx) const
                    H5Datatype member_type(unsigned idx) const
                    string member_name(unsigned idx) const
                    template<typename record_t, typename field_t>\
                    static constexpr size_t offset(field_t record_t::*field_ptr) noexcept

        Compound datatype creation and information-query methods.

        ``create()`` creates a new datatype with datatype-class ``c`` and size in bytes ``size``.
        Valid datatype-classes are defined in :ref:`Dtatype Classes <api-io-dtype-class-enum>`.
        
        ``create_compound(name1, ptr1, ...)`` directly creates a new compound datatype (usually corresponding to 
        a C++ struct/class). For each member of the datatype, you pass two arguments to describe it:
        the name and a member-pointer to it. The member can be one of the numeric 
        :ref:`Predefined Datatypes <api-io-dtype-predefined>` 
        or its raw array (e.g., ``int``, ``float[3][4][5]``). The library will infer its (native) datatype, offset and 
        size from the member-pointer.

        ``insert()`` inserts a member into the compound datatype. There are two overloads:
        
        -   ``insert(name, offset, dtype)`` inserts a member whose name is ``name``, offset is ``offset``
            and datatype is ``dtype`` (which may be a predefined or user-defined datatype).
        -   ``insert(name, field_ptr)`` inserts a member whose name is ``name`` and whose datatype, 
            offset and size (if it is a raw array) are infered from member-pointer ``field_ptr``.
            This is valid for any of the numeric :ref:`Predefined Datatypes <api-io-dtype-predefined>`
            or its raw array.

        -   ``pack()`` recursively removes the paddings in members to make the datatype more memory efficient.

        ``nmembers()`` returns the number of members in the current compound datatype instance.

        ``member_index(name)`` returns the index to the member named ``name`` for a compound/enum datatype. 
        ``member_name(idx)`` converts the index back to the name. Index can be any number in the range ``[0, N-1]`` where 
        ``N`` is returned by ``nmembers()``.

        ``member_class(idx)``, ``member_offset(idx)``, ``member_type(idx)``.


        To create a compound datatype of, e.g. a structured C++ type ``T``, call 
        ``H5Datatype::create(H5Datatype::COMPOUND_C, sizeof(T))`` to get a new datatype instance, 
        and call ``insert()`` to add information of each field of ``T``. 
        
        For example, a dark matter halo in cosmological simulation can be described by 
        the following C++ type::

            // for storing the properties of a dark matter halo
            class DarkMatterHalo {
            public:
                long long id;
                double position[3];
                float tidal_tensor[3][3];
                double radius;
            };

        To create a corresponding HDF5 datatype for I/O, you write::

            /* Create compound datatype for DarkMatterHalo. */
            auto dtype = H5Datatype::create(
                H5Datatype::COMPOUND_C, sizeof(DarkMatterHalo));
            dtype.insert("ID",           H5Datatype::offset(&DarkMatterHalo::id), NATIVE_LLONG_T)
                 .insert("Position",     &DarkMatterHalo::position)
                 .insert("Tidal Tensor", &DarkMatterHalo::tidal_tensor)
                 .insert("Radius",       &DarkMatterHalo::radius);
        
        Note that you can insert each field by ``(name, offset, datatype)`` (like "ID" above)
        or simply by ``(name, field_ptr)`` where ``field_ptr`` is the pointer to 
        that member (like "Position", "Tidal Tensor", "Radius" above). 

        If your C++ structure contains only numeric types (such as ``DarkMatterHalo`` here),
        it is easier to create the compound datatype directly using a single function call::

            /* Another way to create a compound datatype. */
            auto dtype = H5Datatype::create_compound(
                "ID",           &DarkMatterHalo::id,
                "Position",     &DarkMatterHalo::position,
                "Tidal Tensor", &DarkMatterHalo::tidal_tensor,
                "Radius",       &DarkMatterHalo::radius);

        Now you perform I/O using the new datatype::

            /* Write halo instances into a new file */
            vector<DarkMatterHalo> halos(10), halos_in(10);
            H5File file("halos.h5", "w");
            file.create_dataset("Halos", dtype, {10}).write(halos.data(), dtype);

            /* Load it back */
            file.open_dataset("Halos").read(halos_in.data(), dtype);
    
        Using ``h5dump halos.h5`` you see the output

        .. code-block:: text

            HDF5 "halos.h5" {
            GROUP "/" {
            DATASET "Halos" {
                DATATYPE  H5T_COMPOUND {
                    H5T_STD_I64LE "ID";
                    H5T_ARRAY { [3] H5T_IEEE_F64LE } "Position";
                    H5T_ARRAY { [3][3] H5T_IEEE_F32LE } "Tidal Tensor";
                    H5T_IEEE_F64LE "Radius";
                }
                DATASPACE  SIMPLE { ( 10 ) / ( 10 ) }
                DATA {
                    (0): { 0,[ 0, 0, 0 ], [ 0, 0, 0, 0, 0, 0, 0, 0, 0 ], 0}, 
                    ....
                }
            }}}

    .. function::   H5Datatype create_array(const vector<hsize_t> &dims) const
                    template<typename raw_array_t> \
                    static H5Datatype create_array()
                    unsigned array_ndims() const
                    vector<hsize_t> array_dims() const

        Array datatype creation and information-query functions.

.. _api-io-dtype-predefined:

Predefined datatypes 
""""""""""""""""""""""""

.. var::    extern const H5Datatype NATIVE_CHAR_T
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
            extern const H5Datatype NATIVE_FLOAT_T
            extern const H5Datatype NATIVE_DOUBLE_T
    
    The predefined datatypes that correspond to the native numeric types
    in this platform: ``char``, ``signed char``, 
    ``short``, ``int``, ``long``, ``long long``, ``unsigned char``, 
    ``unsigned short``, ``unsigned int``, ``unsigned long``, ``unsigned long long``
    ``float`` and ``double``.

.. var::    extern const H5Datatype STD_I8LE_T
            extern const H5Datatype STD_I16LE_T
            extern const H5Datatype STD_I32LE_T
            extern const H5Datatype STD_I64LE_T
            extern const H5Datatype STD_U8LE_T
            extern const H5Datatype STD_U16LE_T
            extern const H5Datatype STD_U32LE_T
            extern const H5Datatype STD_U64LE_T
            extern const H5Datatype IEEE_F32LE_T
            extern const H5Datatype IEEE_F64LE_T
            extern const H5Datatype STD_I8BE_T
            extern const H5Datatype STD_I16BE_T
            extern const H5Datatype STD_I32BE_T
            extern const H5Datatype STD_I64BE_T
            extern const H5Datatype STD_U8BE_T
            extern const H5Datatype STD_U16BE_T
            extern const H5Datatype STD_U32BE_T
            extern const H5Datatype STD_U64BE_T
            extern const H5Datatype IEEE_F32BE_T
            extern const H5Datatype IEEE_F64BE_T

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

Class H5Dataspace
--------------------

.. class::   H5Dataspace

    In HDF5 format, every dataset has a dataspace which describe its shape. In the I/O process, 
    a dataspace instance also defines which part of the data in the memory or in the file is
    involved.

    ``H5Dataspace`` instance can be copy-constructed, copy-assigned, move-constructed and move-assigned.
    The copy, move operations and destructor are all noexcept. The copy operations are shallow-copy, i.e., 
    instances of source and target refer to the same underlying dataspace object (like a ``shared_ptr``).

    .. _api-io-dspace-class-enum:

    .. enum:: class_t 

    .. enumerator:: class_t::NULL_C
                    class_t::SIMPLE_C
                    class_t::SCALAR_C

        Enumerators of dataspace classes. They can be passed into :func:`create` method
        to create new dataspaces.

    .. var::    static const H5Dataspace allval
                static const H5Dataspace nullval
                static const H5Dataspace scalarval

        Predefined dataspaces. 
        
        -   ``allval`` represents all data in a dataset or a memory buffer,
            whose exact meaning depends on the context. 
        -  ``nullval`` represents an empty dataspace.
        -   ``scalarval`` represents the dataspace for a single element, although the its datatype 
            may be complex.

    .. function::   H5Dataspace( const vector<hsize_t> &dims )
                    H5Dataspace( const vector<hsize_t> &dims, const vector<hsize_t> &maxdims )

        Constructors - create simple dataspaces (i.e., regular array, dataspace class = ``SIMPLE_C``). 
        ``dims`` specifies its shape (i.e., number of element at each dimension). If a ``maxdims``
        is also provided, the maximal number of element at each dimension may be larger than used, 
        which means you may extend its shape later.
 
    .. function::   static H5Dataspace create(class_t type)
                    static H5Dataspace create_null()
                    static H5Dataspace create_scalar()
                    static H5Dataspace create_simple()

        Create a new dataspace instance. ``type`` may be one of the :ref:`Dataspace Classes <api-io-dspace-class-enum>`.
        For convenience, we also provide three functions to create null, scalar and simple dataspace, respectively.

    .. function::   int ndims( )const
                    vector<hsize_t> dims( )const
                    vector<hsize_t> maxdims( )const
                    hsize_t size() const

        Query the information of current dataspace instance.

        ``ndims()`` returns the number of dimensions (i.e., rank). ``dims()`` returns the number 
        of elements in each dimension. 
        ``maxdims()`` returns the maximal number of elements in 
        each dimension. 
        ``size()`` returns the total number of elements (i.e., the product of all 
        values returned by ``dims()``).
    
    .. function::   void select_hyperslab( const vector<hsize_t> &start, const vector<hsize_t> &count )
                    void select_hyperslab( const string &op, const hsize_t *start, const hsize_t *count, \
                            const hsize_t *stride = NULL, const hsize_t *block = NULL );
                            hssize_t get_select_npoint()const

        Select a hyperslab in the current dataspace. 
        ``op`` can be either "set", "or" ("|"), "and" ("&"), "xor" ("^"), "notb", "nota". 
        
        In the first overload, op = "set", ``stride = 1`` in all dimension, ``block=1`` in all dimension.

        In the second overload, setting ``stride`` or ``block`` to ``NULL`` means "1" in all dimensions.


Class H5Dataset 
---------------------

.. class::  H5Dataset

    The API for HDF5 dataset.

    ``H5Dataset`` instance can be copy-constructed, copy-assigned, move-constructed and move-assigned.
    The copy, move operations and destructor are all noexcept. The copy operations are shallow-copy, i.e., 
    instances of source and target refer to the same underlying dataset object (like a ``shared_ptr``).

    .. function::   H5Dataspace dataspace()
                    const H5Dataspace dataspace() const
                    H5Datatype datatype()
                    const H5Datatype datatype() const

        Retrive the information (i.e., dataspace and datatype) of the dataset instance. 
    
    .. function::   template<typename T>\
                        H5Attr create_attr(\
                            const string &name, const vector<hsize_t> &dims, \
                            const string &flag="trunc")
                    H5Attr create_attr(\
                        const string &name, const H5Datatype &dtype, \
                        const vector<hsize_t> &dims, const string &flag="trunc")
                    template<typename T>\
                    H5Attr create_attr_scalar(\
                        const string &name, const string &flag="trunc")
                    H5Attr create_attr_str(\
                        const string &name, size_t len, const string &flag="trunc")
        
        Create a new attribute (or open an existing attribute) under the current dataset.

        The template parameter and argument list are the same with :func:`H5File::create_dataset` and its variants. 
        The difference is that you cannot specify any property list.
        

    .. function::   H5Attr open_attr(const string &name)
                    bool attr_exists(const string &name) const

        Opens an existing attribute of name ``name``. If not existing, throw an error :class:`ErrH5`.

        ``attr_exists()`` checks whether an attribute has been existed.
    
    .. function::   template<typename T, typename A>\
                        void write( const vector<T, A> &buff, const H5Dataspace &memspace = H5Dataspace::allval, \
                        const H5Dataspace &filespace=H5Dataspace::allval, \
                        const H5Proplist &xprop=H5Proplist::defaultval )
                    template<typename T>\
                        void write( const T *buff, \
                            const H5Dataspace &memspace = H5Dataspace::allval, \
                            const H5Dataspace &filespace=H5Dataspace::allval,\
                            const H5Proplist &xprop=H5Proplist::defaultval )
                    void write( const string &buff, \
                        const H5Proplist &xprop=H5Proplist::defaultval )
                    template<typename T, typename A>\
                        void write( const vector<T, A> &buff, \
                            const H5Datatype &memtype,\
                            const H5Dataspace &memspace = H5Dataspace::allval, \
                            const H5Dataspace &filespace=H5Dataspace::allval, \
                            const H5Proplist &xprop=H5Proplist::defaultval )
                    template<typename T>\
                        void write( const T *buff, \
                            const H5Datatype &memtype,\
                            const H5Dataspace &memspace = H5Dataspace::allval, \
                            const H5Dataspace &filespace=H5Dataspace::allval, \
                            const H5Proplist &xprop=H5Proplist::defaultval )
        
        Write data in ``buff`` into the dataset. Type and number of elements in the `buff` must be compatible with the 
        dataset. Five overloads are provided for: 

        (1) ``const vector<T, A> & buff``: write a vector of elements of type T. T must be a numeric 
            :ref:`Predefined Datatypes <api-io-dtype-predefined>` (i.e., int, float) or ``std::string``.
            For the numeric types, the total number of elements in the vector must be compatible with the dataset' size 
            (i.e., the product of actual dims). 
            For the string, the total number of strings and the maximal length of these strings must br compatible with 
            the dataset's dims.
        (2) ``const T *buff``: same as the vector version (1), but use data in the raw buffer.
        (3) ``const string &``: write a single string.
        (4) ``const vector<T, A> &buff, const H5Datatype &memtype``: same as the vector version (1), but now T could
            be any type whose datatype is described by ``memtype```.
        (5) ``const T *buff, const H5Datatype &memtype``: same as (4), but using a raw buffer.

    .. function::   template<typename T, typename A>\
                        void read( vector<T, A> &buff,\
                            const H5Dataspace &memspace = H5Dataspace::allval, \
                            const H5Dataspace &filespace=H5Dataspace::allval,\
                            const H5Proplist &xprop=H5Proplist::defaultval ) const
                    template<typename T>\
                        void read( T *buff,\
                            const H5Dataspace &memspace = H5Dataspace::allval, \
                            const H5Dataspace &filespace=H5Dataspace::allval,\
                            const H5Proplist &xprop=H5Proplist::defaultval ) const
                    void read( string &buff, \
                        const H5Proplist &xprop=H5Proplist::defaultval ) const
                    template<typename T, typename A>\
                        void read( vector<T, A> &buff, const H5Datatype &memtype,\
                            const H5Dataspace &memspace = H5Dataspace::allval, \
                            const H5Dataspace &filespace=H5Dataspace::allval,\
                            const H5Proplist &xprop=H5Proplist::defaultval ) const
                    template<typename T>\
                        void read( T *buff, const H5Datatype &memtype,\
                            const H5Dataspace &memspace = H5Dataspace::allval, \
                            const H5Dataspace &filespace=H5Dataspace::allval,\
                            const H5Proplist &xprop=H5Proplist::defaultval ) const

        Read data in the dataset instance into ``buff``. This the inverse of the :func:`write` method, so we still 
        provide five overloads. The detailed requirement for each overload is the same as :func:`write`.

        The first and the third overloads automatically resize the buffer. In all other cases the buffers must have 
        correct shapes.

    .. function::   static H5Proplist create_proplist(const string &cls)
                    H5Proplist proplist(const string &cls) const

        Dataset property list manipulation methods.

        ``create_proplist(cls)`` creates a property list with given class ``cls``. Possible values for ``cls`` include

        - "c" or "create": properties for dataset creation.
        - "a" or "access": properties for dataset access.
        - "x" or "xfer" or "transfer": properties for dataset transfer.

        ``proplist()`` retrives the property list of current dataset instance. ``cls`` can be either "c" (or "create") or "a" (or "access").




    


