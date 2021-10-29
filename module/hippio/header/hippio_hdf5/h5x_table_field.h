#ifndef _HIPPIO_H5X_TABLE_FIELD_H_
#define _HIPPIO_H5X_TABLE_FIELD_H_

#include "h5_obj.h"
#include <optional>

namespace HIPP::IO::H5 {

namespace _h5x_table_helper {

typedef vector<char, DefaultInitAllocator<char> > buff_t;
typedef std::optional<std::array<hsize_t, 4> > row_sel_t;

class FieldLoader;

/**
Store the information of a field in a record (i.e., a member of a structured 
data type).
The member must be either 
- a RawArray. Then TypeCvt is used to identify its datatype and its 
  element datatype. 
- a scalar, which is TypeCvt available.
*/
class Field {
public:
    enum : int { fSCALAR = 0, fARRAY = 1 };

    /**
    @R: record type.
    @M: member type.

    (2): Copy information from another ``field`` instance with additionally 
    offset.
    (3): Use TypeCvtField to extract information.
    */
    template<typename R, typename M>
    Field(M R::*mem_ptr);

    Field(size_t off, const Field &field);

    template<typename R, typename M>
    Field( const TypeCvtField<R, M> &tc );

    void pack(const void *src, size_t n_records, 
        size_t record_sz, buff_t &dst) const;
    void unpack(const buff_t &src, size_t n_records, 
        size_t record_sz, void *dst) const;

    Dataset create_dataset(size_t n_records, Group &dgrp, const string &name,
        const string &flag, bool raw_array_as_atomic) const;
    void insert_datatype(const string &name, Datatype &compound_dtype) const;
private:
    friend class FieldLoader;

    size_t _offset, _size;                      /* All in bytes. */
    Datatype _dtype;                            /* datatype of the member. */

    /**
    _field_type = fSCALAR or fARRAY. If fARRAY, _extents and _scalar_dtype gives
    its extents and element datatype. Otherwise they are {1} and eq to 
    _dtype, respectively.
    */
    int _field_type;
    Dimensions _extents;
    Datatype _scalar_dtype;
};

/**
To help load/dump a field from/to a dataset.
*/
class FieldLoader {
public: 
    /**
    @dset: dset to load from/dump to. 
    @field: field description. 
    @row_sel: selection on rows of the dataset.
    @raw_array_as_atomic: true then write RawArray field as ARRAY datatype in 
        HDF5.
    @buff: I/O buffer.
    @err_info: e.g., the dataset name to print on error.
    */
    FieldLoader(Dataset &dset, const Field &field, 
        const row_sel_t &row_sel, bool raw_array_as_atomic,
        buff_t &buff, const string &err_info);

    /**
    Retrieve total number of rows in the file and the selected number of rows,
    respectively.
    */
    size_t dset_tot_n_rows() const noexcept;
    size_t sel_n_rows() const noexcept;

    /**
    dump(): dump ``n_records`` records starting from ``src``. Each record has 
    size in bytes ``record_sz``.

    load(): load from dataset to dst, instead.
    */
    void dump(const void *src, size_t n_records, size_t record_sz);
    void load(size_t n_records, size_t record_sz, void *dst);

    Dataset & dataset() { return _dset; }

    /** 
    Find detail for a 1-D space with ``n_rows`` selected with ``sel``.
    Return number of selected elements and the selected dataspace.
    If selection invalid, throw ``ErrLogic(eINVALIDARG)``.
    */
    static std::pair<hsize_t, Dataspace> find_sel_n_rows(
        hsize_t n_rows, const row_sel_t &sel, const string &err_info) noexcept;
protected:
    Dataset &_dset; 
    Dataspace _file_space;
    const Dimensions _file_dims;
    hsize_t _sel_n_rows;
    const Datatype _file_dtype;
    const Datatype::class_t _file_dtype_class;
    
    const Field &_field;
    Datatype _mem_dtype;
    Dataspace _mem_space;

    const row_sel_t &_row_sel;
    const bool _raw_array_as_atomic;
    buff_t &_buff;
    const string &_err_info;

    void _field_dset_cross_chk();
    void _process_arr_field();
    void _process_scalar_field();

    template<typename ...Args>
    void _throw_err_arg(Args &&...args) const;
};

template<typename R, typename M>
Field::Field(M R::*mem_ptr) : Field(TypeCvtField<R, M>{mem_ptr}) {
}

template<typename R, typename M>
Field::Field( const TypeCvtField<R, M> &tc ) 
: _offset(tc.offset()), _size(tc.size()), _dtype(tc.datatype()), 
_field_type(tc.is_raw_array() ? fARRAY : fSCALAR), _extents(tc.extents()), 
_scalar_dtype(tc.value_datatype())
{ }

template<typename ...Args>
void FieldLoader::_throw_err_arg(Args &&...args) const {
    auto &f = _field;
    ErrLogic::throw_(ErrLogic::eINVALIDARG, 
        std::forward<Args>(args)..., '\n',
        "  ... dataset ", _err_info, " IO failed\n",
        "  ... Field{offset=", f._offset, ", size=", f._size, 
            ", field_type=", f._field_type, "}\n",
        "  ... filespace{dims=", _file_space.dims(), ", selected n_rows=", 
            _sel_n_rows, "}, memspace{", _mem_space.dims(), "}\n");
}

} // namespace _h5x_table_helper

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5X_TABLE_FIELD_H_