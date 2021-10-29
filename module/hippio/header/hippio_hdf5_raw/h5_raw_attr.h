/**
create: Yangyao CHEN, 2020/11/29
    [write   ] _Attr - intermediate-level wrapper for HDF5 attribute.
*/ 

#ifndef _HIPPIO_H5_RAW_ATTR_H_
#define _HIPPIO_H5_RAW_ATTR_H_

#include "h5_raw_base.h"
#include "h5_raw_proplist.h"

namespace HIPP::IO::H5 {

class _Attr: public _Obj {
public:
    typedef _Obj parent_t;
    typedef H5A_info_t info_t;
    typedef H5A_operator_t operator_t;
    
    using parent_t::parent_t;

    _Attr(hid_t loc, const char *name, hid_t dtype, hid_t dspace, 
        hid_t cprop = _Proplist::vDFLT, 
        hid_t aprop = _Proplist::vDFLT);

    virtual ~_Attr( ) noexcept override;

    static hid_t create(hid_t loc, const char *name, 
        hid_t dtype, hid_t dspace, 
        hid_t cprop = _Proplist::vDFLT, hid_t aprop = _Proplist::vDFLT);
    
    static hid_t open(hid_t loc, const char *name, 
        hid_t aprop = _Proplist::vDFLT);
    static hid_t open_by_name(hid_t loc, const char *obj_name, 
        const char *attr_name, 
        hid_t aaprop = _Proplist::vDFLT, 
        hid_t laprop = _Proplist::vDFLT);
    static hid_t open_by_idx(hid_t loc, const char *obj_name, 
        index_t idx_type, iter_order_t order, 
        hsize_t n, 
        hid_t aaprop = _Proplist::vDFLT, 
        hid_t laprop = _Proplist::vDFLT);
    
    static void close(hid_t attr);
    
    ssize_t get_name(size_t size, char *name) const;
    string get_name() const;
    static ssize_t get_name_by_idx(hid_t loc, const char *obj_name, 
        index_t idx_type, iter_order_t order, 
        hsize_t n, char *name, size_t size, 
        hid_t laprop = _Proplist::vDFLT);
    static string get_name_by_idx(hid_t loc, const char *obj_name, 
        index_t idx_type, iter_order_t order, hsize_t n, 
        hid_t laprop = _Proplist::vDFLT);

    hsize_t get_storage_size() const;

    void get_info(info_t &info) const;
    static void get_info_by_name(hid_t id, const char *obj_name, 
        const char *attr_name, info_t &info, 
        hid_t laprop = _Proplist::vDFLT);
    static void get_info_by_idx(hid_t id, const char *obj_name, 
        index_t idx_type, iter_order_t order, hsize_t n, 
        info_t &info, hid_t laprop = _Proplist::vDFLT);

    hid_t get_space() const;
    hid_t get_type() const;
    
    static htri_t exists(hid_t id, const char *name);
    static htri_t exists_by_name(hid_t id, const char *obj_name, 
        const char *attr_name, hid_t laprop = _Proplist::vDFLT);

    static herr_t iterate(hid_t loc, index_t idx_type, iter_order_t order, 
        hsize_t &idx, operator_t op, void *op_data);
    static herr_t iterate_by_name(hid_t loc, const char *obj_name,
        index_t idx_type, iter_order_t order, 
        hsize_t &idx, operator_t op, void *op_data, 
        hid_t laprop = _Proplist::vDFLT);

    static void rename(hid_t loc, const char *old_name, const char *new_name);
    static void delete_(hid_t loc, const char *name);
    static void delete_by_name(hid_t loc, const char *obj_name, 
        const char *attr_name, 
        hid_t laprop = _Proplist::vDFLT);
    static void delete_by_idx(hid_t loc, const char *obj_name,
        index_t idx_type, iter_order_t order, hsize_t n, 
        hid_t laprop = _Proplist::vDFLT);

