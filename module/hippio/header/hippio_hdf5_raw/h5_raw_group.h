/**
create: Yangyao CHEN, 2020/11/29
    [write   ] _Group:   Intermediate-level HDF5 group API.
*/ 
#ifndef _HIPPIO_H5_RAW_GROUP_H_
#define _HIPPIO_H5_RAW_GROUP_H_

#include "h5_raw_named_base.h"
#include "../hippio_hdf5/h5_util_path.h"

namespace HIPP::IO::H5 {

class _Group: public _NamedObj {
public:
    typedef _NamedObj parent_t;

    typedef H5G_storage_type_t  storage_type_t;
    typedef H5G_info_t          info_t;

    inline static constexpr storage_type_t 
        storeCOMPACT_T        = H5G_STORAGE_TYPE_COMPACT,
        storeDENSE_T          = H5G_STORAGE_TYPE_DENSE,
        storeSYMBOL_TABLE_T   = H5G_STORAGE_TYPE_SYMBOL_TABLE;

    using parent_t::parent_t;

    _Group(hid_t loc, const char *name, 
        hid_t lcprop = _Proplist::vDFLT, hid_t gcprop = _Proplist::vDFLT, 
        hid_t aprop  = _Proplist::vDFLT);
    
    virtual ~_Group() noexcept override;
    
    static hid_t create(hid_t loc, const char *name, 
        hid_t lcprop = _Proplist::vDFLT, hid_t gcprop = _Proplist::vDFLT, 
        hid_t aprop  = _Proplist::vDFLT);
    
    static hid_t open( hid_t loc, const char *name, 
        hid_t aprop = _Proplist::vDFLT);
    
    static void close(hid_t group);

    void get_info(info_t &info) const;
    void get_info_by_name(const char *name, info_t &info, 
        hid_t laprop = _Proplist::vDFLT) const;
    void get_info_by_idx(const char *group_name, index_t idx_type, 
        iter_order_t order, hsize_t n, info_t &info, 
        hid_t laprop = _Proplist::vDFLT) const;
};


inline _Group::_Group(hid_t loc, const char *name, 
    hid_t lcprop, hid_t gcprop, hid_t aprop )
: _Group( create(loc, name, lcprop, gcprop, aprop) ) { }

inline _Group::~_Group() noexcept { 
    if(is_auto_free()) {
        close(raw()); state_flag(0);
    }
}

inline hid_t _Group::create( hid_t loc, const char *name, 
    hid_t lcprop, hid_t gcprop, hid_t aprop  )
{
    hid_t group = H5Gcreate( loc, name, lcprop, gcprop, aprop );
    ErrH5::check(group, emFLPFB, "  ... cannot create group ", name, '\n');
    return group;
}

inline hid_t _Group::open( hid_t loc, const char *name, 
    hid_t aprop  ){
    hid_t group = H5Gopen( loc, name, aprop );
    ErrH5::check(group, "  ... cannot open group ", group, '\n');
    return group;
}

inline void _Group::close(hid_t group) {
    ErrH5::check( H5Gclose(group), emFLPFB );
}

inline void _Group::get_info(info_t &info) const {
    ErrH5::check( H5Gget_info(raw(), &info), emFLPFB);
}

inline void _Group::get_info_by_name(const char *name, info_t &info, 
    hid_t laprop) const
{
    ErrH5::check( H5Gget_info_by_name(raw(), name, &info, laprop), 
        emFLPFB, "  ... get info failed (name=", name, ")\n");
}

inline void _Group::get_info_by_idx(const char *group_name, index_t idx_type, 
    iter_order_t order, hsize_t n, info_t &info, hid_t laprop) const
{
    ErrH5::check( H5Gget_info_by_idx(raw(), group_name, idx_type, order, n, 
        &info, laprop), emFLPFB, 
        "  ... get info failed (group_name=", group_name, 
        ", idx=", n, ")\n");
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_RAW_GROUP_H_