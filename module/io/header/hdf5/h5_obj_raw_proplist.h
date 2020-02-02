#ifndef _HIPPIO_H5_OBJ_RAW_PROPLIST_H_
#define _HIPPIO_H5_OBJ_RAW_PROPLIST_H_
#include "h5_obj_raw_base.h"
namespace HIPP{
namespace IO{

class _H5Proplist: public _H5Obj{
public:
    typedef H5D_layout_t layout_t;

    explicit _H5Proplist( id_t plist, int state=1 ) 
        noexcept : _H5Obj(plist, state){ }
    
    static id_t create( id_t pclass ){
        id_t plist = H5Pcreate( pclass );
        ErrH5::check( plist, emFLPFB );
        return plist;
    };
    void set_layout( layout_t layout ){
        ErrH5::check( H5Pset_layout( _obj, layout ), emFLPFB );
    }
    layout_t get_layout()const{
        layout_t layout = H5Pget_layout( _obj );
        ErrH5::check( layout, emFLPFB );
        return layout;
    }

    void set_chunk( int ndims, const size_t *dim ){
        ErrH5::check( H5Pset_chunk( _obj, ndims, dim ), emFLPFB );
    }
    int get_chunk_ndims()const{
        int ndims = H5Pget_chunk(_obj, 0, NULL);
        ErrH5::check( ndims, emFLPFB );
        return ndims;
    }
    vector<size_t> get_chunk()const{
        vector<size_t> chunk( get_chunk_ndims() );
        ErrH5::check( 
            H5Pget_chunk( _obj, chunk.size(), chunk.data() ), emFLPFB );
        return chunk;
    }

    void set_chunk_cache( std::size_t nslots, std::size_t nbytes, double w0 ){
        ErrH5::check( H5Pset_chunk_cache( _obj, nslots, nbytes, w0 ), emFLPFB );
    }
    void get_chunk_cache( std::size_t *nslot, 
        std::size_t *nbytes, double *w0 )const{
        ErrH5::check( H5Pget_chunk_cache( _obj, nslot, nbytes, w0 ), emFLPFB );
    }

    void set_cache( int nelmts, std::size_t nslots, 
        std::size_t nbytes, double w0 ){
        ErrH5::check( H5Pset_cache( _obj, nelmts, nslots, nbytes, w0 ),
            emFLPFB );
    }
    void get_cache( int *nelmts, std::size_t *nslots, 
        std::size_t *nbytes, double *w0 )const{
        ErrH5::check( H5Pget_cache( _obj, nelmts, nslots, nbytes, w0 ), 
            emFLPFB );
    }

    ~_H5Proplist(){ if(_state) ErrH5::check( H5Pclose( _obj ), emFLPFB ); }
};

}
}
#endif	//_HIPPIO_H5_OBJ_RAW_PROPLIST_H_