/**
 * creat: Yangyao CHEN, 2020/11/29
 *      [write   ] 
 *      @_H5Dataspace:   Intermediate-level HDF5 dataspace API.
 */ 
#ifndef _HIPPIO_H5_OBJ_RAW_DATASPACE_H_
#define _HIPPIO_H5_OBJ_RAW_DATASPACE_H_
#include "h5_obj_raw_base.h"
namespace HIPP {
namespace IO {

class _H5Dataspace: public _H5Obj {
public:
    typedef H5S_seloper_t seloper_t;
    typedef H5S_class_t class_t;

    static constexpr class_t 
        NULL_C = H5S_NULL, 
        SIMPLE_C = H5S_SIMPLE,
        SCALAR_C = H5S_SCALAR;
        
    explicit _H5Dataspace( id_t dataspace, int state=stFREE) noexcept;
    _H5Dataspace( int rank, const hsize_t *dims, const hsize_t *maxdims=NULL );
    ~_H5Dataspace();

    int get_simple_extent_ndims() const;
    void get_simple_extent_dims(hsize_t dims[], hsize_t maxdims[]) const;
    void select_hyperslab( seloper_t op, 
        const hsize_t *start, const hsize_t *stride,
        const hsize_t *count, const hsize_t *block);
    hssize_t get_select_npoints() const;
    static id_t create( class_t type );
};


inline _H5Dataspace::_H5Dataspace( id_t dataspace, int state) 
    noexcept: _H5Obj( dataspace, state ){ }
inline _H5Dataspace::_H5Dataspace( int rank, const hsize_t *dims, 
    const hsize_t *maxdims )
{
    ErrH5::check( 
        _obj = H5Screate_simple( rank, dims, maxdims ), emFLPFB );
}
inline _H5Dataspace::~_H5Dataspace(){ 
    if(_state & stFREE) 
        ErrH5::check(H5Sclose( _obj ), emFLPFB); 
}
inline int _H5Dataspace::get_simple_extent_ndims() const{
    int ndims = H5Sget_simple_extent_ndims( _obj );
    ErrH5::check( ndims, emFLPFB );
    return ndims;
}
inline void _H5Dataspace::get_simple_extent_dims(hsize_t dims[], 
    hsize_t maxdims[]) const
{
    ErrH5::check( H5Sget_simple_extent_dims(_obj, dims, maxdims), emFLPFB );
}
inline void _H5Dataspace::select_hyperslab( seloper_t op, 
    const hsize_t *start, const hsize_t *stride,
    const hsize_t *count, const hsize_t *block){
    ErrH5::check( H5Sselect_hyperslab( _obj, op, start, stride, count, 
        block ), emFLPFB );
}
inline hssize_t _H5Dataspace::get_select_npoints() const {
    hssize_t npoints = H5Sget_select_npoints( _obj );
    ErrH5::check( npoints, emFLPFB );
    return npoints;
}
inline id_t _H5Dataspace::create( class_t type ) {
    id_t dspace = H5Screate(type);
    ErrH5::check( dspace, emFLPFB );
    return dspace;
}



} // namespace IO
} // namespace HIPP
#endif	//_HIPPIO_H5_OBJ_RAW_DATASPACE_H_