Tabular-Data IO
========================

.. include:: /global.rst

The following classes are all defined within namespace ``HIPP::IO::H5``.

.. namespace:: HIPP::IO::H5


XTable
------------------------

    
.. class:: template<typename RecordT> XTable

    ``XTable`` manipulates the IO of a tabular data, i.e., array of structured type.
    The structured type (defined as ``record_t``) may be any C++ simple struct/class.
    For example::

        struct S {
            int a;
            double b[3];
            float c[2][3];
            array<array<long, 3>, 4> d;
        };

    Those simple structured types exist almost everywhere in C++ programming. The 
    instances of such a data type are usually organized as arrays 
    (e.g., ``std::vector``, raw array, or heap buffer).
    ``XTable`` defines clever interface which enables easy I/O of those data
    structures. The dataset can be either separately for each field, or as a
    single one with compound datatype for the whole ``record_t``. 
    The limitation is that the structured type must be "simple" type, with no
    virtual methods or virtual base class. It may have private attributes, 
    but those attributes cannot be directly touched by ``XTable`` outside 
    the class scope.

    Selectors are also defined for filtering the dataset. User may choose to 
    input/output a subset of all fields, or a subset of all rows.


    .. type:: RecordT record_t
            vector<record_t> table_t

        ``record_t`` : the type of the structured data element to be input/output. 
        Aliased from the class template argument ``RecordT``.
        
        ``table_t``: a vector of records each typed ``record_t``.

    .. function:: \
        XTable()
        template<typename M, typename ...Args> \
        XTable(const string &name, M record_t::* mem_ptr, Args &&...args)
        template<typename R, typename ...Args> \
        XTable(const string &name, const std::pair<size_t, XTable<R> > &field, \
            Args &&...args)

        Constructors.

        (1): default constructor. No field is added. 

        (2,3): initialize by a list of field definitions.
        
        ``args``: ``names`` and ``details``, must be paired, 
        where each name ``std::string`` or string-compatible types for the field 
        name/dataset name in the file. 
        Detail is either

        - a member pointer of ``M R::*`` where the offset, size, and datatype
          are inferred from it. sizeof(R) must be equal to sizeof(record_t)
          and R must be binary compatible with record_t in the specified fields.
        - a std::pair<size_t, XTable> instance denoting the offset 
            and details of this field in memory, i.e., the field itself is 
            another structured type.

        Fields can be added later by method :func:`add_field()` or removed by 
        :func:`remove_field()`. 
        
        The order of the field specifications is not significant. 
        
        It is not necessary to specify all the field in the C++ types. Fields 
        that are not added to ``XTable`` are not touch in file and in memory.

    .. function:: \
        XTable(const XTable &) =delete
        XTable & operator=(const XTable &) =delete
        XTable(XTable &&) =default
        XTable & operator=(XTable &&) =default
        ~XTable() =default

        XTable is not copyable, but it is movable. After move, the move-from object 
        is set to a undefined but valid state.
        
    .. function:: \
        template<typename R, typename M> \
            XTable & add_field(const string &name, M R::*mem_ptr)
        template<typename R> \
        XTable & add_field(const string &name,  \
            const std::pair<size_t, XTable<R> > &field)
        bool remove_field(const string &name) 
        bool has_field(const string &name) const noexcept
        size_t n_fields() const noexcept
        bool empty() const noexcept

        Fields manipulator, acts like a unordered_map.

        ``add_field()`` adds another fields into the table. 
        
        (1): use a member pointer. The member offset, size, and datatype are inferred 
        from that pointer.
        
        - ``R``: the record type, may be different from record_t, but must be binary 
            compatible with it.
        - ``M``: the member type.
        
        (2): add the field that is specified by another ``XTable`` instance, i.e., 
        the field itself is another structured type. The final field name is 
        "."-joined from ``name`` and the field names in ``field``.

        ``remove_field()`` tries to remove a field. Return false on failure.
        
        ``has_field()`` tests whether a field of given name exists.
        
        ``n_field()`` returns the number of fields that have been added.
        
        ``empty()`` is equivalent to ``n_fields() == 0``.

    .. function:: \
        XTable & select_rows(hsize_t start, hsize_t count, hsize_t stride = 1, \
            hsize_t block = 1) noexcept
        XTable & select_all() noexcept
        XTable & select_fields(const vector<string> &names)
        XTable & select_all_fields() noexcept

        Column (field) and row selectors.
        
        ``select_rows()``: select rows by an 1-D regular hyperslab with start, count
        stride and block specified by arguments.
        
        ``select_all()``: select all rows.

        ``select_fields()``: select a list of fields of given names.
        
        ``select_all_fields()``: select all fields.

    .. function:: \
        bool raw_array_as_atomic() const noexcept
        void raw_array_as_atomic(bool as_atomic = false) noexcept
        string dataset_create_flag() const noexcept
        void dataset_create_flag(const string &flag = "ac") noexcept
        bool dataset_create_pack() const noexcept
        void dataset_create_pack(bool pack = false) noexcept

        Specify or retrieve the detail of dataset creation and I/O.
        
        ``raw_array_as_atomic``: whether to treat raw array field (e.g., int [3],
        std::array<double, 4>) as ATOMIC ARRAY datatype.
        
        ``dataset_create_flag``: use which flag to create the dataset in write
        operations (see :expr:`Group::create_dataset`).
        
        ``dataset_create_pack``: whether to pack the datatype when creating a 
        dataset with COMPOUND datatype.

    .. function:: \
        table_t read(Group dgrp)
        table_t read(const string &file_name)
        template<typename Buff> void read(Buff &buff, Group dgrp)
        void read(void *buff, size_t &n, Group dgrp)

        Read the table data. Fields are loaded from separate datasets.
        
        (1): load from a :class:`Group` and return the table. 
        
        (2): load from the root group of a file named ``file_name``.
        
        (3): losd into a buffer ``buff``. If ``buff`` is ``std::vector``, it is 
        automatically resized. If resize happens, then either
        
        - ``buff`` is enlarged, old data are copied to the new space, new elements
          are default-constructed at the tail. 
        - ``buff`` is truncated, the tail of old data is missing but the 
          remaining is not changed.
        
        Then the actual read operation fills the specified fields. 
        
        Otherwise if ``buff`` is not a vector type, it must be consistent with the 
        file content. 
        
        The value_type of the buffer must be binary compatible with ``record_t``.

        (4) Read into a raw buffer. On entry, ``n`` is the size (number of entries) 
        of the buffer. On exit, n is the actual number of entries read.
        If ``n`` is less than required, raise a ErrLogic exception.
        With any exception, ``n`` or ``buff`` may be modified.
        
        In all cases, the space with no specified field is un-modified. 

        If 0 field is selected, return an empty table in (1), (2); 
        ``tbl`` is resized to 0 in (3); ``buff`` is untouched while ``n`` is set to
        0 in (4).

        File with 0 rows, or select 0 rows to read, is valid.

    .. function:: \
        template<typename Buff> void write(const Buff &buff, Group dgrp)
        template<typename Buff> \
        void write(const Buff &buff, const string &file_name,  \
            const string flag="w")
        void write(const void *buff, size_t n, Group dgrp)

        Write the table data as separate datasets.
        
        (1): write to a group. ``buff`` is a Contiguous buffer (e.g., raw array, 
        ``std::vector`` of a type that is binary compatible with ``record_t``).
        
        (2): write into the root group of a file named ``file_name``. The file 
        access mode is ``flag`` (see the file constructor :expr:`File::File`).
        
        (3): write ``n`` records at the raw buffer starting at the address ``buff``
        into the group ``dgrp``.

        If any dataset already exists in the file, it is opened and modified. In
        this case the file dataset must have consistent length.

    .. function:: \
        table_t read_records(Group dgrp, const string &dset_name)
        table_t read_records(const string &file_name, const string &dset_name)
        template<typename Buff> \
            void read_records(Buff &buff, Group dgrp, const string &dset_name)
        void read_records(void *buff, size_t &n,  \
            Group dgrp, const string &dset_name)

        Read a single dataset with COMPOUND datatype.
        
        (1): read from a dataset named ``dset_name`` under group ``dgrp`` and 
        return the table.
        
        (2): read from a dataset named ``dset_name`` under the root group of a
        file named ``file_name`` and return the table.
        
        (3): the same as (1), but the data is read into ``buff`` whose element type 
        is binary compatible with ``record_t``. If ``buff`` is 
        std::vector, it is auto-resized to exactly fit the desired size of data,
        and then the read is performed.

        Otherwise ``buff`` is non-vector, its size must be compatible with the size
        of the selected part in the dataset.

        (4): the same as (1), but read into a raw buffer. 
        On entry, ``n`` is the size (number of entries) 
        of the buffer. On exit, n is the actual number of entries read.
        If ``n`` is less than required, raise a ErrLogic exception.
        With any exception, ``n`` or ``buff`` may be modified. 

        In all cases, the space with no specified field is un-modified. 

        If 0 field is selected, return an empty table in (1), (2); 
        ``tbl`` is resized to 0 in (3); ``buff`` is untouched while ``n`` is set to
        0 in (4).

        File with 0 rows, or select 0 rows to read, is valid.

    .. function:: \
        template<typename Buff> \
            void write_records(const Buff &buff, Group dgrp, const string &dset_name);
        template<typename Buff> \
            void write_records(const Buff &buff, const string &file_name,  \
            const string &dset_name, const string flag="w");
        void write_records(const void *buff, size_t n, \
            Group dgrp, const string &dset_name);

        Write the records into a single dataset with COMPOUND datatype.
        
        (1): write to dataset named ``dset_name`` under group ``dgrp``. 
        ``buff`` is a Contiguous buffer (e.g., raw array, 
        ``std::vector`` of a type that is binary compatible with ``record_t``).
        
        (2): write to the dataset named ``dset_name`` under the root group of a 
        file named ``file_name``. The file access mode is ``flag`` 
        (see the file constructor :expr:`File::File`).
        
        (3): write ``n`` records at the raw buffer starting at the address ``buff``
        into the dataset named ``dset_name`` under the group ``dgrp``.

        If the dataset already exists in the file, it is opened and modified. In
        this case the file dataset must have consistent length.


