/**
 * creat: Yangyao CHEN, 2020/11/29
 *      [write   ] 
 *      @_H5Proplist:   Intermediate-level HDF5 property list API.
 */ 
#ifndef _HIPPIO_H5_OBJ_RAW_PROPLIST_H_
#define _HIPPIO_H5_OBJ_RAW_PROPLIST_H_
#include "h5_obj_raw_base.h"
namespace HIPP{
namespace IO{

class _H5Proplist: public _H5Obj{
public:
    typedef H5D_layout_t layout_t;

    explicit _H5Proplist( id_t plist, int state=stFREE ) noexcept;
    ~_H5Proplist();
    
    static id_t create( id_t pclass );
    void set_layout( layout_t layout );
    layout_t get_layout()const;

    void set_chunk( int ndims, const hsize_t *dim );
    int get_chunk_ndims()const;
    vector<hsize_t> get_chunk()const;

    void set_chunk_cache( size_t nslots, size_t nbytes, double w0 );
    void get_chunk_cache( size_t *nslot, size_t *nbytes, double *w0 )const;

    void set_cache( int nelmts, size_t nslots, size_t nbytes, double w0 );
    void get_cache( int *nelmts, size_t *nslots, size_t *nbytes, 
        double *w0 )const;
};


inline _H5Proplist::_H5Proplist( id_t plist, int state ) 
    noexcept : _H5Obj(plist, state){ }

inline id_t _H5Proplist::create( id_t pclass ){
    id_t plist = H5Pcreate( pclass );
    ErrH5::check( plist, emFLPFB );
    return plist;
};
inline void _H5Proplist::set_layout( layout_t layout ){
    ErrH5::check( H5Pset_layout( _obj, layout ), emFLPFB );
}
inline _H5Proplist::layout_t _H5Proplist::get_layout()const{
    layout_t layout = H5Pget_layout( _obj );
    ErrH5::check( layout, emFLPFB );
    return layout;
}
inline void _H5Proplist::set_chunk( int ndims, const hsize_t *dim ){
    ErrH5::check( H5Pset_chunk( _obj, ndims, dim ), emFLPFB );
}
inline int _H5Proplist::get_chunk_ndims()const{
    int ndims = H5Pget_chunk(_obj, 0, NULL);
    ErrH5::check( ndims, emFLPFB );
    return ndims;
}
inline vector<hsize_t> _H5Proplist::get_chunk()const{
    vector<hsize_t> chunk( get_chunk_ndims() );
    ErrH5::check( 
        H5Pget_chunk( _obj, chunk.size(), chunk.data() ), emFLPFB );
    return chunk;
}
inline void _H5Proplist::set_chunk_cache( 
    size_t nslots, size_t nbytes, double w0 )
{
    ErrH5::check( H5Pset_chunk_cache( _obj, nslots, nbytes, w0 ), emFLPFB );
}
inline void _H5Proplist::get_chunk_cache( size_t *nslot, 
    size_t *nbytes, double *w0 )const{
    ErrH5::check( H5Pget_chunk_cache( _obj, nslot, nbytes, w0 ), emFLPFB );
}

inline void _H5Proplist::set_cache( int nelmts, size_t nslots, 
    size_t nbytes, double w0 ){
    ErrH5::check( H5Pset_cache( _obj, nelmts, nslots, nbytes, w0 ),
        emFLPFB );
}
inline void _H5Proplist::get_cache( int *nelmts, size_t *nslots, 
    size_t *nbytes, double *w0 )const{
    ErrH5::check( H5Pget_cache( _obj, nelmts, nslots, nbytes, w0 ), 
        emFLPFB );
}
inline _H5Proplist::~_H5Proplist(){ 
    if(_state & stFREE) ErrH5::check( H5Pclose( _obj ), emFLPFB ); 
}

}
}
#endif	//_HIPPIO_H5_OBJ_RAW_PROPLIST_H_