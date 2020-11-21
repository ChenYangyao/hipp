#ifndef _HIPPIO_H5X_TABLE_H_
#define _HIPPIO_H5X_TABLE_H_
#include <hippio.h>
#include "h5_obj.h"
namespace HIPP {
namespace IO {

template<typename record_t>
class H5XTable {
public:
    typedef std::size_t size_t;
    typedef vector<record_t> table_t;

    template<typename ...Args>
    H5XTable( Args &&...args );
        
    table_t read(H5Group dgrp);
    void read(table_t &tbl, H5Group dgrp);
    table_t read(H5File file);
    table_t read(const string &file_name);

    void write(const table_t &tbl, H5Group dgrp);
    void write(const table_t &tbl, H5File file);
    void write(const table_t &tbl, const string &file_name, 
        const string flag="w");
private:
    typedef vector<char> buff_t;
    struct Field {
        Field( buff_t *pbuff ): _pbuff(pbuff){}
        buff_t *_pbuff;

        virtual size_t mem_size( const table_t &tbl ) const =0;
        virtual H5Dataset create_or_open(H5Group &h5g, 
            const string &name, const table_t &tbl) const=0;
        virtual void read(H5Dataset &dset, table_t &tbl) =0;
        virtual void write(H5Dataset &dset, const table_t &tbl) =0;

        template<typename scalar_t>
        scalar_t *get_buff(size_t n){
            size_t size_need = sizeof(scalar_t)*n;
            if( size_need > _pbuff->size() ) 
                _pbuff->resize(size_need);
            return reinterpret_cast<scalar_t *>(_pbuff->data());
        }
    };
    template<typename field_t>
    struct ScalarField: public Field {
        typedef field_t record_t::*pfield_t;
        pfield_t _p;
        
        ScalarField(pfield_t p, buff_t *pbuff): Field(pbuff), _p(p){}
        size_t mem_size( const table_t &tbl ) const override {
            return tbl.size();
        }
        H5Dataset create_or_open(H5Group &h5g, 
            const string &name, const table_t &tbl) const override {
            vector<hsize_t> dims = { tbl.size() };
            return h5g.create_dataset<field_t>(name, dims);
        }
        void read(H5Dataset &dset, table_t &tbl)  override {
            field_t *buff = this->template get_buff<field_t>( tbl.size() );
            dset.read(buff);
            for(size_t i=0; i<tbl.size(); ++i)
                tbl[i].*_p = buff[i];
        }
        void write(H5Dataset &dset, const table_t &tbl)  override{
            field_t *buff = this->template get_buff<field_t>( tbl.size() );
            for(size_t i=0; i<tbl.size(); ++i)
                buff[i] = tbl[i].*_p;
            dset.write(buff);
        }
    };
    template<typename field_t>
    struct ArrayField: public Field {
        typedef field_t record_t::*pfield_t;
        typedef std::remove_all_extents_t<field_t> scalar_t;
        static constexpr size_t n_scalar = sizeof(field_t)/sizeof(scalar_t);
        pfield_t _p;

