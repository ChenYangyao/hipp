#ifndef _HIPPIO_H5X_TABLE_IMPL_H_
#define _HIPPIO_H5X_TABLE_IMPL_H_

#include "../hippio_hdf5/h5x_table.h"

namespace HIPP::IO::H5 {

#define _HIPP_TEMPHD template<typename RecordT>
#define _HIPP_TEMPARG <RecordT>
#define _HIPP_TEMPCLS XTable _HIPP_TEMPARG
#define _HIPP_TEMPRET _HIPP_TEMPHD inline auto _HIPP_TEMPCLS::
#define _HIPP_TEMPNORET _HIPP_TEMPHD inline _HIPP_TEMPCLS::

_HIPP_TEMPHD
template<typename T> constexpr bool _HIPP_TEMPCLS::_is_buff() noexcept {
    typedef ContiguousBufferTraits<T> tr_t; 
    return tr_t::is_buffer && ! std::is_pointer_v<T> 
        && sizeof(typename tr_t::value_t) == sizeof(record_t);
        // may change to, e.g.
        // std::is_same_v< std::remove_cv_t<typename tr_t::value_t>, 
        //  std::remove_cv_t<record_t> >;
        // in later versions for greater safety.
}

_HIPP_TEMPHD
template<typename T> constexpr void _HIPP_TEMPCLS::_assert_buff() noexcept {
    typedef ContiguousBufferTraits<T> tr_t; 
    static_assert(tr_t::is_buffer, "Is not buffer");
    static_assert(!std::is_pointer_v<T>, 
        "Raw pointer is not self-descriptive buffer");
    static_assert(sizeof(typename tr_t::value_t) == sizeof(record_t), 
        "Buffer element size does not match record type");
}

_HIPP_TEMPNORET
XTable::XTable()
: _raw_array_as_atomic(false), _dataset_create_flag("ac"), 
_dataset_create_pack(false) {}
    
_HIPP_TEMPHD
template<typename M, typename ...Args>
_HIPP_TEMPCLS::XTable(const string &name, M record_t::* mem_ptr, Args &&...args)
:XTable()
{
    add_field(name, mem_ptr);
    if constexpr (sizeof...(Args) > 0) {
        _push_fields( std::forward<Args>(args)... );
    }
}

_HIPP_TEMPHD
template<typename R, typename ...Args>
_HIPP_TEMPCLS::XTable(const string &name, 
    const std::pair<size_t, XTable<R> > &field, Args &&...args)
:XTable()
{
    add_field(name, field);
    if constexpr (sizeof...(Args) > 0) {
        _push_fields( std::forward<Args>(args)... );
    }
}

_HIPP_TEMPHD
template<typename R, typename M>
auto _HIPP_TEMPCLS::add_field(const string &name, M R::*mem_ptr) ->XTable & {
    static_assert( sizeof(R) == sizeof(record_t), 
        "R is not compatible with the record type" );
    if( has_field(name) )
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, "  ... field ", name, 
            " is already existing\n");
    _fields.emplace(name, mem_ptr);
    return *this;
}

_HIPP_TEMPHD
template<typename R>
inline auto _HIPP_TEMPCLS::add_field(const string &name,
    const std::pair<size_t, XTable<R> > &field) ->XTable &
{
    auto &[off, tbl] = field;
    for(auto &[sub_name, sub_field]: tbl._fields){
        string join_name = name + "." + sub_name;
        if( has_field(join_name) )
            ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, "  ... subfield ", 
                join_name, " is already existing\n");
        _fields.emplace(join_name, sub_field);
    }
    return *this;
}

_HIPP_TEMPRET remove_field(const string &name) -> bool {
    return (bool) _fields.erase(name);
}

_HIPP_TEMPRET has_field(const string &name) const noexcept -> bool {
    return _fields.find(name) != _fields.end();
}

_HIPP_TEMPRET n_fields() const noexcept -> size_t {
    return _fields.size();
}

_HIPP_TEMPRET empty() const noexcept -> bool {
    return _fields.empty();
}

_HIPP_TEMPRET select_rows(hsize_t start, hsize_t count, hsize_t stride, 
    hsize_t block) noexcept -> XTable & 
{
    _row_sel = std::array<hsize_t, 4>{start, count, stride, block};
    return *this;
}

_HIPP_TEMPRET select_all() noexcept -> XTable & {
    _row_sel.reset();
    return *this;
}

_HIPP_TEMPRET select_fields(const vector<string> &names) -> XTable & {
    _field_sel = names;
    return *this;
}

_HIPP_TEMPRET select_all_fields() noexcept -> XTable & {
    _field_sel.reset();
    return *this;
}

_HIPP_TEMPRET raw_array_as_atomic() const noexcept -> bool {
    return _raw_array_as_atomic;
}

_HIPP_TEMPRET raw_array_as_atomic(bool as_atomic) noexcept -> void {
    _raw_array_as_atomic = as_atomic;
}

_HIPP_TEMPRET dataset_create_flag() const noexcept -> string {
    return _dataset_create_flag;
}

_HIPP_TEMPRET dataset_create_flag(const string &flag) noexcept -> void {
    _dataset_create_flag = flag;
}

