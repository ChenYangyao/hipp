/**
 * creat: Yangyao CHEN, 2020/11/29
 *      [write   ]
 *      @_H5XTable_helper: helper defs for H5XTable. Internally used only.
 */ 

#ifndef _HIPPIO_H5X_TABLE_HELPER_H_
#define _HIPPIO_H5X_TABLE_HELPER_H_
#include "h5_obj.h"
namespace HIPP {
namespace IO{
template<typename record_t>  
struct _H5XTable_helper {

typedef vector<char> _buff_t;

/**
 * Each Field instance is responsible for IO of an attribute of record_t.
 * The two subclasses - ScalarField and ArrayField, deal with scalar/array 
 * attributes, respectively.
 */
struct Field {
    _buff_t *_pbuff;
    
    Field( _buff_t *pbuff ): _pbuff(pbuff){}
    virtual ~Field() noexcept {}

    virtual size_t mem_size( size_t n ) const =0;
    virtual H5Dataset create_or_open(H5Group &h5g, 
        const string &name, size_t n) const=0;
    virtual void read(H5Dataset &dset, record_t *buff, size_t n) =0;
    virtual void write(H5Dataset &dset, const record_t *buff, size_t n) =0;
    virtual void insert_comp(const string &name, H5Datatype &dtype) =0;

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
    
    ScalarField(pfield_t p, _buff_t *pbuff): Field(pbuff), _p(p){}
    virtual ~ScalarField() noexcept {}

    size_t mem_size( size_t n ) const override {
        return n;
    }
    H5Dataset create_or_open(H5Group &h5g, 
        const string &name, size_t n) const override {
        vector<hsize_t> dims = { n };
        return h5g.create_dataset<field_t>(name, dims);
    }
    void read(H5Dataset &dset, record_t *buff, size_t n)  override {
        field_t *tempbuff = this->template get_buff<field_t>(n);
        dset.read(tempbuff);
        for(size_t i=0; i<n; ++i)
            buff[i].*_p = tempbuff[i];
    }
    void write(H5Dataset &dset, const record_t *buff, size_t n)  override{
        field_t *tempbuff = this->template get_buff<field_t>(n);
        for(size_t i=0; i<n; ++i)
            tempbuff[i] = buff[i].*_p;
        dset.write(tempbuff);
    }
    void insert_comp(const string &name, H5Datatype &dtype) override {
        dtype.insert(name, _p);
    }
};

template<typename field_t>
struct ArrayField: public Field {
    typedef field_t record_t::*pfield_t;
    typedef std::remove_all_extents_t<field_t> scalar_t;

    static constexpr size_t n_scalar = sizeof(field_t)/sizeof(scalar_t);
    pfield_t _p;

    ArrayField(pfield_t p, _buff_t *pbuff): Field(pbuff), _p(p){}
    virtual ~ArrayField() noexcept {}
    size_t mem_size( size_t n ) const override {
        return n*n_scalar;
    }
    H5Dataset create_or_open(H5Group &h5g, 
        const string &name, size_t n) const override {
        vector<hsize_t> dims = {n};
        auto idims = get_dims();
        dims.insert(dims.end(), idims.begin(), idims.end());
        return h5g.create_dataset<scalar_t>(name, dims);
    }
    void read(H5Dataset &dset, record_t *buff, size_t n)  override {
        scalar_t *tempbuff = this->template get_buff<scalar_t>(mem_size(n));
        dset.read(tempbuff);
        for(size_t i=0; i<n; ++i){
            scalar_t *pmem = reinterpret_cast<scalar_t *>(buff[i].*_p);
            for(size_t j=0; j<n_scalar; ++j)
                pmem[j] = *tempbuff++;
        }
    }
    void write(H5Dataset &dset, const record_t *buff, size_t n)  override {
        scalar_t *tempbuff = this->template get_buff<scalar_t>(mem_size(n)),
            *b = tempbuff;
        for(size_t i=0; i<n; ++i){
            auto pmem = reinterpret_cast<const scalar_t *>(buff[i].*_p);
            for(size_t j=0; j<n_scalar; ++j)
                *b++ = pmem[j];
        }
        dset.write(tempbuff);
    }
    void insert_comp(const string &name, H5Datatype &dtype) override {
        dtype.insert(name, _p);
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

};


} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5X_TABLE_HELPER_H_