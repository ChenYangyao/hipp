/**
 * creat: Yangyao CHEN, 2020/11/29
 *      [write   ] 
 *      @_H5Dataset:   Intermediate-level HDF5 dataset API.
 */ 
#ifndef _HIPPIO_H5_OBJ_RAW_DATASET_H_
#define _HIPPIO_H5_OBJ_RAW_DATASET_H_
#include "h5_obj_raw_base.h"
#include "h5_obj_raw_attr.h"
namespace HIPP{
namespace IO{

class _H5Dataset: public _H5Obj {
public:
    explicit _H5Dataset( id_t dset, int state=stFREE ) 
        noexcept: _H5Obj( dset, state ){ }
    _H5Dataset( id_t loc, const char *name, id_t datatype, id_t dataspace,
        id_t lcprop = H5P_DEFAULT, id_t dcprop = H5P_DEFAULT, 
        id_t aprop = H5P_DEFAULT )
    {
        _obj = create( loc, name, datatype, dataspace, lcprop, dcprop, aprop );
    }
    static id_t create(id_t loc, const char *name, id_t datatype, 
        id_t dataspace, id_t lcprop = H5P_DEFAULT, 
        id_t dcprop = H5P_DEFAULT, 
        id_t aprop = H5P_DEFAULT)
    {
        id_t dset = H5Dcreate( loc, name, datatype, dataspace, lcprop, 
            dcprop, aprop );
        ErrH5::check( dset, 
            emFLPFB, "  ... cannot create dataset named ", name, "\n" );
        return dset;
    }
    static id_t open(id_t loc, const char *name, id_t aprop = H5P_DEFAULT){
        id_t dset = H5Dopen( loc, name, aprop );
        ErrH5::check( dset, emFLPFB, "  ... cannot open dataset named ", 
            name, "\n" );
        return dset;
    }
    id_t get_space() const{
        id_t dspace = H5Dget_space( _obj );
        ErrH5::check( dspace );
        return dspace;
    }
    id_t get_type() const{
        id_t dtype = H5Dget_type( _obj );
        ErrH5::check( dtype );
        return dtype;
    }
    void write( id_t memtype, id_t memspace, id_t filespace, 
        id_t tprop, const void *buff )
    {
        ErrH5::check( H5Dwrite( _obj, memtype, memspace, filespace, 
            tprop, buff ), emFLPFB );
    }
    void read( id_t memtype, id_t memspace, id_t filespace, 
        id_t tprop, void *buff )const
    {
        ErrH5::check( H5Dread( _obj, memtype, memspace, filespace, 
            tprop, buff ), emFLPFB );
    }
    id_t create_attr( const char *name, id_t datatype, id_t dataspace, 
        id_t cprop = H5P_DEFAULT, id_t aprop=H5P_DEFAULT )
    {
        return _H5Attr::create(_obj, name, datatype, dataspace, cprop, aprop);
    }    
    id_t open_attr( const char *name, id_t aprop=H5P_DEFAULT){
        return _H5Attr::open(_obj, name, aprop);
    }
    tri_t exist_attr( const char *name )const {
        return _H5Attr::exists( _obj, name );
    }
    ~_H5Dataset( ){ 
        if(_state & stFREE) 
            ErrH5::check(H5Dclose( _obj ), emFLPFB); 
    }
};


}
}
#endif	//_HIPPIO_H5_OBJ_RAW_DATASET_H_