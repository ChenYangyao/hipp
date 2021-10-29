/**
create: Yangyao CHEN, 2020/11/29
     [write   ] _Dataspace:   Intermediate-level HDF5 dataspace API.
*/ 
#ifndef _HIPPIO_H5_RAW_DATASPACE_H_
#define _HIPPIO_H5_RAW_DATASPACE_H_

#include "h5_raw_base.h"

namespace HIPP::IO::H5 {

class _Dataspace: public _Obj {
public:
    typedef _Obj parent_t;
    
    typedef H5S_seloper_t   seloper_t;
    typedef H5S_class_t     class_t;
    typedef H5S_sel_type    seltype_t;

    inline static constexpr class_t 
        cNULL      = H5S_NULL, 
        cSIMPLE    = H5S_SIMPLE,
        cSCALAR    = H5S_SCALAR;

    inline static constexpr seloper_t
        selSET     =   H5S_SELECT_SET,
        selOR      =   H5S_SELECT_OR,
        selAND     =   H5S_SELECT_AND,
        selXOR     =   H5S_SELECT_XOR,
        selNOTB    =   H5S_SELECT_NOTB,
        selNOTA    =   H5S_SELECT_NOTA,
        selAPPEND  =   H5S_SELECT_APPEND,
        selPREPEND =   H5S_SELECT_PREPEND;

    inline static const seltype_t
        selNONE_T       = H5S_SEL_NONE,
        selALL_T        = H5S_SEL_ALL,
        selPOINTS_T     = H5S_SEL_POINTS,
        selHYPERSLABS_T = H5S_SEL_HYPERSLABS;

    inline static constexpr hid_t vALL = H5S_ALL;
    
    using parent_t::parent_t;

    _Dataspace(int rank, const hsize_t dims[], const hsize_t maxdims[] = NULL);

    virtual ~_Dataspace() noexcept override;

    static hid_t create(class_t type);
    static hid_t create_simple(int rank, const hsize_t dims[], 
        const hsize_t maxdims[]);

    hid_t copy() const;
    void extent_copy(hid_t dst_dspace) const;

    static void close(hid_t dspace);

    class_t get_simple_extent_type() const;
    int get_simple_extent_ndims() const;
    void get_simple_extent_dims(hsize_t dims[], hsize_t maxdims[]) const;

    htri_t extent_equal(hid_t space2_id) const;

    void select_hyperslab( seloper_t op, 
        const hsize_t *start, const hsize_t *stride,
        const hsize_t *count, const hsize_t *block);
    hid_t combine_hyperslab(seloper_t op, 
        const hsize_t *start, const hsize_t *stride,
        const hsize_t *count, const hsize_t *block) const;
    hid_t combine_select(seloper_t op, hid_t space2_id) const;

    void select_elements(seloper_t op, size_t n_elem, const hsize_t coord[]);
    void select_all();
    void select_none();

    void select_copy(hid_t dst_space) const;
    