_HIPP_TEMPRET dataset_create_pack() const noexcept -> bool {
    return _dataset_create_pack;
}

_HIPP_TEMPRET dataset_create_pack(bool pack) noexcept -> void {
    _dataset_create_pack = pack;
}

_HIPP_TEMPRET read(Group dgrp) -> table_t {
    table_t ret;
    read(ret, std::move(dgrp));
    return ret;
}

_HIPP_TEMPRET read(const string &file_name) -> table_t {
    return read( File(file_name, "r") );
}

_HIPP_TEMPHD
template<typename Buff>
void _HIPP_TEMPCLS::read(Buff &buff, Group dgrp) {
    _assert_buff<Buff>();
    
    typedef TypeCvtBuff<Buff> tr_t;
    if constexpr (tr_t::is_vector) {
        auto [has_one, n_rows] = _find_file_n_rows(dgrp);
        buff.resize(n_rows);
        if( has_one && n_rows > 0 ) 
            _read(buff.data(), n_rows, dgrp);
    }else {
        auto [p,n] = ContiguousBufferTraits<Buff>{buff};
        read(p, n, std::move(dgrp));
    }
}

_HIPP_TEMPRET read(void *buff, size_t &n, Group dgrp) -> void {
    auto [has_one, n_rows] = _find_file_n_rows(dgrp);
    if( n_rows > n )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... buffer provided size ", n, " is less than required ", 
            n_rows, '\n');
    n = n_rows;
    if( has_one && n_rows > 0 )
        _read((record_t *)buff, n_rows, dgrp);
}

_HIPP_TEMPHD
template<typename Buff>
void _HIPP_TEMPCLS::write(const Buff &buff, Group dgrp) {
    _assert_buff<Buff>();

    auto [p,n] = ContiguousBufferTraits<const Buff>{buff};
    write(p, n, std::move(dgrp));
}

_HIPP_TEMPHD
template<typename Buff>
void _HIPP_TEMPCLS::write(const Buff &buff, const string &file_name, 
    const string flag)
{
    write(buff, File(file_name, flag));
}

_HIPP_TEMPRET write(const void *buff, size_t n, Group dgrp) -> void {
    if( _field_sel ) {
        const auto &names = *_field_sel;
        for(auto &name: names)
            _write_field((record_t *)buff, n, dgrp, _find_field(name));
    } else {
        for(auto it=_fields.cbegin(); it!=_fields.cend(); ++it)
            _write_field((record_t *)buff, n, dgrp, it);
    }
}

_HIPP_TEMPRET read_records(Group dgrp, const string &dset_name) -> table_t {
    table_t tbl;
    read_records(tbl, std::move(dgrp), dset_name);
    return tbl;
}

_HIPP_TEMPRET read_records(const string &file_name, const string &dset_name) 
-> table_t 
{
    return read_records(File(file_name, "r"), dset_name);
}

_HIPP_TEMPHD
template<typename Buff>
void _HIPP_TEMPCLS::read_records(Buff &buff, Group dgrp, const string &dset_name)
{
    _assert_buff<Buff>();
    typedef TypeCvtBuff<Buff> tr_t;
    if constexpr ( tr_t::is_vector ) {
        auto dset = dgrp.open_dataset(dset_name);
        auto [n_rows, dsp] = _get_compound_filespace(dset, dset_name);
        buff.resize(n_rows);
        if( n_rows == 0 ) return;

        auto dt = _get_compound_dtype();
        if( dt.has_referenced() ) 
            _read_records(buff.data(), n_rows, dset, dt, dsp);    
        else 
            buff.resize(0);
    } else {
        auto [p,n] = ContiguousBufferTraits<Buff>{buff};
        read(p, n, std::move(dgrp), dset_name);
    }
}

_HIPP_TEMPRET read_records(void *buff, size_t &n, Group dgrp, 
    const string &dset_name) -> void 
{
    auto dset = dgrp.open_dataset(dset_name);
    auto [n_rows, dsp] = _get_compound_filespace(dset, dset_name);
    if( n_rows > n )
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, 
            "  ... buffer provided size ", n, " is less than required ", 
            n_rows, '\n');
    n = n_rows;
    
    auto dt = _get_compound_dtype();
    if( dt.has_referenced() ) 
        _read_records((record_t *)buff, n_rows, dset, dt, dsp);
    else 
        n = 0;
}

_HIPP_TEMPHD
template<typename Buff>
void _HIPP_TEMPCLS::write_records(const Buff &buff, Group dgrp, 
    const string &dset_name)
{
    _assert_buff<Buff>();

    auto [p, n] = ContiguousBufferTraits<const Buff>{buff};
    write_records(p, n, std::move(dgrp), dset_name);
}

_HIPP_TEMPHD
template<typename Buff>
void _HIPP_TEMPCLS::write_records(const Buff &buff, const string &file_name, 
    const string &dset_name, const string flag)
{
    write_records(buff, File(file_name, flag), dset_name);
}