        ArrayField(pfield_t p, buff_t *pbuff): Field(pbuff), _p(p){}
        size_t mem_size( const table_t &tbl ) const override {
            return tbl.size() * n_scalar;
        }
        H5Dataset create_or_open(H5Group &h5g, 
            const string &name, const table_t &tbl) const override {
            vector<hsize_t> dims = { tbl.size() };
            auto idims = get_dims();
            dims.insert(dims.end(), idims.begin(), idims.end());
            return h5g.create_dataset<scalar_t>( name, dims );
        }
        void read(H5Dataset &dset, table_t &tbl)  override {
            scalar_t *buff = this->template get_buff<scalar_t>(mem_size(tbl));
            dset.read(buff);
            for(size_t i=0; i<tbl.size(); ++i){
                scalar_t *pmem = reinterpret_cast<scalar_t *>(tbl[i].*_p);
                for(size_t j=0; j<n_scalar; ++j)
                    pmem[j] = *buff++;
            }
        }
        void write(H5Dataset &dset, const table_t &tbl)  override {
            scalar_t *buff = this->template get_buff<scalar_t>(mem_size(tbl)),
                *b = buff;
            for(size_t i=0; i<tbl.size(); ++i){
                auto pmem = reinterpret_cast<const scalar_t *>(tbl[i].*_p);
                for(size_t j=0; j<n_scalar; ++j)
                    *b++ = pmem[j];
            }
            dset.write(buff);
        }
        template<typename array_t>
        static void push_dims( const array_t &arr, vector<hsize_t> &dims ) {
            constexpr size_t n_elem = sizeof(array_t)/sizeof(scalar_t);
            if constexpr( n_elem > 1 ){
                dims.push_back( n_elem );
                push_dims( arr[0], dims );
            }
        }
        static vector<hsize_t> get_dims(){
            field_t tempvar {};
            vector<hsize_t> dims;
            push_dims(tempvar, dims);
            for(size_t i=0; i+1<dims.size(); ++i)
                dims[i] /= dims[i+1];
            return dims;
        }
    };

    std::unordered_map<string, std::unique_ptr<Field> > _fields;
    buff_t _buff;

    template<typename field_t, typename ...Args>
    void _push_fields( const string &name, field_t record_t::*p, Args &&...args ){
        if constexpr( std::is_array_v<field_t> )
            _fields.emplace(name, new ArrayField<field_t>(p, &_buff));
        else 
            _fields.emplace(name, new ScalarField<field_t>(p, &_buff));
        if constexpr( sizeof...(Args) > 0 )
            _push_fields(std::forward<Args>(args)...);
    }
    void _chk_size(size_t mem_size, size_t file_size, const string &info){
        if( mem_size != file_size )
            HIPP::ErrLogic::throw_(HIPP::ErrLogic::eLENGTH, emFLPFB, 
            "  ... ", info, 
            " (mem size=", mem_size, ", file_size=", file_size, ")\n");
    }
    void _read(table_t &tbl, H5Group dgrp){
        if( _fields.empty() ){ 
            tbl.clear();
            return;
        } 
        std::size_t n_obj = dgrp.open_dataset( _fields.begin()->first )
            .dataspace().dims().at(0);
        tbl.resize(n_obj);

        for(auto &[name, field]: _fields){
            H5Dataset dset = dgrp.open_dataset(name);
            _chk_size( field->mem_size(tbl), dset.dataspace().size(), 
                name+" load failed" );
            field->read(dset, tbl);
        }
    }
    void _write(const table_t &tbl, H5Group dgrp) {
        for(auto &[name, field]: _fields){
            H5Dataset dset = field->create_or_open(dgrp, name, tbl);
            _chk_size( field->mem_size(tbl), dset.dataspace().size(),
                name+" store faild" );
            field->write(dset, tbl);
        }
    }
};

template<typename record_t>
template<typename ...Args>
H5XTable<record_t>::H5XTable( Args &&...args )
    { _push_fields( std::forward<Args>(args)... ); }

template<typename record_t>
auto H5XTable<record_t>::read(H5Group dgrp) -> table_t
    { table_t tbl; read(tbl, dgrp); return tbl; }

template<typename record_t>
void H5XTable<record_t>::read(table_t &tbl, H5Group dgrp)
    { _read(tbl, dgrp); }

template<typename record_t>
auto H5XTable<record_t>::read(H5File file) ->table_t 
    { return read(file.open_group("/")); }

template<typename record_t>
auto H5XTable<record_t>::read(const string &file_name) -> table_t
    { return read(H5File(file_name, "r")); }

template<typename record_t>
void H5XTable<record_t>::write(const table_t &tbl, H5Group dgrp)
    { _write(tbl, dgrp); }

template<typename record_t>
void H5XTable<record_t>::write(const table_t &tbl, H5File file)
    { write(tbl, file.open_group("/")); }

template<typename record_t>
void H5XTable<record_t>::write(const table_t &tbl, const string &file_name, 
    const string flag)
    { write(tbl, H5File(file_name, flag)); }

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5X_TABLE_H_