    hssize_t get_select_npoints() const;
    seltype_t get_select_type() const;
    htri_t select_valid() const;
};

inline _Dataspace::_Dataspace(int rank, const hsize_t dims[], 
    const hsize_t maxdims[])
: _Dataspace(create_simple(rank, dims, maxdims))
{ }

inline _Dataspace::~_Dataspace() noexcept { 
    if(is_auto_free()) {
        close(raw()); state_flag(0);
    }
}

inline hid_t _Dataspace::create( class_t type ) {
    hid_t dspace = H5Screate(type);
    ErrH5::check( dspace, emFLPFB );
    return dspace;
}

inline hid_t _Dataspace::create_simple(int rank, const hsize_t dims[], 
    const hsize_t maxdims[])
{
    hid_t dsp = H5Screate_simple(rank, dims, maxdims);
    ErrH5::check(dsp, emFLPFB );
    return dsp;
}

inline hid_t _Dataspace::copy() const {
    hid_t ret = H5Scopy(raw());
    ErrH5::check(ret, emFLPFB, "  ... copy failed\n");
    return ret;
}

inline void _Dataspace::extent_copy(hid_t dst_dspace) const {
    ErrH5::check(H5Sextent_copy(dst_dspace, raw()), 
        emFLPFB, "  ... copy extent failed\n");
}

inline void _Dataspace::close(hid_t dspace) {
    ErrH5::check(H5Sclose(dspace), emFLPFB); 
}

inline _Dataspace::class_t _Dataspace::get_simple_extent_type() const {
    auto ret = H5Sget_simple_extent_type(raw());
    ErrH5::check(ret, emFLPFB, "  ... get dataspace type failed");
    return ret;
}

inline int _Dataspace::get_simple_extent_ndims() const{
    int ndims = H5Sget_simple_extent_ndims( raw() );
    ErrH5::check( ndims, emFLPFB );
    return ndims;
}

inline void _Dataspace::get_simple_extent_dims(hsize_t dims[], 
    hsize_t maxdims[]) const
{
    ErrH5::check( H5Sget_simple_extent_dims(raw(), dims, maxdims), emFLPFB );
}

inline htri_t _Dataspace::extent_equal(hid_t space2_id) const {
    auto ret = H5Sextent_equal(raw(), space2_id);
    ErrH5::check(ret, emFLPFB, "  ... check dataspace equality failed\n");
    return ret;
}

inline void _Dataspace::select_hyperslab( seloper_t op, 
    const hsize_t *start, const hsize_t *stride,
    const hsize_t *count, const hsize_t *block){
    ErrH5::check( H5Sselect_hyperslab( raw(), op, start, stride, count, 
        block ), emFLPFB );
}

inline hid_t _Dataspace::combine_hyperslab(seloper_t op, 
    const hsize_t *start, const hsize_t *stride,
    const hsize_t *count, const hsize_t *block) const
{
    auto new_sp = H5Scombine_hyperslab(raw(), op, start, stride, count, block);
    ErrH5::check(new_sp, emFLPFB, "  ... combination failed\n");
    return new_sp;
}

inline hid_t _Dataspace::combine_select(seloper_t op, hid_t space2_id) const {
    auto new_sp = H5Scombine_select(raw(), op, space2_id);
    ErrH5::check(new_sp, emFLPFB, "  ... combination failed\n");
    return new_sp;
}

inline void _Dataspace::select_elements(seloper_t op, size_t n_elem, 
    const hsize_t coord[])
{
    ErrH5::check(H5Sselect_elements(raw(), op, n_elem, coord), 
        emFLPFB, "  ... selection of element failed\n");
}

inline void _Dataspace::select_all() {
    ErrH5::check(H5Sselect_all(raw()), emFLPFB, "   ... selection of "
        "all failed\n");
}

inline void _Dataspace::select_none() {
    ErrH5::check(H5Sselect_none(raw()), emFLPFB, "   ... selection of "
        "none failed\n");
}

inline void _Dataspace::select_copy(hid_t dst_space) const {
    ErrH5::check(H5Sselect_copy(dst_space, raw()), 
        emFLPFB, "  ... selection of copy failed\n");
}
    

inline hssize_t _Dataspace::get_select_npoints() const {
    hssize_t npoints = H5Sget_select_npoints( raw() );
    ErrH5::check( npoints, emFLPFB );
    return npoints;
}

inline auto _Dataspace::get_select_type() const -> seltype_t {
    auto ret = H5Sget_select_type(raw());
    ErrH5::check(ret, emFLPFB, 
        "   ... get select type failed\n");
    return ret;
}


inline htri_t _Dataspace::select_valid() const {
    auto ret = H5Sselect_valid(raw());
    ErrH5::check(ret, emFLPFB, "  ... failed to verify selection\n");
    return ret;
}

} // namespace HIPP::IO::H5
#endif	//_HIPPIO_H5_RAW_DATASPACE_H_