/**
 * creat: Yangyao CHEN, 2020/11/28
 *      [write   ] 
 *      @H5XTable: high-level tabular data manipulator.
 */ 

#ifndef _HIPPIO_H5X_TABLE_H_
#define _HIPPIO_H5X_TABLE_H_
#include "h5x_table_helper.h"
#include <optional>
namespace HIPP {
namespace IO {
    
template<typename record_t>
class H5XTable {
public:
    typedef vector<record_t> table_t;

    template<typename ...Args>
    H5XTable( Args &&...args );

    /**
     * Safe for operations. 
     * Copy forbidden.
     */
    H5XTable(const H5XTable &) =delete;
    H5XTable & operator=(const H5XTable &) =delete;
    H5XTable(H5XTable &&) noexcept;
    H5XTable & operator=(H5XTable &&) noexcept;
    ~H5XTable() noexcept {}

    /** 
     * Fields manipulator, acts like a unordered_map.
     */
    template<typename field_t>
    H5XTable & add_field(const string &name, field_t record_t::*p);
    bool remove_field(const string &name); 
    bool has_field(const string &name);
    size_t n_fields() const noexcept;
    bool empty() const noexcept;

    /** 
     * I/O as separate dataset for each field.
     */
    table_t read(H5Group dgrp);
    table_t read(H5File file);
    table_t read(const string &file_name);
    void read(table_t &tbl, H5Group dgrp);
    void read(record_t *buff, size_t n, H5Group dgrp);

    void write(const table_t &tbl, H5Group dgrp);
    void write(const table_t &tbl, H5File file);
    void write(const table_t &tbl, const string &file_name, 
        const string flag="w");
    void write(const record_t *buff, size_t n, H5Group dgrp);

    /** 
     * I/O as a single dataset, i.e. using compound datatype for the row/record.
     */
    table_t read_records(H5Group dgrp, const string &dset);
    table_t read_records(H5File file, const string &dset);
    table_t read_records(const string &file_name, const string &dset);
    void read_records(table_t &tbl, H5Group dgrp, const string &dset);
    void read_records(record_t *buff, size_t n, 
        H5Group dgrp, const string &dset);

    void write_records(const table_t &tbl, H5Group dgrp, const string &dset);
    void write_records(const table_t &tbl, H5File file, const string &dset);
    void write_records(const table_t &tbl, const string &file_name, 
        const string &dset, const string flag="w");
    void write_records(const record_t *buff, size_t n,
         H5Group dgrp, const string &dset);
private:
    typedef _H5XTable_helper<record_t> _hp_t;
    using _buff_t = typename _hp_t::_buff_t;
    using Field =  typename _hp_t::Field;

    std::unordered_map<string, std::unique_ptr<Field> > _fields;
    _buff_t _buff;
    std::optional<H5Datatype> _comp_dtype;