Macros for XTable 
""""""""""""""""""""

.. c:macro:: HIPPIO_H5_XTABLE(R, ...) 

.. c:macro:: HIPPIO_H5_XTABLE_DEF(identifier, R, ...)

    These two macros provides quick declaration of a :class:`XTable`.

**Explanation of the macros:**

``HIPPIO_H5_XTABLE(R, ...)``: produces an un-named pr-value definition with 
record type ``R`` and any number of members (may be 0). For example:: 

    auto xtbl = HIPPIO_H5_XTABLE(S, a, b, c);

is expanded as::

    auto xtbl = ::HIPP::IO::H5::XTable<R> { "a", &S::a,
        "b", &S::b,
        "c", &S::c };

``HIPPIO_H5_XTABLE(identifier, R, ...)``: produces a named definition with 
identifier name ``identifier`` and record type ``R``. For example::

    static inline HIPPIO_H5_XTABLE(xtbl, S, a, b, c);

is expanded as::

    static inline ::HIPP::IO::H5::XTable<R> xtbl { "a", &S::a,
        "b", &S::b,
        "c", &S::c };


**Example:** I/O of tabular data with :class:`XTable`.

A structured type may have multiple fields each of which is either scalar of 
RawArray::

    struct S {
        int a;
        double b[3];
        float c[2][3];
        array<array<long, 3>, 4> d;
    };

