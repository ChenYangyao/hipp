/**
create: Yangyao CHEN, 2020/11/28
    [write   ] XTable: high-level tabular data manipulator.
    [update  ] 2021/10/19, - now support the subsetting of fields and rows.
        Also support passing another XTable as member to create XTable for 
        nested structured types.
*/ 

#ifndef _HIPPIO_H5X_TABLE_H_
#define _HIPPIO_H5X_TABLE_H_

#include "h5_obj.h"
#include "h5x_table_field.h"

namespace HIPP::IO::H5 {

/**
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
*/
template<typename RecordT>
class XTable {
public:

    /**
    ``record_t`` : the type of the structured data element to be input/output.
    ``table_t``: a vector of records each typed ``record_t``.
    */
    typedef RecordT record_t;
    typedef vector<record_t> table_t;

    template<typename T> static constexpr bool _is_buff() noexcept;
    template<typename T> static constexpr void _assert_buff() noexcept;

    /**
    Constructors.

    (1): default constructor. No field is added. 

    (2): initialize by a list of field definitions.
    
    @args: ``names`` and ``details``, must be paired, 
    where each name ``std::string`` or string-compatible types for the field 
    name/dataset name in the file. 
    Detail is either
    - a member pointer of ``M R::*`` where the offset, size, and datatype
      are inferred from it. sizeof(R) must be equal to sizeof(record_t)
      and R must be binary compatible with record_t in the specified fields.
    - a std::pair<size_t, XTable> instance denoting the offset 
      and details of this field in memory, i.e., the field itself is 
      another structured type.

    Fields can be added later by method ``add_field()`` or removed by 
    ``remove_field()``. 
    
    The order of the field specifications is not significant. 
    
    It is not necessary to specify all the field in the C++ types. Fields 
    that are not added to ``XTable`` are not touch in file and in memory.
    */
    XTable();
    
    template<typename M, typename ...Args>
    XTable(const string &name, M record_t::* mem_ptr, Args &&...args);
    
    template<typename R, typename ...Args>
    XTable(const string &name, const std::pair<size_t, XTable<R> > &field, 
        Args &&...args);

    /**
    XTable is copyable and movable.
    
    The move-from object is set to a undefined but valid state.
    
    The copy-to object is independent from the source object. Making calls on
    them is thread-safe, as long as the underlying file operations are 
    thread-safe.
    */
    XTable(const XTable &);
    XTable & operator=(const XTable &);
    XTable(XTable &&);
    XTable & operator=(XTable &&);
    
    ~XTable() =default;

    /** 
    Fields manipulator, acts like a unordered_map.

    ``add_field()`` adds another fields into the table. 
    
    (1): use a member pointer. The member offset, size, and datatype are inferred 
    from that pointer.
    @R: the record type, may be different from record_t, but must be binary 
        compatible with it.
    @M: the member type.
    
    (2): add the field that is specified by another ``XTable`` instance, i.e., 
    the field itself is another structured type. The final field name is 
    "."-joined from ``name`` and the field names in ``field``.

    ``remove_field()`` tries to remove a field. Return false on failure.
    ``has_field()`` tests whether a field of given name exists.
    ``n_field()`` returns the number of fields that have been added.
    ``empty()`` is equivalent to ``n_fields() == 0``.
    */
    template<typename R, typename M>
    XTable & add_field(const string &name, M R::*mem_ptr);

    template<typename R>
    XTable & add_field(const string &name, 
        const std::pair<size_t, XTable<R> > &field);
        
    bool remove_field(const string &name); 
    bool has_field(const string &name) const noexcept;
    size_t n_fields() const noexcept;
    bool empty() const noexcept;

    /**
    Column (field) and row selectors.
    
    ``select_rows()``: select rows by an 1-D regular hyperslab with start, count
        stride and block specified by arguments.
    ``select_all()``: select all rows.

    ``select_fields()``: select a list of fields of given names.
    ``select_all_fields()``: select all fields.
    */
    XTable & select_rows(hsize_t start, hsize_t count, hsize_t stride = 1, 
        hsize_t block = 1) noexcept;
    XTable & select_all() noexcept;
    XTable & select_fields(const vector<string> &names);
    XTable & select_all_fields() noexcept;

