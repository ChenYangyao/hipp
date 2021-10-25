/**
create: Yangyao CHEN, 2020/11/29
    [write   ] _Proplist - Intermediate-level HDF5 property list API.
*/ 
#ifndef _HIPPIO_H5_RAW_PROPLIST_H_
#define _HIPPIO_H5_RAW_PROPLIST_H_

#include "h5_raw_base.h"

namespace HIPP::IO::H5 {

class _Proplist: public _Obj {
public:
    inline static constexpr hid_t vDFLT = H5P_DEFAULT;

    typedef H5D_layout_t d_layout_t;
    inline static constexpr d_layout_t
        laCONTIGUOUS = H5D_CONTIGUOUS,
        laCHUNKED    = H5D_CHUNKED,
        laCOMPACT    = H5D_COMPACT,
        laVIRTUAL    = H5D_VIRTUAL;
    
    inline static const hid_t 
        dACCESS = H5P_DATASET_ACCESS,
        dCREATE = H5P_DATASET_CREATE,
        dXFER   = H5P_DATASET_XFER,

        fCREATE = H5P_FILE_CREATE,
        fMOUNT  = H5P_FILE_MOUNT,
        fACCESS = H5P_FILE_ACCESS;

    typedef _Obj parent_t;
    
    using parent_t::parent_t;
    
    virtual ~_Proplist() noexcept override;
    
    static hid_t create(hid_t pclass);
    static void close(hid_t plist);
    
    void set_layout(d_layout_t layout);
    d_layout_t get_layout()const;

    void set_chunk( int ndims, const hsize_t *dim );
    int get_chunk_ndims()const;
    vector<hsize_t> get_chunk()const;

    void set_chunk_cache(size_t nslots, size_t nbytes, double w0);
    void get_chunk_cache(size_t *nslot, size_t *nbytes, double *w0)const;

    void set_cache( int nelmts, size_t nslots, size_t nbytes, double w0);
    void get_cache( int *nelmts, size_t *nslots, size_t *nbytes, 
        double *w0 )const;

    void set_preserve(hbool_t status);
    int get_preserve() const;
};


inline _Proplist::~_Proplist() noexcept { 
    if(is_auto_free()) {
        close(raw()); state_flag(0);
    }
}

inline hid_t _Proplist::create(hid_t pclass){
    hid_t plist = H5Pcreate(pclass);
    ErrH5::check(plist, emFLPFB);
    return plist;
}

inline void _Proplist::close(hid_t plist) {
    ErrH5::check(H5Pclose(plist), emFLPFB); 
}

inline void _Proplist::set_layout(d_layout_t layout){
    ErrH5::check( H5Pset_layout(raw(), layout), emFLPFB );
}

inline _Proplist::d_layout_t _Proplist::get_layout()const{
    d_layout_t layout = H5Pget_layout( raw() );
    ErrH5::check(layout, emFLPFB);
    return layout;
}

inline void _Proplist::set_chunk(int ndims, const hsize_t *dim){
    ErrH5::check( H5Pset_chunk( raw(), ndims, dim ), emFLPFB );
}

inline int _Proplist::get_chunk_ndims()const{
    int ndims = H5Pget_chunk(raw(), 0, NULL);
    ErrH5::check(ndims, emFLPFB);
    return ndims;
}

inline vector<hsize_t> _Proplist::get_chunk()const{
    vector<hsize_t> chunk( get_chunk_ndims() );
    ErrH5::check( 
        H5Pget_chunk( raw(), chunk.size(), chunk.data() ), emFLPFB );
    return chunk;
}

inline void _Proplist::set_chunk_cache( 
    size_t nslots, size_t nbytes, double w0 )
{
    ErrH5::check(H5Pset_chunk_cache( raw(), nslots, nbytes, w0 ), emFLPFB);
}

inline void _Proplist::get_chunk_cache( size_t *nslot, 
    size_t *nbytes, double *w0 )const{
    ErrH5::check(H5Pget_chunk_cache( raw(), nslot, nbytes, w0 ), emFLPFB);
}

inline void _Proplist::set_cache( int nelmts, size_t nslots, 
    size_t nbytes, double w0 ){
    ErrH5::check(
        H5Pset_cache( raw(), nelmts, nslots, nbytes, w0 ),
        emFLPFB);
}
inline void _Proplist::get_cache( int *nelmts, size_t *nslots, 
    size_t *nbytes, double *w0 )const{
    ErrH5::check(
        H5Pget_cache( raw(), nelmts, nslots, nbytes, w0 ), 
        emFLPFB);
}

inline void _Proplist::set_preserve(hbool_t status) {
    ErrH5::check(H5Pset_preserve(raw(), status), emFLPFB, 
        "  ... set preserve to ", status, " failed\n");
}

inline int _Proplist::get_preserve() const {
    auto ret = H5Pget_preserve(raw());
    ErrH5::check(ret, emFLPFB, "  ... get preserve failed\n");
    return ret;
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_RAW_PROPLIST_H_