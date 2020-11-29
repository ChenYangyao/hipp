Dataset, Datatype and Dataspace
==================================

The following classes are all defined within namespace ``HIPP::IO``.

.. namespace:: HIPP::IO 

Class H5Datatype
-----------------

.. class:: H5Datatype
    
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
                    unsigned nmember_index(const string &name) const
                    class_t member_class(unsigned idx) const
                    size_t member_offset(unsigned idx) const
                    H5Datatype member_type(unsigned idx) const
                    string member_name(unsigned idx) const
                    template<typename record_t, typename field_t>\
                    static constexpr size_t offset(field_t record_t::*field_ptr) noexcept

        Compound datatype creation and information-query methods.

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
        
        Note that you can insert each field by ``(name, offset, datatype)`` (like "ID")
        or simply by ``(name, field_ptr)`` where ``field_ptr`` is the pointer to 
        that member. The later one applies for all numeric types and their raw arrays 
        (e.g., ``int``, ``float[3][4][5]``, ...).

        If your C++ structure contains only numeric types (such as ``DarkMatterHalo`` here),
        it is easier to create the compound datatype directly using a single function call::

            /* another way to create a compound datatype */
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