    /**
    Specify or retrieve the detail of dataset creation and I/O.
    ``raw_array_as_atomic``: whether to treat raw array field (e.g., int [3],
        std::array<double, 4>) as ATOMIC ARRAY datatype.
    ``dataset_create_flag``: use which flag to create the dataset in write
        operations (see :expr:`Group::create_dataset`).
    ``dataset_create_pack``: whether to pack the datatype when creating a 
        dataset with COMPOUND datatype.
    */
    bool raw_array_as_atomic() const noexcept;
    void raw_array_as_atomic(bool as_atomic = false) noexcept;
    string dataset_create_flag() const noexcept;
    void dataset_create_flag(const string &flag = "ac") noexcept;
    bool dataset_create_pack() const noexcept;
    void dataset_create_pack(bool pack = false) noexcept;

    /** 
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

    Thread-safety: internal state modified, hance not thread-safe. Make a 
    copied XTable instead.
    */
    table_t read(Group dgrp);
    table_t read(const string &file_name);
    template<typename Buff> void read(Buff &buff, Group dgrp);
    void read(void *buff, size_t &n, Group dgrp);

    /**
    Write the table data as separate datasets.
    (1): write to a group. ``buff`` is a Contiguous buffer (e.g., raw array, 
        ``std::vector`` of a type that is binary compatible with ``record_t``).
    (2): write into the root group of a file named ``file_name``. The file 
        access mode is ``flag`` (see the file constructor :expr:`File::File`).
    (3): write ``n`` records at the raw buffer starting at the address ``buff``
        into the group ``dgrp``.

    If any dataset already exists in the file, it is opened and modified. In
    this case the file dataset must have consistent length.

    Thread-safety: internal state modified, hance not thread-safe. Make a 
    copied XTable instead.
    */
    template<typename Buff> void write(const Buff &buff, Group dgrp);
    template<typename Buff>
    void write(const Buff &buff, const string &file_name, 
        const string flag="w");
    void write(const void *buff, size_t n, Group dgrp);

    /** 
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

    Thread-safety: internal state modified, hance not thread-safe. Make a 
    copied XTable instead.
    */
    table_t read_records(Group dgrp, const string &dset_name);
    table_t read_records(const string &file_name, const string &dset_name);
    template<typename Buff>
    void read_records(Buff &buff, Group dgrp, const string &dset_name);
    void read_records(void *buff, size_t &n, 
        Group dgrp, const string &dset_name);

    /**
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

    Thread-safety: internal state modified, hance not thread-safe. Make a 
    copied XTable instead.
    */
    template<typename Buff>
    void write_records(const Buff &buff, Group dgrp, const string &dset_name);
    template<typename Buff>
    void write_records(const Buff &buff, const string &file_name, 
        const string &dset_name, const string flag="w");
    void write_records(const void *buff, size_t n,
         Group dgrp, const string &dset_name);
private:
    typedef std::optional<vector<string> > _field_sel_t;
    typedef _h5x_table_helper::row_sel_t _row_sel_t;
    typedef _h5x_table_helper::buff_t _buff_t;

    typedef _h5x_table_helper::Field _Field;
    typedef _h5x_table_helper::FieldLoader _FieldLoader;
    typedef std::unordered_map<string, _Field> _field_map_t;
    typedef _field_map_t::const_iterator _field_iter_t;

    _field_map_t _fields;

    _field_sel_t _field_sel;
    _row_sel_t _row_sel;

    bool _raw_array_as_atomic;
    string _dataset_create_flag;
    bool _dataset_create_pack;

    mutable _buff_t _buff;

    template<typename Arg1, typename ...Args>
    void _push_fields(const string &name, Arg1 &&a1, Args &&...args);
    
