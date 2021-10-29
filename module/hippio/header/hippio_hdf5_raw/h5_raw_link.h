/**
create: Yangyao CHEN, 2021/10/12
    [write   ] _Link - for hdf5 link features.
*/

#ifndef _HIPPIO_H5_RAW_LINK_H_
#define _HIPPIO_H5_RAW_LINK_H_
#include "h5_raw_base.h"
#include "h5_raw_proplist.h"

namespace HIPP::IO::H5 {

class _Link {
public:
    typedef H5L_info_t info_t;
    typedef H5L_type_t type_t;
    inline static constexpr type_t 
        tERROR    = H5L_TYPE_ERROR,
        tHARD     = H5L_TYPE_HARD,
        tSOFT     = H5L_TYPE_SOFT,
        tEXTERNAL = H5L_TYPE_EXTERNAL,
        tMAX      = H5L_TYPE_MAX;

    typedef H5L_iterate_t iterate_t;

    static void create_hard(hid_t cur_loc, const char *cur_name, hid_t dst_loc,
        const char *dst_name, 
        hid_t lcprop = _Proplist::vDFLT, 
        hid_t laprop = _Proplist::vDFLT);
    static void  create_soft(const char *link_target, 
        hid_t link_loc_id, const char *link_name, 
        hid_t lcprop = _Proplist::vDFLT, hid_t laprop = _Proplist::vDFLT);
    static void create_external(const char *file_name, const char *obj_name, 
        hid_t link_loc_id, const char *link_name, 
        hid_t lcprop = _Proplist::vDFLT, hid_t laprop = _Proplist::vDFLT);

    static htri_t exists(hid_t loc, const char *name, 
        hid_t laprop = _Proplist::vDFLT);

    static void get_info(hid_t loc, const char *name, info_t &info, 
        hid_t laprop = _Proplist::vDFLT);
    static void get_info_by_idx(hid_t loc, const char *group_name, 
        index_t idx_type, iter_order_t order, hsize_t n, info_t &info, 
        hid_t laprop = _Proplist::vDFLT);

    static ssize_t get_name_by_idx(hid_t loc, const char *group_name, 
        index_t index_field, iter_order_t order, hsize_t n, 
        char *name, size_t size, hid_t laprop = _Proplist::vDFLT);
    static string get_name_by_idx(hid_t loc, const char *group_name, 
        index_t index_field, iter_order_t order, hsize_t n, 
        hid_t laprop = _Proplist::vDFLT);

    static void get_val(hid_t loc, const char *name, 
        void *buff, size_t size, hid_t laprop = _Proplist::vDFLT);
    static void get_val_by_idx(hid_t loc, const char *group_name, 
        index_t idx_type, iter_order_t order, hsize_t n, void *buff,
        size_t size, hid_t laprop = _Proplist::vDFLT);
    
    static herr_t iterate(hid_t group, index_t idx_type, iter_order_t order, 
        hsize_t &idx, iterate_t op, void *op_data);
    static herr_t iterate_by_name(hid_t loc, const char *group_name, 
        index_t idx_type, iter_order_t order, 
        hsize_t &idx, iterate_t op, void *op_data, 
        hid_t laprop = _Proplist::vDFLT);

    static herr_t visit(hid_t group, index_t idx_type, iter_order_t order, 
        iterate_t op, void *op_data);
    static herr_t visit_by_name(hid_t loc, const char *group_name, 
        index_t idx_type, iter_order_t order, iterate_t op, void *op_data, 
        hid_t laprop = _Proplist::vDFLT);

