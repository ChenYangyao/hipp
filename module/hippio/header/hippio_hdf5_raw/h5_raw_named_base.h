#ifndef _HIPPIO_H5_RAW_NAMED_BASE_H_
#define _HIPPIO_H5_RAW_NAMED_BASE_H_

#include "h5_raw_base.h"
#include "h5_raw_attr.h"
#include "h5_raw_dataspace.h"
#include "h5_raw_proplist.h"

namespace HIPP::IO::H5 {

/**
Intermediate wrapper for HDF5 named object, i.e., this is the common base 
class of group and dataset and datatype.
*/
class _NamedObj : public _Obj {
public: 
    typedef _Obj parent_t;

    typedef H5O_info_t info_t;
    typedef H5O_type_t type_t;
    inline static constexpr type_t 
        tUNKNOWN        = H5O_TYPE_UNKNOWN,
        tGROUP          = H5O_TYPE_GROUP,
        tDATASET        = H5O_TYPE_DATASET,
        tNAMED_DATATYPE = H5O_TYPE_NAMED_DATATYPE,
        tMAP            = H5O_TYPE_MAP,
        tNTYPES         = H5O_TYPE_NTYPES;

    typedef unsigned info_field_t;
    inline static constexpr info_field_t
        infoBASIC      = H5O_INFO_BASIC,
        infoTIME       = H5O_INFO_TIME,
        infoNUM_ATTRS  = H5O_INFO_NUM_ATTRS,
        infoHDR        = H5O_INFO_HDR,
        infoMETA_SIZE  = H5O_INFO_META_SIZE,
        infoALL        = H5O_INFO_ALL;

    typedef H5O_iterate_t iterate_t;

    using parent_t::parent_t;

    virtual ~_NamedObj() noexcept override;

    static hid_t open(hid_t loc, const char *name, 
        hid_t laprop = _Proplist::vDFLT);
    static hid_t open_by_idx(hid_t loc, const char *group_name, 
        index_t idx_type, iter_order_t order, hsize_t n, 
        hid_t laprop = _Proplist::vDFLT);

    static void close(hid_t obj);

    htri_t exists_by_name(const char *name, 
        hid_t laprop = _Proplist::vDFLT) const;

    void get_info(info_t &info, info_field_t fields = infoALL) const;
    void get_info_by_name(const char *name, info_t &info, 
        info_field_t fields = infoALL, hid_t laprop = _Proplist::vDFLT) const;
    void get_info_by_idx(const char *name, index_t idx_type, iter_order_t order,
        hsize_t n, info_t &info, info_field_t fields = infoALL, 
        hid_t laprop = _Proplist::vDFLT) const;

    herr_t visit(index_t idx_type, iter_order_t order, iterate_t op, 
        void *op_data, info_field_t fields = infoALL) const;
    herr_t visit_by_name(const char *name, index_t idx_type, iter_order_t order, 
        iterate_t op, void *op_data, info_field_t fields = infoALL, 
        hid_t laprop = _Proplist::vDFLT) const;
};


inline _NamedObj::~_NamedObj() noexcept {
    if( is_auto_free() ) {
        close(raw()); state_flag(0);
    }
}

inline hid_t _NamedObj::open(hid_t loc, const char *name, hid_t laprop) {
    auto ret = H5Oopen(loc, name, laprop);
    ErrH5::check(ret, emFLPFB,
        "  ...open failed (name=", name, ")\n");
    return ret;
}

inline hid_t _NamedObj::open_by_idx(hid_t loc, const char *group_name, 
    index_t idx_type, iter_order_t order, hsize_t n, hid_t laprop) 
{
    auto ret = H5Oopen_by_idx(loc, group_name, idx_type, order, n, laprop);
    ErrH5::check(ret, emFLPFB, "  ... open failed (group_name=", 
        group_name, ")\n");
    return ret;
}

inline void _NamedObj::close(hid_t obj) {
    ErrH5::check(H5Oclose(obj), emFLPFB);
}

inline htri_t _NamedObj::exists_by_name(const char *name, hid_t laprop) const {
    auto ret = H5Oexists_by_name(raw(), name, laprop);
    ErrH5::check(ret, emFLPFB, "  ... resolving ", name, " failed\n");
    return ret;
}

inline void _NamedObj::get_info(info_t &info, info_field_t fields) const {
    ErrH5::check(H5Oget_info(raw(), &info, fields), emFLPFB, 
        "  ... get info failed\n");
}

inline void _NamedObj::get_info_by_name(const char *name, 
    info_t &info, info_field_t fields, hid_t laprop) const 
{
    ErrH5::check(H5Oget_info_by_name(raw(), name, &info, fields, laprop), 
        emFLPFB, "  ... get info failed (name=", name, ")\n");
}

inline void _NamedObj::get_info_by_idx(const char *name, index_t idx_type, 
    iter_order_t order, hsize_t n, info_t &info, info_field_t fields, 
    hid_t laprop) const 
{
    ErrH5::check(H5Oget_info_by_idx(raw(), name, idx_type, order, n, &info, 
        fields, laprop), emFLPFB, 
        "  ... get info failed (name=", name, ", idx=", n, ")\n");
}

inline herr_t _NamedObj::visit(index_t idx_type, iter_order_t order, iterate_t op, 
    void *op_data, info_field_t fields) const 
{
    auto ret = H5Ovisit(raw(), idx_type, order, op, 
        op_data, fields);
    ErrH5::check(ret, emFLPFB, "  ... visit failed\n");
    return ret;
}

inline herr_t _NamedObj::visit_by_name(const char *name, index_t idx_type, 
    iter_order_t order, iterate_t op, void *op_data, info_field_t fields,
    hid_t laprop) const 
{
    auto ret = H5Ovisit_by_name(raw(), name, idx_type, order, op, op_data, 
        fields, laprop);
    ErrH5::check(ret, emFLPFB, 
        "  ... get info failed (name=", name, ")\n");
    return ret;
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_RAW_NAMED_BASE_H_