To read/write array of ``S``, define a :class:`XTable` by passing name and 
member pointer of each desired field into the constructor::

    H5::XTable xtbl {
        "a",    &S::a,
        "b",    &S::b,
        "c",    &S::c,
        "d",    &S::d};

You may reorder the fields, or specify only subset of all fields. When performing
I/O, the un-specified gaps are not touched.

:class:`XTable` is able to write any ContiguousBuffer or the structured type, 
for example, RawArray or ``std::vector``::
    
    S data[10];
    vector<S> vec_data(10);

The following call of :expr:`XTable::write` write the records into the root 
group of a file of given name, where each field is write as a single dataset::

    xtbl.write(data, "xtbl.h5");
    xtbl.write(vec_data, "xtbl.h5");                                    // (1)

Or, write to a group::

    H5::File fout("xtbl.h5");
    xtbl.write(data, fout.create_group("S-datasets"));

The call of :expr:`XTable::write_records` writes the records into a single 
dataset with COMPOUND datatype. For example, write to a dataset named ``S-records``
under the root group of the file ``fout``::

    xtbl.write_records(data, fout, "S-records");                        // (2)

To read the databack, call :expr:`XTable::read` for separate datasets for fields, 
or :expr:`XTable::read_records` for COMPOUND datatype. For example::

    vector<S> data_in = xtbl.read(fout.open_group("S-datasets"));       // (3)