    static void move(hid_t src_loc, const char *src_name, 
        hid_t dst_loc, const char *dst_name, 
        hid_t lcprop = _Proplist::vDFLT, hid_t laprop = _Proplist::vDFLT);
    static void copy(hid_t src_loc, const char *src_name, 
        hid_t dst_loc, const char *dst_name, 
        hid_t lcprop = _Proplist::vDFLT, hid_t laprop = _Proplist::vDFLT);
    static void delete_(hid_t loc, const char *name, 
        hid_t laprop = _Proplist::vDFLT);
    static void delete_by_idx(hid_t loc, const char *group_name, 
        index_t idx_type, iter_order_t order, hsize_t n, 
        hid_t laprop = _Proplist::vDFLT);
};

inline void _Link::create_hard(hid_t cur_loc, const char *cur_name, 
    hid_t dst_loc, const char *dst_name, hid_t lcprop, hid_t laprop)
{
    ErrH5::check(H5Lcreate_hard(cur_loc, cur_name, dst_loc, dst_name, lcprop, 
        laprop), emFLPFB, "  ... creation failed (cur_name=", cur_name, 
        ", dst_name=", dst_name, ")\n");
}

inline void _Link::create_soft(const char *link_target, 
    hid_t link_loc_id, const char *link_name, 
    hid_t lcprop, hid_t laprop)
{
    ErrH5::check(H5Lcreate_soft(link_target, link_loc_id, link_name, lcprop, 
        laprop), emFLPFB, "  ... creation failed (link_target=", link_target, 
        ", link_name=", link_name, ")\n");
}

inline void _Link::create_external(const char *file_name, const char *obj_name, 
    hid_t link_loc_id, const char *link_name, 
    hid_t lcprop, hid_t laprop)
{
    ErrH5::check(H5Lcreate_external(file_name, obj_name, link_loc_id, link_name, 
        lcprop, laprop), emFLPFB, "  ... creation failed (file_name=", 
        file_name, ", obj_name=", obj_name, 
        ", link_name=", link_name, ")\n");
}

inline htri_t _Link::exists(hid_t loc, const char *name, hid_t laprop) {
    htri_t ret = H5Lexists(loc, name, laprop);
    ErrH5::check(ret, emFLPFB, "  ... name ", name, " detection failed\n");
    return ret;
}

inline void _Link::get_info(hid_t loc, const char *name, info_t &info, 
    hid_t laprop)
{
    ErrH5::check(H5Lget_info(loc, name, &info, laprop), 
        emFLPFB, "  ... get info failed (name=", name, ")\n");
}

inline void _Link::get_info_by_idx(hid_t loc, const char *group_name, 
    index_t idx_type, iter_order_t order, hsize_t n, info_t &info, 
    hid_t laprop)
{
    ErrH5::check(H5Lget_info_by_idx(loc, group_name, idx_type, order, n, 
        &info, laprop), emFLPFB, "  ... get info failed (group_name=", 
        group_name, ", idx=", n, ")\n");
}

inline ssize_t _Link::get_name_by_idx(hid_t loc, const char *group_name, 
    index_t index_field, iter_order_t order, hsize_t n, 
    char *name, size_t size, hid_t laprop) {
    ssize_t actual_size = H5Lget_name_by_idx(loc, group_name, index_field, 
        order, n, name, size, laprop);
    ErrH5::check(actual_size, emFLPFB, 
        "  ... get name indexed ", n, " failed");
    return actual_size;
}

inline string _Link::get_name_by_idx(hid_t loc, const char *group_name, 
    index_t index_field, iter_order_t order, hsize_t n, hid_t laprop)
{
    auto size = get_name_by_idx(loc, group_name, index_field, order, 
        n, NULL, 0, laprop);
    string name(static_cast<size_t>(size), '\0');
    auto act_sz = get_name_by_idx(loc, group_name, index_field, order, 
        n, name.data(), size+1, laprop);
    if( act_sz != size ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB);
    return name;
}

inline void _Link::get_val(hid_t loc, const char *name, 
    void *buff, size_t size, hid_t laprop)
{
    ErrH5::check(H5Lget_val(loc, name, buff, size, laprop), 
        emFLPFB,  "  ... get value failed (name=", name, ")\n");
}

inline void _Link::get_val_by_idx(hid_t loc, const char *group_name, 
    index_t idx_type, iter_order_t order, hsize_t n, void *buff,
    size_t size, hid_t laprop)
{
    ErrH5::check(H5Lget_val_by_idx(loc, group_name, idx_type, order, n, buff, 
        size, laprop), emFLPFB, "  ... get value failed "
        "(group_name=", group_name, ", idx=", n, ")\n");
}

inline herr_t _Link::iterate(hid_t group, index_t idx_type, iter_order_t order, 
    hsize_t &idx, iterate_t op, void *op_data)
{
    auto ret = H5Literate(group, idx_type, order, &idx, op, op_data);
    ErrH5::check(ret, emFLPFB, "  ... iteration failed\n");
    return ret;
}

inline herr_t _Link::iterate_by_name(hid_t loc, const char *group_name, 
    index_t idx_type, iter_order_t order, 
    hsize_t &idx, iterate_t op, void *op_data, hid_t laprop)
{
    auto ret = H5Literate_by_name(loc, group_name, idx_type, order, &idx, op, 
        op_data, laprop);
    ErrH5::check(ret, emFLPFB, "  ... iteration failed (group_name=", 
        group_name, ")\n");
    return ret;
}

inline herr_t _Link::visit(hid_t group, index_t idx_type, iter_order_t order, 
    iterate_t op, void *op_data)
{
    auto ret = H5Lvisit(group, idx_type, order, op, 
        op_data);
    ErrH5::check(ret, emFLPFB, "  ... visit failed\n");
    return ret;
}

inline herr_t _Link::visit_by_name(hid_t loc, const char *group_name, 
    index_t idx_type, iter_order_t order, iterate_t op, void *op_data, 
    hid_t laprop)
{
    auto ret = H5Lvisit_by_name(loc, group_name, idx_type, order, op, 
        op_data, laprop);
    ErrH5::check(ret, emFLPFB, "  ... visit failed (group_name=", 
        group_name, ")\n");
    return ret;
}

inline void _Link::move(hid_t src_loc, const char *src_name, 
    hid_t dst_loc, const char *dst_name, hid_t lcprop, hid_t laprop)
{
    ErrH5::check(H5Lmove(src_loc, src_name, dst_loc, dst_name, lcprop, 
        laprop), emFLPFB, "  ... move ", src_name, 
        " to ", dst_name, " failed\n");
}

inline void _Link::copy(hid_t src_loc, const char *src_name, 
    hid_t dst_loc, const char *dst_name, hid_t lcprop, hid_t laprop)
{
    ErrH5::check(H5Lcopy(src_loc, src_name, dst_loc, dst_name, lcprop, 
        laprop), emFLPFB, "  ... copy ", src_name, 
        " to ", dst_name, " failed\n");
}

inline void _Link::delete_(hid_t loc, const char *name, hid_t laprop) {
    ErrH5::check(H5Ldelete(loc, name, laprop), 
        emFLPFB, "  .. cannot delete ", name);
}

inline void _Link::delete_by_idx(hid_t loc, const char *group_name, 
    index_t idx_type, iter_order_t order, hsize_t n, hid_t laprop)
{
    ErrH5::check(H5Ldelete_by_idx(loc, group_name, idx_type, order, n, laprop), 
        emFLPFB, "  ... iteration failed (group_name=", group_name, ")\n");
}
    
} // namespace HIPP::IO::H5
#endif	//_HIPPIO_H5_RAW_LINK_H_