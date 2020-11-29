/**
 * creat: Yangyao CHEN, 2020/11/29
 *      [write   ] 
 *      @_H5File:   Intermediate-level HDF5 file API.
 */ 
#ifndef _HIPPIO_H5_OBJ_RAW_FILE_H_
#define _HIPPIO_H5_OBJ_RAW_FILE_H_
#include "h5_obj_raw_base.h"
#include "h5_obj_raw_attr.h"
#include "h5_obj_raw_dataset.h"
namespace HIPP {
namespace IO {

class _H5File: public _H5Obj {
public:
    explicit _H5File( id_t file, int state=stFREE ) 
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
        id_t cprop = H5P_DEFAULT, id_t aprop = H5P_DEFAULT );
    ~_H5File( );

    id_t create_dataset( const char *name, id_t datatype, id_t dataspace, 
        id_t lcprop = H5P_DEFAULT, id_t dcprop = H5P_DEFAULT, 
        id_t aprop = H5P_DEFAULT );
    id_t open_dataset( const char *name, id_t aprop = H5P_DEFAULT );
    id_t create_attr( const char *name, id_t datatype, id_t dataspace, 
        id_t cprop = H5P_DEFAULT, id_t aprop=H5P_DEFAULT );
    id_t open_attr( const char *name, id_t aprop=H5P_DEFAULT );
    tri_t exist_attr( const char *name )const;
};


inline _H5File::_H5File( const char *name, const string &flag, id_t cprop, 
    id_t aprop )
{
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
inline id_t _H5File::create_dataset( const char *name, id_t datatype, 
    id_t dataspace, id_t lcprop, id_t dcprop, id_t aprop )
{
    return _H5Dataset::create(_obj, name, datatype, dataspace, lcprop, 
        dcprop, aprop );
}
inline id_t _H5File::open_dataset( const char *name, id_t aprop ){
    return _H5Dataset::open( _obj, name, aprop );
}
inline id_t _H5File::create_attr( const char *name, id_t datatype, 
    id_t dataspace, id_t cprop, id_t aprop)
{
    return _H5Attr::create( _obj, name, datatype, dataspace, cprop, aprop );
}
inline id_t _H5File::open_attr( const char *name, id_t aprop ){
    return _H5Attr::open(_obj, name, aprop);
}
inline tri_t _H5File::exist_attr( const char *name )const {
    return _H5Attr::exists( _obj, name );
}
inline _H5File::~_H5File( ){ 
    if(_state & stFREE) 
        ErrH5::check(H5Fclose( _obj ), emFLPFB); 
}

} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_RAW_FILE_H_