    _field_iter_t _find_field(const string &name) const;
    std::pair<bool, hsize_t> _find_file_n_rows(Group &src_dgrp) const;          // no selected => {false, 0}, otherwise {true, selected n_rows}
    Datatype _get_compound_dtype() const;                                       // no selected => empty.
    std::pair<hsize_t, Dataspace> _get_compound_filespace(
        Dataset &src_dset, const string &dset_name) const;                      // may return 0. selected filespace
    //std::tuple<bool, hsize_t, Datatype, Dataspace> 
    //    _get_compound_detail(Dataset &src_dset, const string &dset_name) const; // no selected => {false, 0, empty}, otherwise {true, selected n_rows, dtype}
    
    void _read(record_t *buff, hsize_t n, Group &dgrp) const;                   // actual read, n must be equal to required.
    void _read_field(record_t *buff, hsize_t n, Group &dgrp, 
        _field_iter_t it) const;
    void _write_field(const record_t *buff, hsize_t n, Group &dgrp, 
        _field_iter_t it) const;

    void _read_records(record_t *buff, hsize_t n, Dataset &dset, 
        Datatype &dtype, Dataspace &dsp) const;
};

/**
Template class deduction guide.
The record type is deduced from the member pointer of the first field 
specified in the constructor.
*/
template<typename R, typename M, typename ...Args>
XTable(const string &name, M R::* mem_ptr, Args &&...args) -> XTable<R>;

#define _HIPPIO_H5_XTABLE_ARGS0(R, m)
#define _HIPPIO_H5_XTABLE_ARGS1(R, m) #m, &R::m
#define _HIPPIO_H5_XTABLE_ARGS2(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS1(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS3(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS2(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS4(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS3(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS5(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS4(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS6(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS5(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS7(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS6(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS8(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS7(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS9(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS8(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS10(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS9(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS11(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS10(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS12(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS11(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS13(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS12(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS14(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS13(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS15(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS14(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS16(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS15(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS17(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS16(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS18(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS17(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS19(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS18(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS20(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS19(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS21(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS20(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS22(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS21(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS23(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS22(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS24(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS23(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS25(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS24(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS26(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS25(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS27(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS26(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS28(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS27(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS29(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS28(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS30(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS29(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS31(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS30(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS32(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS31(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS33(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS32(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS34(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS33(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS35(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS34(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS36(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS35(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS37(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS36(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS38(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS37(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS39(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS38(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS40(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS39(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS41(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS40(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS42(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS41(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS43(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS42(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS44(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS43(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS45(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS44(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS46(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS45(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS47(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS46(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS48(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS47(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS49(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS48(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS50(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS49(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS51(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS50(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS52(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS51(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS53(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS52(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS54(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS53(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS55(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS54(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS56(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS55(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS57(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS56(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS58(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS57(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS59(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS58(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS60(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS59(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS61(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS60(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS62(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS61(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS63(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS62(R, __VA_ARGS__)
#define _HIPPIO_H5_XTABLE_ARGS64(R, m, ...) #m, &R::m, _HIPPIO_H5_XTABLE_ARGS63(R, __VA_ARGS__)

#define _HIPPIO_H5_XTABLE_GET_MACRO(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, _64, macro_name, ...) macro_name

#define _HIPPIO_H5_XTABLE_EXPAND_ARGS(R, ...) \
    _HIPPIO_H5_XTABLE_GET_MACRO(_0, __VA_ARGS__, \
    _HIPPIO_H5_XTABLE_ARGS64, \
    _HIPPIO_H5_XTABLE_ARGS63, \
    _HIPPIO_H5_XTABLE_ARGS62, \
    _HIPPIO_H5_XTABLE_ARGS61, \
    _HIPPIO_H5_XTABLE_ARGS60, \
    _HIPPIO_H5_XTABLE_ARGS59, \
    _HIPPIO_H5_XTABLE_ARGS58, \
    _HIPPIO_H5_XTABLE_ARGS57, \
    _HIPPIO_H5_XTABLE_ARGS56, \
    _HIPPIO_H5_XTABLE_ARGS55, \
    _HIPPIO_H5_XTABLE_ARGS54, \
    _HIPPIO_H5_XTABLE_ARGS53, \
    _HIPPIO_H5_XTABLE_ARGS52, \
    _HIPPIO_H5_XTABLE_ARGS51, \
    _HIPPIO_H5_XTABLE_ARGS50, \
    _HIPPIO_H5_XTABLE_ARGS49, \
    _HIPPIO_H5_XTABLE_ARGS48, \
    _HIPPIO_H5_XTABLE_ARGS47, \
    _HIPPIO_H5_XTABLE_ARGS46, \
    _HIPPIO_H5_XTABLE_ARGS45, \
    _HIPPIO_H5_XTABLE_ARGS44, \
    _HIPPIO_H5_XTABLE_ARGS43, \
    _HIPPIO_H5_XTABLE_ARGS42, \
    _HIPPIO_H5_XTABLE_ARGS41, \
    _HIPPIO_H5_XTABLE_ARGS40, \
    _HIPPIO_H5_XTABLE_ARGS39, \
    _HIPPIO_H5_XTABLE_ARGS38, \
    _HIPPIO_H5_XTABLE_ARGS37, \
    _HIPPIO_H5_XTABLE_ARGS36, \
    _HIPPIO_H5_XTABLE_ARGS35, \
    _HIPPIO_H5_XTABLE_ARGS34, \
    _HIPPIO_H5_XTABLE_ARGS33, \
    _HIPPIO_H5_XTABLE_ARGS32, \
    _HIPPIO_H5_XTABLE_ARGS31, \
    _HIPPIO_H5_XTABLE_ARGS30, \
    _HIPPIO_H5_XTABLE_ARGS29, \
    _HIPPIO_H5_XTABLE_ARGS28, \
    _HIPPIO_H5_XTABLE_ARGS27, \
    _HIPPIO_H5_XTABLE_ARGS26, \
    _HIPPIO_H5_XTABLE_ARGS25, \
    _HIPPIO_H5_XTABLE_ARGS24, \
    _HIPPIO_H5_XTABLE_ARGS23, \
    _HIPPIO_H5_XTABLE_ARGS22, \
    _HIPPIO_H5_XTABLE_ARGS21, \
    _HIPPIO_H5_XTABLE_ARGS20, \
    _HIPPIO_H5_XTABLE_ARGS19, \
    _HIPPIO_H5_XTABLE_ARGS18, \
    _HIPPIO_H5_XTABLE_ARGS17, \
    _HIPPIO_H5_XTABLE_ARGS16, \
    _HIPPIO_H5_XTABLE_ARGS15, \
    _HIPPIO_H5_XTABLE_ARGS14, \
    _HIPPIO_H5_XTABLE_ARGS13, \
    _HIPPIO_H5_XTABLE_ARGS12, \
    _HIPPIO_H5_XTABLE_ARGS11, \
    _HIPPIO_H5_XTABLE_ARGS10, \
    _HIPPIO_H5_XTABLE_ARGS9, \
    _HIPPIO_H5_XTABLE_ARGS8, \
    _HIPPIO_H5_XTABLE_ARGS7, \
    _HIPPIO_H5_XTABLE_ARGS6, \
    _HIPPIO_H5_XTABLE_ARGS5, \
    _HIPPIO_H5_XTABLE_ARGS4, \
    _HIPPIO_H5_XTABLE_ARGS3, \
    _HIPPIO_H5_XTABLE_ARGS2, \
    _HIPPIO_H5_XTABLE_ARGS1, \
    _HIPPIO_H5_XTABLE_ARGS0, ... ) (R, __VA_ARGS__)

/**
These two macros provides quick declaration of a :class:`XTable`.

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
*/
#define HIPPIO_H5_XTABLE(R, ...) \
    ::HIPP::IO::H5::XTable<R> { _HIPPIO_H5_XTABLE_EXPAND_ARGS(R, __VA_ARGS__) }
#define HIPPIO_H5_XTABLE_DEF(identifier, R, ...) \
    ::HIPP::IO::H5::XTable<R> identifier { _HIPPIO_H5_XTABLE_EXPAND_ARGS(R, __VA_ARGS__) }

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5X_TABLE_H_