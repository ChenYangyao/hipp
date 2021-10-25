#include <hippio.h>

namespace HIPP::IO::H5 {

AttrManager NamedObj::attrs() noexcept {
    return AttrManager{*this};
}

auto NamedObj::attr_exists(const string &name) const -> bool {
    return (bool)_Attr::exists(raw(), name.c_str());
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
