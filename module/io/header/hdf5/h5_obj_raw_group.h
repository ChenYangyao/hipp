/**
 * creat: Yangyao CHEN, 2020/11/29
 *      [write   ] 
 *      @_H5Group:   Intermediate-level HDF5 group API.
 */ 
#ifndef _HIPPIO_H5_OBJ_RAW_GROUP_H_
#define _HIPPIO_H5_OBJ_RAW_GROUP_H_
#include "h5_obj_raw_base.h"
#include "h5_obj_raw_attr.h"
#include "h5_obj_raw_dataset.h"
namespace HIPP {
namespace IO {
class _H5Group: public _H5Obj {
public:
    typedef H5G_storage_type_t storage_type_t;
    static constexpr storage_type_t 
        STORAGE_TYPE_COMPACT = H5G_STORAGE_TYPE_COMPACT,
        STORAGE_TYPE_DENSE = H5G_STORAGE_TYPE_DENSE,
        STORAGE_TYPE_SYMBOL_TABLE = H5G_STORAGE_TYPE_SYMBOL_TABLE;

    typedef H5G_info_t info_t;

    explicit _H5Group( id_t group, int state=stFREE ) 
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
    void get_info(info_t &info) const {
        ErrH5::check(H5Gget_info(_obj, &info), emFLPFB);
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
    ~_H5Group(){ if(_state & stFREE) ErrH5::check( H5Gclose( _obj ), emFLPFB );}
};

} // namespace IO
} // namespace HIPP

#endif	//_HIPPIO_H5_OBJ_RAW_GROUP_H_