Using the preprocessor macros, the above definition of ``xtbl`` can be simplified further, 
e.g.::

    auto xtbl2 = HIPPIO_H5_XTABLE(S, a, b, c, d);
    HIPPIO_H5_XTABLE_DEF(xtbl3, S, a, b, c, d);

The file content shown by ``h5dump`` is:

- four datasets under the root group written by statement ``(1)``;
- four datasets under "S-datasets" written by statement ``(2)``;
- a dataset with COMPOUND datatype written by statement ``(3)``.

.. code-block:: text 

    HDF5 "xtbl.h5" {
    GROUP "/" {
    GROUP "S-datasets" {            # <- written by (2)
        DATASET "a" {
            DATATYPE  H5T_STD_I32LE
            DATASPACE  SIMPLE { ( 10 ) / ( 10 ) }
            DATA { ... }
        }
        DATASET "b" {
            DATATYPE  H5T_IEEE_F64LE
            DATASPACE  SIMPLE { ( 10, 3 ) / ( 10, 3 ) }
            DATA { ... }
        }
        DATASET "c" {
            DATATYPE  H5T_IEEE_F32LE
            DATASPACE  SIMPLE { ( 10, 2, 3 ) / ( 10, 2, 3 ) }
            DATA { ... }
        }
        DATASET "d" {
            DATATYPE  H5T_STD_I64LE
            DATASPACE  SIMPLE { ( 10, 4, 3 ) / ( 10, 4, 3 ) }
            DATA { ... }
        }
    }
    DATASET "S-records" {           # <- written by (3)
        DATATYPE  H5T_COMPOUND {
            H5T_STD_I32LE "a";
            H5T_ARRAY { [3] H5T_IEEE_F64LE } "b";
            H5T_ARRAY { [2][3] H5T_IEEE_F32LE } "c";
            H5T_ARRAY { [4][3] H5T_STD_I64LE } "d";
        }
        DATASPACE  SIMPLE { ( 10 ) / ( 10 ) }
        DATA { ... }
    }
    DATASET "a" {                   # <- written by (1)
        DATATYPE  H5T_STD_I32LE
        DATASPACE  SIMPLE { ( 10 ) / ( 10 ) }
        DATA { ... }
    }
    DATASET "b" {
        DATATYPE  H5T_IEEE_F64LE
        DATASPACE  SIMPLE { ( 10, 3 ) / ( 10, 3 ) }
        DATA { ... }
    }
    DATASET "c" {
        DATATYPE  H5T_IEEE_F32LE
        DATASPACE  SIMPLE { ( 10, 2, 3 ) / ( 10, 2, 3 ) }
        DATA { ... }
    }
    DATASET "d" {
        DATATYPE  H5T_STD_I64LE
        DATASPACE  SIMPLE { ( 10, 4, 3 ) / ( 10, 4, 3 ) }
        DATA { ... }
    }
    }
    }


    