_HIPP_TEMPRET write_records(const void *buff, size_t n, Group dgrp, 
    const string &dset_name) -> void 
{
    auto file_dtype = _get_compound_dtype(),
        mem_dtype = file_dtype.copy();
    if( _dataset_create_pack ) 
        file_dtype.pack();

    Dataspace mem_sp {n};
    auto dset = dgrp.create_dataset(dset_name, file_dtype, mem_sp, 
        _dataset_create_flag);

    auto [file_n_rows, file_sp] = _get_compound_filespace(dset, dset_name);
    auto [sel_n_rows, sel_sp] = 
        _FieldLoader::find_sel_n_rows(file_n_rows, _row_sel, dset_name);
    
    auto xprop = Dataset::create_proplist("xfer");
    xprop.preserve(true);
    dset.write(buff, mem_dtype, mem_sp, sel_sp, xprop);
}

_HIPP_TEMPHD
template<typename Arg1, typename ...Args>
void _HIPP_TEMPCLS::_push_fields(const string &name, Arg1 &&a1, Args &&...args) 
{
    add_field(name, std::forward<Arg1>(a1));
    if constexpr (sizeof...(Args) > 0) {
        _push_fields( std::forward<Args>(args)... );
    }
}

_HIPP_TEMPRET
_find_field(const string &name) const -> _field_iter_t {
    auto it = _fields.find(name);
    if( it == _fields.end() )
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, "  ... field ", name, 
            " is not existing");
    return it;
}

_HIPP_TEMPRET
_find_file_n_rows(Group &src_dgrp) const -> std::pair<bool, hsize_t> {
    _field_iter_t it;
    if ( _field_sel ) {
        const auto &names = *_field_sel;
        if( names.empty() ) return {false, hsize_t(0)};
        it = _find_field( names[0] );
    } else {
        if( _fields.empty() )  return {false, hsize_t(0)};
        it = _fields.cbegin();
    }
    auto dset = src_dgrp.open_dataset(it->first);
    _FieldLoader fl (dset, it->second, _row_sel, 
        _raw_array_as_atomic, _buff, it->first);
    return {true, fl.sel_n_rows()};
}

_HIPP_TEMPRET _read(record_t *buff, hsize_t n, Group &dgrp) const -> void {
    if( _field_sel ) {
        const auto &names = *_field_sel;
        for(auto &name: names)
            _read_field(buff, n, dgrp, _find_field(name));
    }else {
        for(auto it=_fields.cbegin(); it != _fields.cend(); ++it)
            _read_field(buff, n, dgrp, it);
    }
}

_HIPP_TEMPRET _read_field(record_t *buff, hsize_t n, Group &dgrp, 
    _field_iter_t it) const -> void
{
    auto dset = dgrp.open_dataset(it->first);
    _FieldLoader fl(dset, it->second, _row_sel, _raw_array_as_atomic, 
        _buff, it->first);
    fl.load(size_t(n), sizeof(record_t), buff);
}

_HIPP_TEMPRET _write_field(const record_t *buff, hsize_t n, Group &dgrp, 
    _field_iter_t it) const -> void
{   
    auto dset = it->second.create_dataset(n, dgrp, it->first, 
        _dataset_create_flag, _raw_array_as_atomic);
    _FieldLoader fl(dset, it->second, _row_sel, _raw_array_as_atomic,
        _buff, it->first);
    fl.dump(buff, size_t(n), sizeof(record_t));
}

_HIPP_TEMPRET _get_compound_dtype() const -> Datatype {
    Datatype dtype {nullptr};
    if( _field_sel ) {
        auto &names = *_field_sel;
        if ( names.empty() ) return dtype;
        dtype = Datatype::create_compound(sizeof(record_t));
        for(auto &name: names)
            _find_field(name)->second.insert_datatype(name, dtype);
    } else {
        if( _fields.empty() ) return dtype;
        dtype = Datatype::create_compound(sizeof(record_t));
        for(auto &[name, f]: _fields)
            f.insert_datatype(name, dtype);
    }
    return dtype;
}

_HIPP_TEMPRET _get_compound_filespace(Dataset &src_dset, 
    const string &dset_name) const 
-> std::pair<hsize_t, Dataspace> 
{
    auto dims = src_dset.dataspace().dims();
    size_t ndims = dims.ndims();
    if( ndims != size_t(1) )
        ErrLogic::throw_(ErrLogic::eINVALIDARG, emFLPFB, 
        "  ... compound dataset ", dset_name, 
        " must have rank 1 (got", ndims, ")\n");
    hsize_t n_rows = dims[0];
    return _FieldLoader::find_sel_n_rows(n_rows, _row_sel, dset_name);
}

_HIPP_TEMPRET
_read_records(record_t *buff, hsize_t n, Dataset &dset, 
        Datatype &dtype, Dataspace &dsp) const -> void
{
    Dataspace mem_sp {n};
    auto xprop = Dataset::create_proplist("xfer");
    xprop.preserve(true);
    dset.read(buff, dtype, mem_sp, dsp, xprop);
}   


#undef _HIPP_TEMPHD
#undef _HIPP_TEMPARG
#undef _HIPP_TEMPCLS
#undef _HIPP_TEMPRET
#undef _HIPP_TEMPNORET


} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5X_TABLE_IMPL_H_