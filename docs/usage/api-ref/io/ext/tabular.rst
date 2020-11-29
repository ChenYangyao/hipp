Tabular-Data IO
========================

The following classes are all defined within namespace ``HIPP::IO``.

.. namespace:: HIPP::IO 


Class H5XTable
------------------------


.. class:: template<typename record_t> H5XTable

    ``H5XTable`` manipulates the IO of a table, i.e., array of ``record_t``, where 
    ``record_t`` is any C++ (simple) struct/class. Examples of 
    allowed ``record_t`` include ::

        // for bookkeeping the information of a person
        struct Person {
            int age; 
            int gender;
            char name[32];
        };

        // for storing the properties of a dark matter halo
        class DarkMatterHalo {
        public:
            long long id;
            double position[3];
            float tidal_tensor[3][3];
            double radius;
        };

    Such structural data types exist almost everywhere in C++ programming. The 
    instances of such a data type are usually organized as a STL ``vector`` or 
    in raw buffer allocated by ``std::malloc()`` or ``new`` operator.
    In such cases, ``H5XTable`` provides easy-use methods to I/O them, either
    as separate HDF5 datasets for each field or as a single dataset with compound
    datatype.  

    ``record_t`` cannot have a virtual method. It may have private attributes 
    (can be touched by ``H5XTable`` only if defined within the class namespace).

    ``H5XTable`` cannot be copied or copy-constructed, but it can be moved 
    or move constructed. The destructor and move operations are noexcept.

    .. type:: vector<record_t> table_t

    .. function:: template<typename ...Args> H5XTable( Args &&...args )
        
        Initialize the table manipulator. For each attibute of ``record_t``,
        you simply pass as arguments a string for its name and a pointer to that 
        attribute.

        In the I/O process using method :func:`read`/:func:`write`, 
        each attribute is loaded from/stored into a HDF5 dataset. 
        The name of the dataset is specified by string you pass.

        In the I/O process using method :func:`read_records`/:func:`write_records`,
        the array of ``record_t`` is loaded/stored into a single HDF5 dataset
        with corresponding compound datatype.
        The name of each field in compound datatype is specified by the string 
        you pass.

        The pointer to attribute provides type and size information that is 
        used internally by the library.

        After construction, you may add/remove attributes by 
        :func:`add_field`/:func:`remove_field`.

        Examples of table manipulators of ``Person`` and ``DarkMatterHalo`` 
        defined above are ::

            H5XTable<Person> person_manip(
                "age", &Person::age,
                "gender", &Person::gender,
                "name", &Person::name);

            H5XTable<DarkMatterHalo> halo_manip (
                "position", &DarkMatterHalo::position,
                "Index", &DarkMatterHalo::id, 
                "tidal_tensor", &DarkMatterHalo::tidal_tensor);

        The order of attributes passed may be different from that defined in the 
        struct/class (such as the ``DarkMatterHalo::id`` and ``DarkMatterHalo::position``). 
        It is not necessary to pass all attributes (the ignored 
        attributes are not I/O in the call of read/write, such as the 
        ``DarkMatterHalo::radius``).

    
    .. function::   template<typename field_t> H5XTable & add_field(const string &name, field_t record_t::*p)
                    bool remove_field(const string &name) 
                    bool has_field(const string &name)
                    size_t n_fields() const noexcept
                    bool empty() const noexcept

            ``add_field(name, p)`` adds an attribute, pointed by ``p``, named ``name``, to the table manipulator.
            
            ``remove_field(name)`` removes an attribute named ``name``.
            
            ``has_field(name)`` test whether an attribute of name ``name`` has been in the table manipulator.
            
            ``n_fields()`` returns the total number of attributes.
            
            ``empty()`` checks whether there is no attribute in the manipulator.

    .. function::   void write(const table_t &tbl, H5Group dgrp)
                    void write(const table_t &tbl, H5File file)
                    void write(const table_t &tbl, const string &file_name, const string flag="w")
                    void write(const record_t *buff, size_t n, H5Group dgrp)

        Write a table (array of ``record_t``) into the target group ``dgrp`` as separate 
        datasets. Four overloads are:
        
        (1) Specify the table ``tbl`` and target group ``dgrp`` (in file).
        (2) Is similar but accepts a :class:`H5File` - the table is written into 
            its root group. 
        (3) Is similar but opens/creates a HDF5 file named ``file_name`` with flag ``flag``, 
            and write table into its root group.
        (4) Is similar but writes data from raw buffer starting at ``buff`` and 
            consisting of ``n`` values typed ``record_t``.
        
        For example, if you have a ``vector`` of ``DarkMatterHalo`` and output it 
        with ``halo_manip``, say::

            vector<DarkMatterHalo> halos(8);
            halo_manip.write(halos, "halos.h5");

        A file named "halos.h5" in the current working directory is created 
        (and truncated if existing). Three datasets are created under that file: 
        float64 (named "position", shaped 8x3), int64 (named "Index", shaped 8),
        float32 (named "tidal_tensor", shaped 8x3x3).

        If you write into an existing group and the HDF5 dataset with the corresponding
        name already exists, it should have the proper shape. The data in the dataset are 
        overrided after writing. 

    .. function::   table_t read(H5Group dgrp)
                    table_t read(H5File file)
                    table_t read(const string &file_name)
                    void read(table_t &tbl, H5Group dgrp)
                    void read(record_t *buff, size_t n, H5Group dgrp)

        Load a table from a group in the file. Five overloads are:
        
        (1) Read a table from the group ``dgrp``. The table is returned.
        (2) Is similar but loads from the root group of the file instance ``file``.
        (3) Is similar but loads from the root group of an existing file named "file_name".
        (4) Loads into table ``tbl`` instead of returning it. ``tbl`` is resized automatically.
        (5) Loads into raw buffer starting at ``buff`` lengthed ``n`` (items of ``record_t``).
            ``n`` must be exactly the length of datasets in dgrp.
         
        Some fields may be ignored, depending on the construction of :class:`H5XTable`.
        If resize operation is used for a vector, the value of an ignored field depends
        on the default constructor of ``record_t``. Otherwise such a field is not changed.

        For example, the following code loads a table of ``DarkMatterHalo``::

            vector<DarkMatterHalo> halos = halo_manip.read("halos.h5");

        Or you may load into an existing table by::

            vector<DarkMatterHalo> halos;
            halo_manip.read(halos, HIPP::IO::H5File("halos.h5", "r").open_group("/"));  

    .. function::   void write_records(const table_t &tbl, H5Group dgrp, const string &dset)
                    void write_records(const table_t &tbl, H5File file, const string &dset)
                    void write_records(const table_t &tbl, const string &file_name, \
                        const string &dset, const string flag="w")
                    void write_records(const record_t *buff, size_t n,\
                         H5Group dgrp, const string &dset)
                         
        These methods are similar to :func:`write`, but they store data into a single dataset named ``dset``
        in the group or file.
        The datatype of the dataset is a compound type, in which the names of fields are determined
        by the names in the construction of the current instance 
        (and through :func:`add_field` and :func:`remove_field`).

    .. function::   table_t read_records(H5Group dgrp, const string &dset)
                    table_t read_records(H5File file, const string &dset)
                    table_t read_records(const string &file_name, const string &dset)
                    void read_records(table_t &tbl, H5Group dgrp, const string &dset)
                    void read_records(record_t *buff, size_t n, H5Group dgrp, const string &dset)

        These methods are similar to :func:`read`, but they load data from a single dataset named ``dset``
        in the group or file.
        The dataset must be an array of compound datatype compatible with the current instance.


    **Examples:**

    The following piece of code declares a C++ struct ``halo_t``, defines an I/O manipulator
    ``tbl_manip`` for it, writes and reads a vector of ``halo_t`` into/from HDF5 file.

    .. code:: 

        /**
         * Declare a struct for holding a dark matter halo.
         * Define a size-10 vector of it (attribute-setting codes are ignored).
         */
        struct halo_t {
            float pos[3], vel[3];
            double halo_mass;
            int id;
            float tidal_tensor[3][3];
        };
        vector<halo_t> halos(10);

        /**
         * Defined a halo table manipulator, which helps to load/store the 
         * attributes in that vector from/into four datasets. 
         */
        HIPP::IO::H5XTable<halo_t> tbl_manip(
            "Position", &halo_t::pos, 
            "Velocity", &halo_t::vel,
            "Halo Mass", &halo_t::halo_mass,
            "Tidal Tensor", &halo_t::tidal_tensor );

        /* Write the 10 halos into the root group of the file "halos.h5". */
        tbl_manip.write(halos, "halos.h5");
        
        /* Or, write them into a given group */
        tbl_manip.write(halos, 
            HIPP::IO::H5File("halos.h5", "a").create_group("Halos") );

        /* Load back the halos. */
        vector<halo_t> halos_loaded = tbl_manip.read("halos.h5");
        
        /* Or load into existing vector. */
        vector<halo_t> halos_loaded2;
        tbl_manip.read(halos_loaded2, 
            HIPP::IO::H5File("halos.h5", "r").open_group("Halos"));

    If you prefer to I/O the table as a single dataset with compound datatype,
    using :func:`read_records`/:func:`write_records` instead. For example::

        /* Output to dataset "HaloRecords" in file "halos.h5". */
        tbl_manip.write_records(halos, "halos.h5", "HaloRecords", "a");

        /* Loads it back. */
        halos_loaded = tbl_manip.read_records("halos.h5", "HaloRecords");

    Using ``h5dump halos.h5`` in the command line prompt, you have 

    .. code-block:: text 

        HDF5 "halos.h5" {
        GROUP "/" {
            DATASET "Halo Mass" {
                DATATYPE  H5T_IEEE_F64LE
                DATASPACE  SIMPLE { ( 10 ) / ( 10 ) }
                DATA { (0): 0, 100, 200, 300, 400, 500, 600, 700, 800, 900 }
            }
            ....
            GROUP "Halos" {
                DATASET "Halo Mass" {....}
                DATASET "Position" {....}
                ....
            }
            DATASET "HaloRecords" {
                DATATYPE  H5T_COMPOUND {
                    H5T_ARRAY { [3] H5T_IEEE_F32LE } "Position";
                    H5T_ARRAY { [3][3] H5T_IEEE_F32LE } "Tidal Tensor";
                    H5T_IEEE_F64LE "Halo Mass";
                    H5T_ARRAY { [3] H5T_IEEE_F32LE } "Velocity";
                }
                DATASPACE  SIMPLE { ( 10 ) / ( 10 ) }
                DATA {
                    (0): {
                        [ 0, 0, 0 ], [ 0, 0, 0, 0, 0, 0, 0, 0, 0 ],
                        0, [ 0, 0, 0 ] 
                    }, ....
                }
            }
        }
        }