#ifndef _HIPPIO_H5_OBJ_RAW_DATASET_H_
#define _HIPPIO_H5_OBJ_RAW_DATASET_H_
#include "h5_obj_raw_base.h"
#include "h5_obj_raw_attr.h"
namespace HIPP{
namespace IO{
class _H5Dataspace: public _H5Obj {
public:
    typedef H5S_seloper_t seloper_t;
    typedef H5S_class_t class_t;

    explicit _H5Dataspace( id_t dataspace, int state=1 ) 
        noexcept: _H5Obj( dataspace, state ){ }
    _H5Dataspace( int rank, const size_t *dims, const size_t *maxdims=NULL ){
        ErrH5::check( 
            _obj = H5Screate_simple( rank, dims, maxdims ), emFLPFB );
    }
    int get_simple_extent_ndims() const{
        int ndims = H5Sget_simple_extent_ndims( _obj );
        ErrH5::check( ndims, emFLPFB );
        return ndims;
    }
    void get_simple_extent_dims(size_t dims[], size_t maxdims[]) const{
        ErrH5::check( H5Sget_simple_extent_dims(_obj, dims, maxdims), emFLPFB );
    }
    void select_hyperslab( seloper_t op, 
        const size_t *start, const size_t *stride,
        const size_t *count, const size_t *block){
        ErrH5::check( H5Sselect_hyperslab( _obj, op, start, stride, count, 
            block ), emFLPFB );
    }
    size_t get_select_npoints() const {
        hssize_t npoints = H5Sget_select_npoints( _obj );
        ErrH5::check( npoints, emFLPFB );
        return size_t(npoints);
    }
    static id_t create( class_t type ) {
        id_t dspace = H5Screate(type);
        ErrH5::check( dspace, emFLPFB );
        return dspace;
    }

    ~_H5Dataspace( ){ if(_state) ErrH5::check(H5Sclose( _obj ), emFLPFB); }
};

class _H5Datatype: public _H5Obj {
public:
    explicit _H5Datatype( id_t datatype, int state = 1 ) 
        noexcept: _H5Obj( datatype, state ){ }
    _H5Datatype( id_t datatype, std::size_t size ){
        ErrH5::check(
            H5Tset_size( datatype, size ), emFLPFB);
        _obj = datatype;
    }
    tri_t equal( hid_t dtype ) const{
        tri_t is_eq = H5Tequal( _obj, dtype );
        ErrH5::check( is_eq, emFLPFB );
        return is_eq;
    }
    size_t get_size() const {
        size_t size = H5Tget_size( _obj );
        if( size == 0 ) ErrH5::throw_( size, emFLPFB );
        return size;
    }

    static id_t copy( id_t dtype ){
        id_t ret = H5Tcopy( dtype );
        ErrH5::check( ret, emFLPFB );
        return ret;
    }
    void set_size( size_t size ){
        ErrH5::check( H5Tset_size( _obj, size ), emFLPFB );
    }

    ~_H5Datatype( ){ if(_state) ErrH5::check(H5Tclose( _obj ), emFLPFB); }
};

class _H5Dataset: public _H5Obj {
public:
    explicit _H5Dataset( id_t dset, int state = 1 ) 
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
    ~_H5Dataset( ){ if(_state) ErrH5::check(H5Dclose( _obj ), emFLPFB); }
};


}
}
#endif	//_HIPPIO_H5_OBJ_RAW_DATASET_H_