    void write(hid_t memtype, const void *buff);
    void read(hid_t memtype, void *buff);
};

/** Implementation. */

inline _Attr::_Attr( hid_t loc, const char *name, hid_t dtype, 
    hid_t dspace, hid_t cprop, hid_t aprop )
: _Attr( create(loc, name, dtype, dspace, cprop, aprop) )
{ }

inline _Attr::~_Attr( ) noexcept { 
    if( is_auto_free() ) {
        close(raw()); state_flag(0);
    }
        
}

inline hid_t _Attr::create( hid_t loc, const char *name, hid_t dtype, 
    hid_t dspace, hid_t cprop, hid_t aprop )
{
    hid_t attr = H5Acreate( loc, name, dtype, dspace, cprop, aprop );
    ErrH5::check(
        attr, emFLPFB, 
        "  ... creation of attribute ", name, "failed\n");
    return attr;
}

inline hid_t _Attr::open( hid_t loc, const char *name, hid_t aprop)
{
    hid_t attr = H5Aopen( loc, name, aprop );
    ErrH5::check(
        attr, emFLPFB, 
        "  ... open of attribute ", name, "failed\n");
    return attr;
}

inline hid_t _Attr::open_by_name(hid_t loc, const char *obj_name, 
        const char *attr_name, hid_t aaprop, hid_t laprop) {
    hid_t attr = H5Aopen_by_name(loc, obj_name, attr_name, aaprop, laprop);
    ErrH5::check(attr, emFLPFB, "  ... open of attribute ", 
        attr_name, " under ", obj_name, " failed\n");
    return attr;
}

inline hid_t _Attr::open_by_idx(hid_t loc, const char *obj_name, 
    index_t idx_type, iter_order_t order, 
    hsize_t n, hid_t aaprop, hid_t laprop)
{
    hid_t attr = H5Aopen_by_idx(loc, obj_name, idx_type, order, n, 
        aaprop, laprop);
    ErrH5::check(attr, emFLPFB, "  ... open of attribute indexed ", n, 
        " under ", obj_name, " failed\n");
    return attr;
}

inline void _Attr::close(hid_t attr) {
    ErrH5::check(H5Aclose(attr), emFLPFB); 
}

inline ssize_t _Attr::get_name(size_t size, char *name) const {
    auto ret = H5Aget_name(raw(), size, name);
    ErrH5::check(ret, emFLPFB, "  ... get name failed (buf at ", (void *)name, 
        ", sized ", size, ")\n");
    return ret;
}

inline string _Attr::get_name() const {
    auto sz_required = get_name(0, nullptr);
    string ret(sz_required, '\0');
    auto sz_actual = get_name(sz_required+1, ret.data());
    if( sz_required != sz_actual ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... size got ", 
            sz_actual, " but required ", sz_required);
    return ret;
}

inline ssize_t _Attr::get_name_by_idx(hid_t loc, const char *obj_name, 
    index_t idx_type, iter_order_t order, 
    hsize_t n, char *name, size_t size, hid_t laprop)
{
    auto ret = H5Aget_name_by_idx(loc, obj_name, idx_type, order, n, 
        name, size, laprop);
    ErrH5::check(ret, emFLPFB, "  ... get name failed (buf at ", (void *)name, 
        ", sized ", size, ", attr index ", n, " under ", obj_name, 
        " at loc_id ", loc, ", idx_type ", idx_type, " ordered ",
        order, ")\n");
    return ret;
}

inline string _Attr::get_name_by_idx(hid_t loc, const char *obj_name, 
    index_t idx_type, iter_order_t order, hsize_t n, hid_t laprop)
{
    auto sz_required = get_name_by_idx(loc, obj_name, idx_type, order, n, 
        nullptr, 0, laprop);
    string ret(sz_required, '\0');
    auto sz_actual = get_name_by_idx(loc, obj_name, idx_type, order, n, 
        ret.data(), sz_required+1, laprop);
    if( sz_required != sz_actual ) 
        ErrLogic::throw_(ErrLogic::eLENGTH, emFLPFB, "  ... size got ", 
            sz_actual, " but required ", sz_required);
    return ret;
}

inline hsize_t _Attr::get_storage_size() const {
    auto ret = H5Aget_storage_size(raw());
    ErrH5::check(ret, emFLPFB);
    return ret;
}

inline void _Attr::get_info(info_t &info) const {
    ErrH5::check(H5Aget_info(raw(), &info), emFLPFB);
}

inline void _Attr::get_info_by_name(hid_t id, const char *obj_name, 
    const char *attr_name, info_t &info, hid_t laprop)
{
    ErrH5::check(H5Aget_info_by_name(id, obj_name, attr_name, &info, 
        laprop), emFLPFB, "  ... get info for attribute ", attr_name, 
            " under ", obj_name, " failed\n");
}

inline void _Attr::get_info_by_idx(hid_t id, const char *obj_name, 
    index_t idx_type, iter_order_t order, hsize_t n, 
    info_t &info, hid_t laprop)
{
    ErrH5::check(H5Aget_info_by_idx(id, obj_name, idx_type, order, n, &info, 
        laprop), emFLPFB, "  ... get info for attribute index ", n, 
            " under ", obj_name, " failed\n");
}

inline hid_t _Attr::get_space() const{
    hid_t dspace = H5Aget_space( raw() );
    ErrH5::check(dspace, emFLPFB);
    return dspace;
}
inline hid_t _Attr::get_type() const{
    hid_t dtype = H5Aget_type( raw() );
    ErrH5::check(dtype, emFLPFB);
    return dtype;
}

inline htri_t _Attr::exists(hid_t id, const char *name ){
    htri_t ret = H5Aexists( id, name );
    ErrH5::check(ret, emFLPFB, "  ... attributed ", name, '\n');
    return ret;
}

inline htri_t _Attr::exists_by_name(hid_t id, const char *obj_name, 
    const char *attr_name, hid_t laprop)
{
    htri_t ret = H5Aexists_by_name(id, obj_name, attr_name, laprop);
    ErrH5::check(ret, emFLPFB, "  ... attributed ", attr_name, " under ", 
        obj_name, '\n');
    return ret;
}

inline herr_t _Attr::iterate(hid_t loc, index_t idx_type, iter_order_t order, 
    hsize_t &idx, operator_t op, void *op_data)
{
    auto ret = H5Aiterate(loc, idx_type, order, &idx, 
        op, op_data);
    ErrH5::check(ret, emFLPFB, "  ... iterate failed at index", idx, '\n');
    return ret;
}

inline herr_t _Attr::iterate_by_name(hid_t loc, const char *obj_name,
    index_t idx_type, iter_order_t order, 
    hsize_t &idx, operator_t op, void *op_data, hid_t laprop)
{
    auto ret = H5Aiterate_by_name(loc, obj_name, idx_type, order, &idx, op,
        op_data, laprop);
    ErrH5::check(ret, emFLPFB, 
        "  ... iterate failed at index", idx, " under ", obj_name, '\n');
    return ret;
}

inline void _Attr::rename(hid_t loc, const char *old_name, 
    const char *new_name) 
{
    ErrH5::check(H5Arename(loc, old_name, new_name),
        emFLPFB, "  ... rename from ", old_name, " to ", 
        new_name, " failed\n");
}

inline void _Attr::delete_(hid_t loc, const char *name) {
    ErrH5::check(H5Adelete(loc, name), 
        emFLPFB, "  ... delete failed for ", name, '\n');
}

inline void _Attr::delete_by_name(hid_t loc, const char *obj_name, 
    const char *attr_name, hid_t laprop) {
    ErrH5::check(H5Adelete_by_name(loc, obj_name, attr_name, laprop), 
        emFLPFB, "  ... delete failed for ", attr_name, 
        " under ", obj_name, '\n');
}

inline void _Attr::delete_by_idx(hid_t loc, const char *obj_name,
    index_t idx_type, iter_order_t order, hsize_t n, hid_t laprop) {
    ErrH5::check(H5Adelete_by_idx(loc, obj_name, 
        idx_type, order, n, laprop), emFLPFB, 
        "  ... delete failed for attribute indexed ", n, " under ", obj_name, 
        '\n');
}

inline void _Attr::write( hid_t memtype, const void *buff )
{
    ErrH5::check(H5Awrite( raw(), memtype, buff ), emFLPFB );
}

inline void _Attr::read( hid_t memtype, void *buff )
{
    ErrH5::check(H5Aread( raw(), memtype, buff ), emFLPFB );
}

} // namespace HIPP::IO::H5

#endif	//_HIPPIO_H5_RAW_ATTR_H_