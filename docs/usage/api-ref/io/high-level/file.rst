File and Group
====================================


The following classes are all defined within namespace ``HIPP::IO``.

.. namespace:: HIPP::IO


Class H5File
-----------------

.. class:: H5File


    The API for HDF5 file.

    ``H5File`` instance can be copy-constructed, copy-assigned, move-constructed and move-assigned.
    The copy, move operations and destructor are all noexcept. The copy operations are shallow-copy, i.e., 
    instances of source and target refer to the same underlying file object (like a ``shared_ptr``).


    .. function::   H5File( const string &name, const string &flag, \
                        const H5Proplist &cporp = H5Proplist::defaultval, \ 
                        const H5Proplist &aprop = H5Proplist::defaultval)

        Constructor - create a new file or open an existing file named ``name``.
        
        ``flag`` controls the the open/creation and Read/Write mode. It can be one of the following values

        -   "w": create a new file (truncate if existing) as R/W mode. 
        -   "r": open an existing file as R mode. 
        -   "a": open an existing file as R/W mode. 
        -   "ac" or "ca": open an existing file (create if not existing) as R/W mode.

        ``cprop`` and ``aprop`` specify the creation and access properties, respectively.

    .. function::   template<typename T>\
                    H5Dataset create_dataset( const string &name, const vector<hsize_t> &dims, \
                        const string &flag="trunc", \
                        const H5Proplist &lcprop = H5Proplist::defaultval,\
                        const H5Proplist &cprop = H5Proplist::defaultval,\
                        const H5Proplist &aprop = H5Proplist::defaultval )
                    H5Dataset create_dataset(const string &name, const H5Datatype &dtype, \
                        const vector<hsize_t> &dims, const string &flag="trunc", \
                        const H5Proplist &lcprop = H5Proplist::defaultval,\
                        const H5Proplist &cprop = H5Proplist::defaultval,\
                        const H5Proplist &aprop = H5Proplist::defaultval)
                    template<typename T>\
                    H5Dataset create_dataset_scalar( const string &name,\
                        const string &flag="trunc", \
                        const H5Proplist &lcprop = H5Proplist::defaultval,\
                        const H5Proplist &cprop = H5Proplist::defaultval,\
                        const H5Proplist &aprop = H5Proplist::defaultval )
                    H5Dataset create_dataset_str( const string &name, size_t len,\
                        const string &flag="trunc", \
                        const H5Proplist &lcprop = H5Proplist::defaultval,\
                        const H5Proplist &cprop = H5Proplist::defaultval,\
                        const H5Proplist &aprop = H5Proplist::defaultval )
        
        Create a new dataset (or open an existing dataset) under the root group of the file instance.

        ``name`` specifies the name of the dataset to open/create. If the dataset 
        does not exist, it is created. Otherwise, if ``flag=="trunc"``, the dataset is opened, or if ``flag=="excl"``,
        an exception :class:`ErrH5` is thrown.
        
        ``lcprop`` specifies the link-creation properties, ``cprop`` specifies the dataset creation properties, and ``aprop``
        specifies the dataset access properties.

        Four methods are:

        (1) ``create_dataset<T>()`` creates an array-like dataset with element type ``T``, array shape ``dims``. Type ``T``
            can be either numeric :ref:`Predefined Datatypes <api-io-dtype-predefined>` (e.g., int, float), 
            or ``std::string``. For the numeric types, ``dims`` is the array shape. For ``std::string``, ``dims`` must be 
            ``{ num_of_str, max_str_size}``, where ``max_str_size`` must contain the space of the terminate null character. 
            It is best to calculate dims use :func:`H5TypeStr::shape`
        (2) ``create_dataset()`` creates and array-like dataset with element type ``dtype``, array shape ``dims``.
        (3) ``create_dataset_scalar()`` creates a scalar dataset. 
        (4) ``create_dataset_str()`` creates a dataset for a single string.

    .. function::   H5Dataset open_dataset( const string &name,\
                        const H5Proplist &aprop = H5Proplist::defaultval )
                    bool dataset_exists( const string &name ) const

        ``open_dataset(name)`` opens an existing dataset named ``name``. If not existing, throw an exception :class:`ErrH5`.
        
        To check whether a dataset exists, use ``dataset_exists(name)``.

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

        Create a new attribute (or open an existing attribute) under the root group of this file instance.

        The template parameter and argument list is the same with :func:`create_dataset()` and its variants. The difference
        is that you cannot specify any property list.
        
    .. function::   H5Attr open_attr(const string &name)
                    bool attr_exists(const string &name) const

        ``open_attr(name)`` opens an existing attribute named ``name``. If not existing, throw an exception :class:`ErrH5`.

        To check whether an attribute exists, use ``attr_exists(name)``.

    .. function::       H5Group create_group( const string &name )
                        H5Group try_create_group( const string &name )
                        H5Group open_group( const string &name ) 
                        bool group_exists( const string &name ) const

        Data group manipulation methods.

        ``create_group(name)`` creates a group named ``name``. If it has already existed, throw an exception :class:`ErrH5`.

        ``try_create_group(name)`` is similar, but open the group if it has already existed.

        ``open_group(name)`` opens an existing group. If it is not existed, throw an exception :class:`ErrH5`.

        ``group_exists(name)`` check whether a group exists.

    .. function::   static H5Proplist create_proplist(const string &cls = "c")

        Create a file-related property list of given class ``cls``. Possible values are:

        - "c" or "create": properties for file creation. 
        - "a" or "access": properties for file access.
        - "m" or "mount": properties for file mounting.