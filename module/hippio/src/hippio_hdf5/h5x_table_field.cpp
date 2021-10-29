#include <hippio.h>

namespace HIPP::IO::H5 {

namespace _h5x_table_helper {

Field::Field(size_t off, const Field &field) 
: Field(field)
{
    _offset += off;
}

void Field::pack(const void *src, size_t n_records, 
    size_t record_sz, buff_t &dst) const
{
    size_t tot_sz = n_records * _size;
    dst.resize(tot_sz);

    const char *psrc = reinterpret_cast<const char *>(src);
    for(size_t i=0; i<n_records; ++i)
        std::copy_n(psrc + record_sz * i + _offset, _size, &dst[i*_size]);
}

void Field::unpack(const buff_t &src, size_t n_records, 
    size_t record_sz, void *dst) const
{
    size_t tot_sz = n_records * _size;
    if( src.size() != tot_sz )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... input ",
        "source buffer size ", src.size(), " != expectation ", tot_sz, '\n');
    
    char *pdst = reinterpret_cast<char *>(dst);
    for(size_t i=0; i<n_records; ++i)
        std::copy_n(&src[i*_size], _size, pdst + record_sz * i + _offset);
}

Dataset Field::create_dataset(size_t n_records, Group &dgrp, const string &name,
    const string &flag, bool raw_array_as_atomic) const 
{
    if(_field_type == fSCALAR || ( raw_array_as_atomic ) ) {
        return dgrp.create_dataset(name, _dtype, {n_records}, flag);
    } else {
        size_t rank = _extents.size();
        Dimensions dims(rank+1);
        dims[0] = n_records;
        std::copy_n(_extents.data(), rank, &dims[1]);
        return dgrp.create_dataset(name, _scalar_dtype, dims, flag);
    }
}

void Field::insert_datatype(const string &name, Datatype &compound_dtype) const 
{
    compound_dtype.insert(name, _offset, _dtype);
}

FieldLoader::FieldLoader(Dataset &dset, const Field &field, 
    const row_sel_t &row_sel, bool raw_array_as_atomic, buff_t &buff, 
    const string &err_info)
: _dset(dset), _file_space(_dset.dataspace()), _file_dims(_file_space.dims()),
_file_dtype(_dset.datatype()), _file_dtype_class(_file_dtype.get_class()),
_field(field), _mem_dtype{nullptr}, _mem_space{nullptr}, _row_sel(row_sel),
_raw_array_as_atomic(raw_array_as_atomic), _buff(buff), _err_info(err_info)
{
    _field_dset_cross_chk();
}

size_t FieldLoader::dset_tot_n_rows() const noexcept {
    return _file_dims[0];
}

size_t FieldLoader::sel_n_rows() const noexcept {
    return _sel_n_rows;
}

void FieldLoader::dump(const void *src, size_t n_records, size_t record_sz) {
    if( _sel_n_rows != hsize_t(n_records) ) _throw_err_arg(emFLPFB, 
        "dump ", n_records, " to selected length ", _sel_n_rows);
    _field.pack(src, n_records, record_sz, _buff);
    _dset.write(_buff.data(), _mem_dtype, _mem_space, _file_space);
}

void FieldLoader::load(size_t n_records, size_t record_sz, void *dst) {
    if( _sel_n_rows != hsize_t(n_records) ) _throw_err_arg(emFLPFB, 
        "load ", n_records, " from selected length ", _sel_n_rows);
    size_t tot_sz = _field._size * n_records;
    _buff.resize(tot_sz);
    _dset.read(_buff.data(), _mem_dtype, _mem_space, _file_space);
    _field.unpack(_buff, n_records, record_sz, dst);
}

std::pair<hsize_t, Dataspace> FieldLoader::find_sel_n_rows(hsize_t n_rows, 
    const row_sel_t &sel, const string &err_info) noexcept
{
    Dataspace dsp1d {n_rows};
    hsize_t sel_n_rows = n_rows;
    if( sel ) {
        auto &[start, count, stride, block] = *sel;
        dsp1d.select_hyperslab(dsp1d.selSET, 
            &start, &count, &stride, &block);
        if( !dsp1d.select_valid() ) ErrLogic::throw_(ErrLogic::eINVALIDARG, 
            emFLPFB, "  ... dataset ", err_info, ", invalid selection ", 
                start, ", ", count, ", ", stride, ", ", block, 
                " with n_rows ", n_rows);
        sel_n_rows = dsp1d.get_select_npoints();
    }
    return { sel_n_rows, std::move(dsp1d) };
}

void FieldLoader::_field_dset_cross_chk() {

    // check file_n_dims.
    size_t file_ndims = _file_dims.ndims();
    if( file_ndims == 0 ) _throw_err_arg(emFLPFB, "rank = 0");

    if( _field._field_type == _field.fSCALAR 
        || _file_dtype_class  == Datatype::cARRAY )
        if( file_ndims != 1 ) _throw_err_arg(emFLPFB, " ndims ", 
            file_ndims, " != 1");

    // selection applied to file space.
    _sel_n_rows = find_sel_n_rows(_file_dims[0], _row_sel, _err_info).first;
    if( _row_sel ) _file_space.select_hyperslab( 
            Hyperslab::subsetting_1d(_file_dims, 0, *_row_sel) );

    // process detail mem_dtype, mem_space.
    if( _field._field_type == _field.fARRAY )
        _process_arr_field();
    else
        _process_scalar_field();
}

void FieldLoader::_process_arr_field() {
    auto &mem_ext = _field._extents; size_t mem_rank = mem_ext.ndims();
    if( _file_dtype_class == Datatype::cARRAY ) {
        if( !_raw_array_as_atomic ) _throw_err_arg(emFLPFB, 
            "ARRAY expect atomic");
        auto arr_dims = _file_dtype.array_dims();
        if( arr_dims != mem_ext ) _throw_err_arg(emFLPFB, 
            "ARRAY extents ", arr_dims, " != field extents ", mem_ext);
        _mem_dtype = _field._dtype;
        _mem_space = Dataspace {_sel_n_rows};
    } else {
        if( _raw_array_as_atomic ) _throw_err_arg(emFLPFB,
            "atomic array field cannot dump to scalar dataset");
        size_t file_rank = _file_dims.ndims();
        if( file_rank != mem_rank  + 1 ) _throw_err_arg(emFLPFB,
            "ndims ", file_rank, " != field rank ", mem_rank);
        for(size_t i=0; i<mem_rank; ++i)
            if( mem_ext[i] != _file_dims[i+1] ) _throw_err_arg(emFLPFB, "dim ", 
            i, ", extent ", _file_dims[i+1], " != field extent ", mem_ext[i]);
        Dimensions mem_dims = _file_dims;
        mem_dims[0] = _sel_n_rows;
        _mem_space = Dataspace { mem_dims };
        _mem_dtype = _field._scalar_dtype;
    }
}

void FieldLoader::_process_scalar_field() {
    if( _file_dtype_class == Datatype::cARRAY )
        _throw_err_arg(emFLPFB, "cannot map scalar field to ARRAY dataset");
    _mem_dtype = _field._dtype;
    _mem_space = Dataspace {_sel_n_rows};
}

} // namespace _h5x_table_helper
    
} // namespace HIPP::IO::H5
