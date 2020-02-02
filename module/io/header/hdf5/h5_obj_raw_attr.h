#ifndef _HIPPIO_H5_OBJ_RAW_ATTR_H_
#define _HIPPIO_H5_OBJ_RAW_ATTR_H_
#include "h5_obj_raw_base.h"
namespace HIPP{
namespace IO{
class _H5Attr: public _H5Obj {
public:
    explicit _H5Attr( id_t attr, int state=1 ) noexcept: 
        _H5Obj( attr, state ){ }
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
    ~_H5Attr( ){ if(_state) ErrH5::check(H5Aclose( _obj ), emFLPFB); }
};
}
}
#endif	//_HIPPIO_H5_OBJ_RAW_ATTR_H_