    template<typename field_t, typename ...Args>
    void _push_fields( const string &name, 
        field_t record_t::*p, Args &&...args )
    {
        typedef typename _hp_t::template ScalarField<field_t> ScalarField;
        typedef typename _hp_t::template ArrayField<field_t> ArrayField;

        if constexpr( std::is_array_v<field_t> )
            _fields.emplace(name, 
                new ArrayField(p, &_buff));
        else 
            _fields.emplace(name, 
                new ScalarField(p, &_buff));
        if constexpr( sizeof...(Args) > 0 )
            _push_fields(std::forward<Args>(args)...);
    }
    void _chk_size(size_t mem_size, size_t file_size, const string &info){
        if( mem_size != file_size )
            HIPP::ErrLogic::throw_(HIPP::ErrLogic::eLENGTH, emFLPFB, 
            "  ... ", info, 
            " (mem size=", mem_size, ", file_size=", file_size, ")\n");
    }
    void _chk_and_set_comp_dtype(){
        if( !_comp_dtype.has_value() ){
            _comp_dtype = H5Datatype::create(
                H5Datatype::COMPOUND_C, sizeof(record_t));
            for(auto &[name, f]: _fields){
                f->insert_comp(name, *_comp_dtype);
            }
        }
    }
    void _read(table_t &tbl, H5Group dgrp){
        if( _fields.empty() ){ 
            tbl.clear();
            return;
        } 
        std::size_t n_obj = dgrp.open_dataset( _fields.begin()->first )
            .dataspace().dims().at(0);
        tbl.resize(n_obj);
        _read(tbl.data(), n_obj, dgrp);
    }
    void _read(record_t *buff, size_t n, H5Group dgrp){
        for(auto &[name, field]: _fields){
            H5Dataset dset = dgrp.open_dataset(name);
            _chk_size(field->mem_size(n), dset.dataspace().size(), 
                name+" load failed" );
            field->read(dset, buff, n);
        }
    }
    void _write(const table_t &tbl, H5Group dgrp) {
        _write(tbl.data(), tbl.size(), dgrp);
    }
    void _write(const record_t *buff, size_t n, H5Group dgrp) {
        for(auto &[name, field]: _fields){
            H5Dataset dset = field->create_or_open(dgrp, name, n);
            _chk_size(field->mem_size(n), dset.dataspace().size(),
                name+" store faild" );
            field->write(dset, buff, n);
        }
    }
    void _read_records(table_t &tbl, H5Group dgrp, const string &name){
        size_t n = dgrp.open_dataset(name).dataspace().dims().at(0);
        tbl.resize(n);
        _read_records(tbl.data(), n, dgrp, name);
    }
    void _read_records(record_t *buff, size_t n, H5Group dgrp, 
        const string &name)
    {
        _chk_and_set_comp_dtype();

        auto dset = dgrp.open_dataset(name);
        _chk_size(n, dset.dataspace().dims().at(0), name+" load failed");
        
        dset.read(buff, *_comp_dtype);
    }
    void _write_records(const table_t &tbl, H5Group dgrp, const string &name){
        _write_records(tbl.data(), tbl.size(), dgrp, name);
    }
    void _write_records(const record_t *buff, size_t n, H5Group dgrp, 
        const string &name)
    {
        _chk_and_set_comp_dtype();
        H5Dataset dset = dgrp.create_dataset(name, *_comp_dtype, {n});   
        dset.write(buff, *_comp_dtype);
    }
};

template<typename record_t>
template<typename ...Args>
H5XTable<record_t>::H5XTable( Args &&...args )
: _comp_dtype(std::nullopt)
{ 
    _push_fields( std::forward<Args>(args)... ); 
}

template<typename record_t>
H5XTable<record_t>::H5XTable(H5XTable &&that) noexcept 
: _fields(std::move(that._fields)), _buff(std::move(that._buff)),
_comp_dtype(std::move(that._comp_dtype))
{}

template<typename record_t>
H5XTable<record_t> & H5XTable<record_t>::operator=(H5XTable &&that) noexcept 
{
    _buff = std::move(that._buff);
    _fields = std::move(that._fields);
    _comp_dtype = std::move(that._comp_dtype);
    return *this;
}
template<typename record_t>
template<typename field_t>
H5XTable<record_t> & H5XTable<record_t>::add_field(const string &name, 
    field_t record_t::*p) {
    _push_fields(name, p);
    _comp_dtype.reset();
    return *this;
}
template<typename record_t>
bool H5XTable<record_t>::remove_field(const string &name) {
    bool ret = _fields.erase(name) == 1;
    _comp_dtype.reset();
    return ret;
}
template<typename record_t>
bool H5XTable<record_t>::has_field(const string &name){
    return _fields.find(name) != _fields.end();
}
template<typename record_t>
size_t H5XTable<record_t>::n_fields() const noexcept{
    return _fields.size();
}
template<typename record_t>
bool H5XTable<record_t>::empty() const noexcept{
    return _fields.empty();
}
template<typename record_t>
auto H5XTable<record_t>::read(H5Group dgrp) -> table_t { 
    table_t tbl; read(tbl, dgrp); return tbl; 
}
template<typename record_t>
auto H5XTable<record_t>::read(H5File file) ->table_t { 
    return read(file.open_group("/")); 
}
template<typename record_t>
auto H5XTable<record_t>::read(const string &file_name) -> table_t { 
    return read(H5File(file_name, "r"));
}
template<typename record_t>
void H5XTable<record_t>::read(table_t &tbl, H5Group dgrp){ 
    _read(tbl, dgrp); 
}
template<typename record_t>
void H5XTable<record_t>::read(record_t *buff, size_t n, H5Group dgrp){
    _read(buff, n, dgrp);
}
template<typename record_t>
void H5XTable<record_t>::write(const table_t &tbl, H5Group dgrp) { 
    _write(tbl, dgrp); 
}
template<typename record_t>
void H5XTable<record_t>::write(const table_t &tbl, H5File file) { 
    write(tbl, file.open_group("/")); 
}
template<typename record_t>
void H5XTable<record_t>::write(const table_t &tbl, const string &file_name, 
    const string flag){ 
    write(tbl, H5File(file_name, flag)); 
}
template<typename record_t>
void H5XTable<record_t>::write(const record_t *buff, size_t n, H5Group dgrp){
    _write(buff, n, dgrp);
}
template<typename record_t>
auto H5XTable<record_t>::read_records(H5Group dgrp, const string &dset)
    -> table_t 
{
    table_t tbl;
    _read_records(tbl, dgrp, dset);
    return tbl;
}
template<typename record_t>
auto H5XTable<record_t>::read_records(H5File file, const string &dset) 
-> table_t 
{
    return read_records(file.open_group("/"), dset);
}
template<typename record_t>
auto H5XTable<record_t>::read_records(const string &file_name, 
    const string &dset) -> table_t 
{
    return read_records(H5File(file_name, "r"), dset);
}
template<typename record_t>
void H5XTable<record_t>::read_records(table_t &tbl, 
    H5Group dgrp, const string &dset) 
{
    _read_records(tbl, dgrp, dset);
}
template<typename record_t>
void H5XTable<record_t>::read_records(record_t *buff, size_t n, 
    H5Group dgrp, const string &dset) {
    _read_records(buff, n, dgrp, dset);
}
template<typename record_t>
void H5XTable<record_t>::write_records(const table_t &tbl, 
    H5Group dgrp, const string &dset) 
{
    _write_records(tbl, dgrp, dset);
}
template<typename record_t>
void H5XTable<record_t>::write_records(const table_t &tbl, 
    H5File file, const string &dset) 
{
    write_records(tbl, file.open_group("/"), dset);
}
template<typename record_t>
void H5XTable<record_t>::write_records(const table_t &tbl, 
    const string &file_name, const string &dset, const string flag) 
{
    write_records(tbl, H5File(file_name, flag), dset);
}
template<typename record_t>
void H5XTable<record_t>::write_records(const record_t *buff, size_t n,
    H5Group dgrp, const string &dset) 
{
    _write_records(buff, n, dgrp, dset);
}

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5X_TABLE_H_