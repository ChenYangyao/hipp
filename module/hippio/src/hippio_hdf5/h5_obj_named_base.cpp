#include <hippio.h>

namespace HIPP::IO::H5 {

void NamedObj::get_info(info_t &obj_info, info_field_t fields) const {
    obj_raw().get_info(obj_info, fields);
}

NamedObj::info_t NamedObj::get_info(info_field_t fields) const {
    info_t info;
    get_info(info, fields);
    return info;
}

void NamedObj::get_attr_info(const string &obj_name, 
    const string &attr_name, attr_info_t &info, const Proplist &laprop) const
{
    _Attr::get_info_by_name(raw(), obj_name.c_str(), attr_name.c_str(),
        info, laprop.raw());
}

void NamedObj::get_attr_info(const string &obj_name, hsize_t idx, 
    attr_info_t &info, index_t idx_type, 
    iter_order_t order, const Proplist &laprop) const
{
    _Attr::get_info_by_idx(raw(), obj_name.c_str(), idx_type, 
        order, idx, info, laprop.raw());
}

NamedObj::attr_info_t NamedObj::get_attr_info(const string &obj_name, 
    const string &attr_name, const Proplist &laprop) const
{
    attr_info_t info;
    get_attr_info(obj_name, attr_name, info, laprop);
    return info;
}

NamedObj::attr_info_t NamedObj::get_attr_info(const string &obj_name, 
    hsize_t idx, index_t idx_type, iter_order_t order, 
    const Proplist &laprop) const
{
    attr_info_t info;
    get_attr_info(obj_name, idx, info, idx_type, order, laprop);
    return info;
}

string NamedObj::get_attr_name(const string &obj_name, hsize_t idx,
    index_t idx_type, iter_order_t order, 
    const Proplist &laprop) const
{
    return _Attr::get_name_by_idx(raw(), obj_name.c_str(), 
        idx_type, order, idx, laprop.raw());
}

bool NamedObj::attr_exists(const string &name) const {
    return (bool)_Attr::exists(raw(), name.c_str());
}

void NamedObj::rename_attr(const string &old_name, const string &new_name)
{
    _Attr::rename(raw(), old_name.c_str(), new_name.c_str());
}

void NamedObj::delete_attr(const string &name) {
    _Attr::delete_(raw(), name.c_str());
}

void NamedObj::delete_attr(const string &obj_name, const string &attr_name, 
    const Proplist &laprop)
{
    _Attr::delete_by_name(raw(), obj_name.c_str(), attr_name.c_str(), 
        laprop.raw());
}

void NamedObj::delete_attr(const string &obj_name, hsize_t idx,         
    index_t idx_type, iter_order_t order, const Proplist &laprop)
{
    _Attr::delete_by_idx(raw(), obj_name.c_str(), idx_type, order, 
        idx, laprop.raw());
}

herr_t NamedObj::attr_iterate(hsize_t &idx, attr_iter_op_t op, void *op_data,
    index_t idx_type, iter_order_t order) const
{
    using namespace _named_obj_attr_helper;
    iter_data_t data {std::move(op), op_data};
    return _Attr::iterate(raw(), idx_type, order, idx, raw_op, &data);
}

herr_t NamedObj::attr_iterate(const string &obj_name, hsize_t &idx, 
    attr_iter_op_t op, void *op_data,
    index_t idx_type, iter_order_t order,
    const Proplist &laprop) const
{
    using namespace _named_obj_attr_helper;
    iter_data_t data {std::move(op), op_data};
    return _Attr::iterate_by_name(raw(), obj_name.c_str(), idx_type,
        order, idx, raw_op, &data, laprop.raw());
}

AttrManager NamedObj::attrs() noexcept {
    return AttrManager{*this};
}

Attr NamedObj::create_attr(const string &name, 
    const Datatype &dtype, const Dataspace &dspace, 
    const string &flag, const Proplist &cprop, 
    const Proplist &aprop)
{
    Attr ret = _attr_create_detect_exists(name, flag, aprop);
    if( ret.has_referenced() ) 
        return ret;
    
    auto raw_attr = 
        _Attr::create(raw(), name.c_str(), dtype.raw(), dspace.raw(), 
        cprop.raw(), aprop.raw());
    
    return Attr::_from_raw(raw_attr);
}

Attr NamedObj::create_attr_str(const string &name, size_t n, 
    const string &flag)
{
    return create_attr(name, C_S1_T.resized(n), Dataspace::vSCALAR, flag);
}

Attr NamedObj::create_attr_str(const string &name, size_t n_str, 
    size_t n, const string &flag)
{
    return create_attr(name, C_S1_T.resized(n), {(hsize_t)n_str}, flag);
}

Attr NamedObj::create_attr_for_str(const string &name, const string &s, 
    const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr{s};
    return create_attr(name, dt, dsp, flag);
}

Attr NamedObj::create_attr_for_str(const string &name, const char *s, 
    const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr{s};
    return create_attr(name, dt, dsp, flag);
}

Attr NamedObj::create_attr_for_str(const string &name, 
    const vector<string> &ss, const string &flag)
{
    auto [buff, dsp, dt] = ConstDatapacketStr{ss};
    return create_attr(name, dt, dsp, flag);
}

Attr NamedObj::open_attr(const string &name, const Proplist &aprop) const {
    auto attr_id = Attr::_obj_raw_t::open(raw(), name.c_str(), aprop.raw());
    return Attr::_from_raw(attr_id);
}

AttrManager::AttrManager() noexcept : _obj() {}

AttrManager::AttrManager(refobj_t obj) noexcept
: _obj( std::move(obj) ) {}

auto AttrManager::refobj() const -> refobj_t { return _obj; }

void AttrManager::reset(refobj_t obj) noexcept { _obj = obj; }

void AttrManager::reset() noexcept { _obj.free(); }

bool AttrManager::exists(const string &name) {
    return _obj.attr_exists(name);
}

Attr AttrManager::open(const string &name) {
    return _obj.open_attr(name);
}

} // namespace HIPP::IO::H5
