/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] 
 *          _H5Obj, _H5Dataspace, _H5Datatype, _H5Attr, _H5Dataset, _H5File
 *              HDF5 intermediate-level object. 
 *          _H5EStackTempOff - temporarily turn off the error reporting system.
 */ 

#ifndef _HIPPIO_H5_OBJ_RAW_H_
#define _HIPPIO_H5_OBJ_RAW_H_
#include "h5_base.h"
#include "h5_error.h"
#include <cstring>
namespace HIPP{
namespace IO{

class _H5Obj {
public:
    typedef hid_t id_t;
    typedef herr_t err_t;
    typedef htri_t tri_t;
    typedef hsize_t size_t;

    explicit _H5Obj( id_t id = -1 ) noexcept : _obj( id ){}
    ~_H5Obj() noexcept {}

    _H5Obj( const _H5Obj & ) = delete;
    _H5Obj & operator=( const _H5Obj & ) = delete;
    _H5Obj( _H5Obj && ) = delete;
    _H5Obj & operator=( _H5Obj && ) = delete;
    
    id_t raw() const noexcept { return _obj; }
protected:
    id_t _obj;
};    

class _H5Dataspace: public _H5Obj {
public:
    explicit _H5Dataspace( id_t dataspace ) noexcept: _H5Obj( dataspace ){ }
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
    ~_H5Dataspace( ){ ErrH5::check(H5Sclose( _obj ), emFLPFB); }
};

class _H5Datatype: public _H5Obj {
public:
    explicit _H5Datatype( id_t datatype ) noexcept: _H5Obj( datatype ){ }
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

    ~_H5Datatype( ){ ErrH5::check(H5Tclose( _obj ), emFLPFB); }
};

class _H5Attr: public _H5Obj {
public:
    explicit _H5Attr( id_t attr ) noexcept: _H5Obj( attr ){ }
    _H5Attr( id_t loc, const char *name, id_t datatype, id_t dataspace, 
        id_t cprop = H5P_DEFAULT, id_t aprop = H5P_DEFAULT )
    {
        ErrH5::check(
            _obj = H5Acreate( loc, name, datatype, dataspace, cprop, aprop )
            , emFLPFB );
    }
    static id_t create( id_t loc, const char *name, id_t datatype, 
        id_t dataspace, id_t cprop=H5P_DEFAULT, id_t aprop=H5P_DEFAULT )
    {
        id_t attr = H5Acreate( loc, name, datatype, dataspace, cprop, aprop );
        ErrH5::check(
            attr, emFLPFB, "  ... cannot create attribute named ", 
            name, "\n");
        return attr;
    }
    static id_t open( id_t loc, const char *name, id_t aprop=H5P_DEFAULT ){
        id_t attr = H5Aopen( loc, name, aprop );
        ErrH5::check(
            attr, emFLPFB, "  ... cannot open attribute named ", name, "\n");
        return attr;
    }
    id_t get_space() const{
        id_t dspace = H5Aget_space( _obj );
        ErrH5::check( dspace );
        return dspace;
    }
    id_t get_type() const{
        id_t dtype = H5Aget_type( _obj );
        ErrH5::check( dtype );
        return dtype;
    }
    static tri_t exists( id_t id, const char *name ){
        tri_t ret = H5Aexists( id, name );
        ErrH5::check(ret, emFLPFB);
        return ret;
    }
    void write( id_t memtype, const void *buff )
    {
        ErrH5::check( H5Awrite( _obj, memtype, buff ), emFLPFB );
    }
    void read( id_t memtype, void *buff )
    {
        ErrH5::check( H5Aread( _obj, memtype, buff ), emFLPFB );
    }
    ~_H5Attr( ){ ErrH5::check(H5Aclose( _obj ), emFLPFB); }
};

class _H5Dataset: public _H5Obj {
public:
    explicit _H5Dataset( id_t dset ) noexcept: _H5Obj( dset ){ }
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
    ~_H5Dataset( ){ ErrH5::check(H5Dclose( _obj ), emFLPFB); }
};

class _H5File: public _H5Obj {
public:
    explicit _H5File( id_t file ) noexcept: _H5Obj( file ){ }
    /**
     * create a new file or open an existing file
     * @name:   file name
     * @flag:   "w" for create and truncate, opened as R/W mode.
     *          "r" for open existing file as R mode.
     *          "a" for open existing file as W mode.
     * @cprop, aprop:   
     *          creation and access property lists. If open existing one, cprop
     *          is ignored.
     */
    _H5File( const char *name, const string &flag, 
        id_t cprop = H5P_DEFAULT, id_t aprop = H5P_DEFAULT )
    {
        if( flag == "w" ){
            _obj = H5Fcreate( name, H5F_ACC_TRUNC, cprop, aprop );
        }else if( flag == "r" ){
            _obj = H5Fopen( name, H5F_ACC_RDONLY, aprop);
        }else if ( flag == "a" ){
            _obj = H5Fopen( name, H5F_ACC_RDWR, aprop);
        }else{
            ErrLogic::throw_( ErrLogic::eINVALIDARG, emFLPFB, 
                "\n  ... invalid flag ", flag );
        }
        ErrH5::check( _obj, emFLPFB );
    }

    id_t create_dataset( const char *name, id_t datatype, id_t dataspace, 
        id_t lcprop = H5P_DEFAULT, id_t dcprop = H5P_DEFAULT, 
        id_t aprop = H5P_DEFAULT )
    {
        return _H5Dataset::create(_obj, name, datatype, dataspace, lcprop, 
            dcprop, aprop );
    }
    id_t open_dataset( const char *name, id_t aprop = H5P_DEFAULT ){
        return _H5Dataset::open( _obj, name, aprop );
    }
    ~_H5File( ){ ErrH5::check(H5Fclose( _obj ), emFLPFB); }
};

class _H5EStackTempOff: public _H5Obj {
public:
    explicit _H5EStackTempOff( id_t estack, ErrH5::flag_t flag=0 ) noexcept {
        _obj = estack;
        H5Eget_auto( _obj, &_old_efunc, &_old_edata );
        H5Eset_auto( _obj, NULL, NULL );
        _old_flag = ErrH5::err_cntl_flag();
        ErrH5::err_cntl_flag(flag);
    }
    ~_H5EStackTempOff() noexcept{
        ErrH5::err_cntl_flag(_old_flag);
        H5Eset_auto( _obj, _old_efunc, _old_edata );
    }
private:
    H5E_auto_t _old_efunc = NULL;
    void *_old_edata = NULL;
    ErrH5::flag_t _old_flag = 0;
};


} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_HDF5_OBJ_H_