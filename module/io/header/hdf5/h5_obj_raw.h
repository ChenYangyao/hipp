/**
 * creat: Yangyao CHEN, 2020/01/11
 *      [write   ] 
 *          _H5Obj, _H5Dataspace, _H5Datatype, _H5Attr, _H5Dataset, _H5File
 *              HDF5 intermediate-level object. 
 *          _H5EStackTempOff - temporarily turn off the error reporting system.
 */ 

#ifndef _HIPPIO_H5_OBJ_RAW_H_
#define _HIPPIO_H5_OBJ_RAW_H_
#include "h5_obj_raw_base.h"
#include "h5_obj_raw_dataset.h"
#include "h5_obj_raw_attr.h"
#include "h5_obj_raw_proplist.h"
namespace HIPP{
namespace IO{

class _H5File: public _H5Obj {
public:
    explicit _H5File( id_t file, int state=1 ) 
        noexcept: _H5Obj( file, state ){ }
    /**
     * create a new file or open an existing file
     * @name:   file name
     * @flag:   "w" - create and truncate, open as R/W mode.
     *          "x" - exclusive create (failed if existing), open as R/W mode.
     *          "r" - open existing file as R mode.
     *          "a" - open existing file as R/W mode.
     * @cprop, aprop:   
     *          creation and access property lists. If open existing one, cprop
     *          is ignored.
     */
    _H5File( const char *name, const string &flag, 
        id_t cprop = H5P_DEFAULT, id_t aprop = H5P_DEFAULT ){
        if( flag == "w" ){
            _obj = H5Fcreate( name, H5F_ACC_TRUNC, cprop, aprop );
        }else if( flag == "x" ){
            _obj = H5Fcreate( name, H5F_ACC_EXCL, cprop, aprop );
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
        id_t aprop = H5P_DEFAULT ){
        return _H5Dataset::create(_obj, name, datatype, dataspace, lcprop, 
            dcprop, aprop );
    }
    id_t open_dataset( const char *name, id_t aprop = H5P_DEFAULT ){
        return _H5Dataset::open( _obj, name, aprop );
    }
    id_t create_attr( const char *name, id_t datatype, id_t dataspace, 
        id_t cprop = H5P_DEFAULT, id_t aprop=H5P_DEFAULT ){
        return _H5Attr::create( _obj, name, datatype, dataspace, cprop, aprop );
    }
    id_t open_attr( const char *name, id_t aprop=H5P_DEFAULT ){
        return _H5Attr::open(_obj, name, aprop);
    }
    tri_t exist_attr( const char *name )const {
        return _H5Attr::exists( _obj, name );
    }
    ~_H5File( ){ if(_state) ErrH5::check(H5Fclose( _obj ), emFLPFB); }
};

class _H5Group: public _H5Obj {
public:
    explicit _H5Group( id_t group, int state=1 ) 
        noexcept: _H5Obj( group, state ) { }
    static id_t create( id_t loc, const char *name, id_t lcprop = H5P_DEFAULT, 
        id_t gcprop = H5P_DEFAULT, id_t aprop = H5P_DEFAULT )
    {
        id_t group = H5Gcreate( loc, name, lcprop, gcprop, aprop );
        ErrH5::check( group, emFLPFB, "  ... cannot create group named ", 
            name, '\n' );
        return group;
    }
    static id_t open( id_t loc, const char *name, id_t aprop = H5P_DEFAULT ){
        id_t group = H5Gopen( loc, name, aprop );
        ErrH5::check(group, "  ... cannot open group named ", group, '\n');
        return group;
    }
    id_t create_dataset( const char *name, id_t datatype, id_t dataspace, 
        id_t lcprop = H5P_DEFAULT, id_t dcprop = H5P_DEFAULT, 
        id_t aprop = H5P_DEFAULT ){
        return _H5Dataset::create(_obj, name, datatype, dataspace, lcprop, 
            dcprop, aprop );
    }
    id_t open_dataset( const char *name, id_t aprop = H5P_DEFAULT ){
        return _H5Dataset::open( _obj, name, aprop );
    }
    id_t create_attr( const char *name, id_t datatype, id_t dataspace, 
        id_t cprop = H5P_DEFAULT, id_t aprop=H5P_DEFAULT ){
        return _H5Attr::create( _obj, name, datatype, dataspace, cprop, aprop );
    }
    id_t open_attr( const char *name, id_t aprop=H5P_DEFAULT ){
        return _H5Attr::open(_obj, name, aprop);
    }
    tri_t exist_attr( const char *name )const {
        return _H5Attr::exists( _obj, name );
    }
    ~_H5Group(){ if(_state) ErrH5::check( H5Gclose( _obj ), emFLPFB );}
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